/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** ifyou have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "highlighter.h"
#include "highlightdefinition.h"
#include "context.h"
#include "rule.h"
#include "itemdata.h"
#include "highlighterexception.h"
#include "progressdata.h"
#include "reuse.h"
#include "tabsettings.h"
#include <QDebug>

#include <QtCore/QLatin1String>
#include <QtCore/QLatin1Char>

using namespace TextEditor;
using namespace Internal;

namespace {
	static const QLatin1String kStay("#stay");
	static const QLatin1String kPop("#pop");
	static const QLatin1Char kBackSlash('\\');
	static const QLatin1Char kHash('#');
}

const Highlighter::KateFormatMap Highlighter::gKateFormats;

Highlighter::Highlighter(QTextDocument *parent) :
	TextEditor::SyntaxHighlighter(parent),
	mRegionDepth(0),
	mIndentationBasedFolding(false),
	mTabSettings(0),
	mPersistentObservableStatesCounter(PersistentsStart),
	mDynamicContextsCounter(0),
	mRegions(0),
	mIsBroken(false)
{}

Highlighter::~Highlighter()
{}

Highlighter::BlockData::BlockData() : TextBlockUserData(), mFoldingIndentDelta(0), mOriginalObservableState(-1)
{}

Highlighter::BlockData::~BlockData()
{}

Highlighter::KateFormatMap::KateFormatMap() {
	mIds.insert(QLatin1String("dsNormal"), Highlighter::Normal);
	mIds.insert(QLatin1String("dsKeyword"), Highlighter::Keyword);
	mIds.insert(QLatin1String("dsDataType"), Highlighter::DataType);
	mIds.insert(QLatin1String("dsDecVal"), Highlighter::Decimal);
	mIds.insert(QLatin1String("dsBaseN"), Highlighter::BaseN);
	mIds.insert(QLatin1String("dsFloat"), Highlighter::Float);
	mIds.insert(QLatin1String("dsChar"), Highlighter::Char);
	mIds.insert(QLatin1String("dsString"), Highlighter::String);
	mIds.insert(QLatin1String("dsComment"), Highlighter::Comment);
	mIds.insert(QLatin1String("dsOthers"), Highlighter::Others);
	mIds.insert(QLatin1String("dsAlert"), Highlighter::Alert);
	mIds.insert(QLatin1String("dsFunction"), Highlighter::Function);
	mIds.insert(QLatin1String("dsRegionMarker"), Highlighter::RegionMarker);
	mIds.insert(QLatin1String("dsError"), Highlighter::Error);
}

void Highlighter::configureFormat(TextFormatId id, const QTextCharFormat &format) {
	mCreatorFormats[id] = format;
}

const QTextCharFormat&Highlighter::getFormat(Highlighter::TextFormatId id) const {
	return mCreatorFormats[id];
}

void  Highlighter::setDefaultContext(const QSharedPointer<Context> &defaultContext) {
	mDefaultContext = defaultContext;
	mPersistentObservableStates.insert(mDefaultContext->name(), Default);
	mIndentationBasedFolding = defaultContext->definition()->isIndentationBasedFolding();
}

const QSharedPointer<HighlightDefinition>& Highlighter::getDefinition() const {
	return mDefaultContext->definition();
}


void Highlighter::setTabSettings(const TabSettings &ts) {
	mTabSettings = &ts;
}

void Highlighter::highlightBlock(const QString &text) {

	if(currentBlock().firstLineNumber() == 0)
		mRegions = 0;

	if(!mDefaultContext.isNull() && !mIsBroken) {
		try {
			if(!currentBlockUserData())
				initializeBlockData();
			setupDataForBlock(text);

			handleContextChange(mCurrentContext->lineBeginContext(),
					    mCurrentContext->definition());

			ProgressData progress;
			const int length = text.length();
			while(progress.offset() < length)
				iterateThroughRules(text, length, &progress, false, mCurrentContext->rules());

			handleContextChange(mCurrentContext->lineEndContext(),
					    mCurrentContext->definition(),
					    false);
			mContexts.clear();

			if(mIndentationBasedFolding) {
				applyIndentationBasedFolding(text);
			} else {
				applyRegionBasedFolding();

				// In the case region depth has changed since the last time the state was set.
				setCurrentBlockState(computeState(extractObservableState(currentBlockState())));
			}
		} catch (const HighlighterException &) {
			mIsBroken = true;
		}
	}

	applyFormatToSpaces(text, mCreatorFormats.value(VisualWhitespace));
}

void Highlighter::setupDataForBlock(const QString &text) {
	if(extractObservableState(currentBlockState()) == WillContinue)
		analyseConsistencyOfWillContinueBlock(text);

	if(previousBlockState() == -1) {
		mRegionDepth = 0;
		setupDefault();
	} else {
		mRegionDepth = extractRegionDepth(previousBlockState());
		const int observablePreviousState = extractObservableState(previousBlockState());
		if(observablePreviousState == Default)
			setupDefault();
		else if(observablePreviousState == WillContinue)
			setupFromWillContinue();
		else if(observablePreviousState == Continued)
			setupFromContinued();
		else
			setupFromPersistent();

		blockData(currentBlockUserData())->mFoldingRegions =
				blockData(currentBlock().previous().userData())->mFoldingRegions;
		blockData(currentBlockUserData())->setRegionId(blockData(currentBlock().previous().userData())->getRegionId());
	}

	assignCurrentContext();
}

void Highlighter::setupDefault() {
	mContexts.push_back(mDefaultContext);

	setCurrentBlockState(computeState(Default));
}

void Highlighter::setupFromWillContinue() {
	BlockData *previousData = blockData(currentBlock().previous().userData());
	mContexts.push_back(previousData->mContextToContinue);

	BlockData *data = blockData(currentBlock().userData());
	data->mOriginalObservableState = previousData->mOriginalObservableState;

	if(currentBlockState() == -1 || extractObservableState(currentBlockState()) == Default)
		setCurrentBlockState(computeState(Continued));
}

void Highlighter::setupFromContinued() {
	BlockData *previousData = blockData(currentBlock().previous().userData());

	Q_ASSERT(previousData->mOriginalObservableState != WillContinue &&
			previousData->mOriginalObservableState != Continued);

	if(previousData->mOriginalObservableState == Default ||
	    previousData->mOriginalObservableState == -1) {
		mContexts.push_back(mDefaultContext);
	} else {
		pushContextSequence(previousData->mOriginalObservableState);
	}

	setCurrentBlockState(computeState(previousData->mOriginalObservableState));
}

void Highlighter::setupFromPersistent() {
	pushContextSequence(extractObservableState(previousBlockState()));

	setCurrentBlockState(previousBlockState());
}

void Highlighter::iterateThroughRules(const QString &text,
				      const int length,
				      ProgressData *progress,
				      const bool childRule,
				      const QList<QSharedPointer<Rule> > &rules) {
	typedef QList<QSharedPointer<Rule> >::const_iterator RuleIterator;

	bool contextChanged = false;
	bool atLeastOneMatch = false;

	RuleIterator it = rules.begin();
	RuleIterator endIt = rules.end();
	while(it != endIt && progress->offset() < length) {
		int startOffset = progress->offset();
		const QSharedPointer<Rule> &rule = *it;
		if(rule->matchSucceed(text, length, progress)) {
			atLeastOneMatch = true;

			if(!mIndentationBasedFolding) {
				BlockData* curBlockData = blockData(currentBlockUserData());
				QTextBlock prevBlock = currentBlock().previous();
				BlockData* prevBlockData = nullptr;
				if(prevBlock.isValid())
					prevBlockData = blockData(currentBlock().previous().userData());

				if(!rule->beginRegion().isEmpty()) {
					curBlockData->mFoldingRegions.push(rule->beginRegion());
					++mRegionDepth;
					++mRegions;
					curBlockData->setRegionId(mRegions);
					if(progress->isOpeningBraceMatchAtFirstNonSpace())
						++curBlockData->mFoldingIndentDelta;
				}
				if(!rule->endRegion().isEmpty()) {
					QStack<QString> *currentRegions =
							&curBlockData->mFoldingRegions;
					if(!currentRegions->isEmpty() && rule->endRegion() == currentRegions->top()) {
						currentRegions->pop();
						--mRegionDepth;

						if(prevBlockData != nullptr) curBlockData->setRegionId(prevBlockData->getRegionId()-1);

						if(progress->isClosingBraceMatchAtNonEnd())
							--curBlockData->mFoldingIndentDelta;
					}
				}
				progress->clearBracesMatches();
			}

			if(progress->isWillContinueLine()) {
				createWillContinueBlock();
				progress->setWillContinueLine(false);
			} else {
				if(rule->hasChildren())
					iterateThroughRules(text, length, progress, true, rule->children());

				if(!rule->context().isEmpty() && contextChangeRequired(rule->context())) {
					mCurrentCaptures = progress->captures();
					changeContext(rule->context(), rule->definition());
					contextChanged = true;
				}
			}

			// Format is not applied to child rules directly (but relative to the offset of their
			// parent) nor to look ahead rules.
			if(!childRule && !rule->isLookAhead()) {
				if(rule->itemData().isEmpty())
					applyFormat(startOffset, progress->offset() - startOffset,
						    mCurrentContext->itemData(), mCurrentContext->definition());
				else
					applyFormat(startOffset, progress->offset() - startOffset, rule->itemData(),
						    rule->definition());
			}

			// When there is a match of one child rule the others should be skipped. Otherwise
			// the highlighting would be incorret in a case like 9ULLLULLLUULLULLUL, for example.
			if(contextChanged || childRule) {
				break;
			} else {
				it = rules.begin();
				continue;
			}
		}
		++it;
	}

	if(!childRule && !atLeastOneMatch) {
		if(mCurrentContext->isFallthrough()) {
			handleContextChange(mCurrentContext->fallthroughContext(),
					    mCurrentContext->definition());
			iterateThroughRules(text, length, progress, false, mCurrentContext->rules());
		} else {
			applyFormat(progress->offset(), 1, mCurrentContext->itemData(),
				    mCurrentContext->definition());
			if(progress->isOnlySpacesSoFar() && !text.at(progress->offset()).isSpace())
				progress->setOnlySpacesSoFar(false);
			progress->incrementOffset();
		}
	}
}

bool Highlighter::contextChangeRequired(const QString &contextName) const {
	if(contextName == kStay)
		return false;
	return true;
}

void Highlighter::changeContext(const QString &contextName,
				const QSharedPointer<HighlightDefinition> &definition,
				const bool assignCurrent) {
	if(contextName.startsWith(kPop)) {
		QStringList list = contextName.split(kHash, QString::SkipEmptyParts);
		for (int i = 0; i < list.size(); ++i)
			mContexts.pop_back();

		if(extractObservableState(currentBlockState()) >= PersistentsStart) {
			// One or more contexts were popped during during a persistent state.
			const QString &currentSequence = currentContextSequence();
			if(mPersistentObservableStates.contains(currentSequence))
				setCurrentBlockState(
							computeState(mPersistentObservableStates.value(currentSequence)));
			else
				setCurrentBlockState(
							computeState(mLeadingObservableStates.value(currentSequence)));
		}
	} else {
		const QSharedPointer<Context> &context = definition->context(contextName);

		if(context->isDynamic())
			pushDynamicContext(context);
		else
			mContexts.push_back(context);

		if(mContexts.back()->lineEndContext() == kStay ||
		    extractObservableState(currentBlockState()) >= PersistentsStart) {
			const QString &currentSequence = currentContextSequence();
			mapLeadingSequence(currentSequence);
			if(mContexts.back()->lineEndContext() == kStay) {
				// A persistent context was pushed.
				mapPersistentSequence(currentSequence);
				setCurrentBlockState(
							computeState(mPersistentObservableStates.value(currentSequence)));
			}
		}
	}

	if(assignCurrent)
		assignCurrentContext();
}

void Highlighter::handleContextChange(const QString &contextName,
				      const QSharedPointer<HighlightDefinition> &definition,
				      const bool setCurrent) {
	if(!contextName.isEmpty() && contextChangeRequired(contextName))
		changeContext(contextName, definition, setCurrent);
}

void Highlighter::applyFormat(int offset,
			      int count,
			      const QString &itemDataName,
			      const QSharedPointer<HighlightDefinition> &definition) {
	if(count == 0)
		return;

	QSharedPointer<ItemData> itemData;
	try {
		itemData = definition->itemData(itemDataName);
	} catch (const HighlighterException &) {
		// There are some broken files. For instance, the Printf context in java.xml points to an
		// inexistent Printf item data. These cases are considered to have normal text style.
		return;
	}

	TextFormatId formatId = gKateFormats.mIds.value(itemData->style());
	if(formatId != Normal) {
		QHash<TextFormatId, QTextCharFormat>::const_iterator cit =
				mCreatorFormats.constFind(formatId);
		if(cit != mCreatorFormats.constEnd()) {
			QTextCharFormat format = cit.value();
			if(itemData->isCustomized()) {
				// Please notice that the following are applied every time for item datas which have
				// customizations. The configureFormats method could be used to provide a "one time"
				// configuration, but it would probably require to traverse all item datas from all
				// definitions available/loaded (either to set the values or for some "notifying"
				// strategy). This is because the highlighter does not really know on which
				// definition(s) it is working. Since not many item datas specify customizations I
				// think this approach would fit better. ifthere are other ideas...
				if(itemData->color().isValid())
					format.setForeground(itemData->color());
				if(itemData->isItalicSpecified())
					format.setFontItalic(itemData->isItalic());
				if(itemData->isBoldSpecified())
					format.setFontWeight(toFontWeight(itemData->isBold()));
				if(itemData->isUnderlinedSpecified())
					format.setFontUnderline(itemData->isUnderlined());
				if(itemData->isStrikeOutSpecified())
					format.setFontStrikeOut(itemData->isStrikeOut());
			}

			setFormat(offset, count, format);
		}
	}
}

void Highlighter::createWillContinueBlock() {
	BlockData *data = blockData(currentBlockUserData());
	const int currentObservableState = extractObservableState(currentBlockState());
	if(currentObservableState == Continued) {
		BlockData *previousData = blockData(currentBlock().previous().userData());
		data->mOriginalObservableState = previousData->mOriginalObservableState;
	} else if(currentObservableState != WillContinue) {
		data->mOriginalObservableState = currentObservableState;
	}
	data->mContextToContinue = mCurrentContext;

	setCurrentBlockState(computeState(WillContinue));
}

void Highlighter::analyseConsistencyOfWillContinueBlock(const QString &text) {
	if(currentBlock().next().isValid() && (
		    text.length() == 0 || text.at(text.length() - 1) != kBackSlash) &&
	    extractObservableState(currentBlock().next().userState()) != Continued) {
		currentBlock().next().setUserState(computeState(Continued));
	}

	if(text.length() == 0 || text.at(text.length() - 1) != kBackSlash) {
		BlockData *data = blockData(currentBlockUserData());
		data->mContextToContinue.clear();
		setCurrentBlockState(computeState(data->mOriginalObservableState));
	}
}

void Highlighter::mapPersistentSequence(const QString &contextSequence) {
	if(!mPersistentObservableStates.contains(contextSequence)) {
		int newState = mPersistentObservableStatesCounter;
		mPersistentObservableStates.insert(contextSequence, newState);
		mPersistentContexts.insert(newState, mContexts);
		++mPersistentObservableStatesCounter;
	}
}

void Highlighter::mapLeadingSequence(const QString &contextSequence) {
	if(!mLeadingObservableStates.contains(contextSequence))
		mLeadingObservableStates.insert(contextSequence,
						 extractObservableState(currentBlockState()));
}

void Highlighter::pushContextSequence(int state) {
	const QVector<QSharedPointer<Context> > &contexts = mPersistentContexts.value(state);
	for (int i = 0; i < contexts.size(); ++i)
		mContexts.push_back(contexts.at(i));
}

QString Highlighter::currentContextSequence() const {
	QString sequence;
	for (int i = 0; i < mContexts.size(); ++i)
		sequence.append(mContexts.at(i)->id());

	return sequence;
}

Highlighter::BlockData *Highlighter::initializeBlockData() {
	BlockData *data = new BlockData;
	setCurrentBlockUserData(data);
	return data;
}

Highlighter::BlockData *Highlighter::blockData(QTextBlockUserData *userData) {
	return static_cast<BlockData *>(userData);
}

void Highlighter::pushDynamicContext(const QSharedPointer<Context> &baseContext) {
	// A dynamic context is created from another context which serves as its basis. Then,
	// its rules are updated according to the captures from the calling regular expression which
	// triggered the push of the dynamic context.
	QSharedPointer<Context> context(new Context(*baseContext));
	context->configureId(mDynamicContextsCounter);
	context->updateDynamicRules(mCurrentCaptures);
	mContexts.push_back(context);
	++mDynamicContextsCounter;
}

void Highlighter::assignCurrentContext() {
	if(mContexts.isEmpty()) {
		// This is not supposed to happen. However, there are broken files (for example, php.xml)
		// which will cause this behaviour. In such cases pushing the default context is enough to
		// keep highlighter working.
		mContexts.push_back(mDefaultContext);
	}
	mCurrentContext = mContexts.back();
}

int Highlighter::extractRegionDepth(const int state) {
	return state >> 12;
}

int Highlighter::extractObservableState(const int state) {
	return state & 0xFFF;
}

int Highlighter::computeState(const int observableState) const {
	return mRegionDepth << 12 | observableState;
}

void Highlighter::applyRegionBasedFolding() const {
	int folding = 0;
	BlockData *data = blockData(currentBlockUserData());
	BlockData *previousData = blockData(currentBlock().previous().userData());
	if(previousData) {
		folding = extractRegionDepth(previousBlockState());
		if(data->mFoldingIndentDelta != 0) {
			folding += data->mFoldingIndentDelta;

			if(data->mFoldingIndentDelta > 0) {
				data->setFoldingStartIncluded(true);
			} else {
				previousData->setFoldingEndIncluded(false);
			}
			data->mFoldingIndentDelta = 0;
		}
	}
	data->setFoldingEndIncluded(true);
	data->setFoldingIndent(folding);
}

void Highlighter::applyIndentationBasedFolding(const QString &text) const {
	BlockData *data = blockData(currentBlockUserData());
	data->setFoldingEndIncluded(true);

	// ifthis line is empty, check its neighbours. They all might be part of the same block.
	if(text.trimmed().isEmpty()) {
		data->setFoldingIndent(0);
		const int previousIndent = neighbouringNonEmptyBlockIndent(currentBlock().previous(), true);
		if(previousIndent > 0) {
			const int nextIndent = neighbouringNonEmptyBlockIndent(currentBlock().next(), false);
			if(previousIndent == nextIndent)
				data->setFoldingIndent(previousIndent);
		}
	} else {
		data->setFoldingIndent(mTabSettings->indentationColumn(text));
	}
}

int Highlighter::neighbouringNonEmptyBlockIndent(QTextBlock block, const bool previous) const {
	while(true) {
		if(!block.isValid())
			return 0;
		if(block.text().trimmed().isEmpty()) {
			if(previous)
				block = block.previous();
			else
				block = block.next();
		} else {
			return mTabSettings->indentationColumn(block.text());
		}
	}
}
