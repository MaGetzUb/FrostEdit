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

#include "highlightdefinition.h"
#include "highlighterexception.h"
#include "context.h"
#include "keywordlist.h"
#include "itemdata.h"
#include "reuse.h"

#include <QString>

using namespace TextEditor;
using namespace Internal;

HighlightDefinition::HighlightDefinition() :
	mSingleLineCommentAfterWhiteSpaces(false),
	mKeywordCaseSensitivity(Qt::CaseSensitive),
	mIndentationBasedFolding(false)
{
	QString s(QLatin1String(".():!+,-<=>%&/;?[]^{|}~\\*, \t"));
	for(const QChar &c: s)
		mDelimiters.insert(c);
}

HighlightDefinition::~HighlightDefinition() {}

template <class Element, class Container>
QSharedPointer<Element> HighlightDefinition::
GenericHelper::create(const QString &name, Container &container) {
	if(name.isEmpty())
		throw HighlighterException();

	if(container.contains(name))
		throw HighlighterException();

	return container.insert(name, QSharedPointer<Element>(new Element)).value();
}

template <class Element, class Container>
QSharedPointer<Element> HighlightDefinition::
GenericHelper::find(const QString &name, const Container &container) const {
	typename Container::const_iterator it = container.find(name);
	if(it == container.end())
		throw HighlighterException();

	return it.value();
}

QSharedPointer<KeywordList> HighlightDefinition::createKeywordList(const QString &list) {
	return mHelper.create<KeywordList>(list, mLists);
}

QSharedPointer<KeywordList> HighlightDefinition::keywordList(const QString &list) {
	return mHelper.find<KeywordList>(list, mLists);
}

QSharedPointer<Context> HighlightDefinition::createContext(const QString &context, bool initial) {
	if(initial)
		mInitialContext = context;

	QSharedPointer<Context> newContext = mHelper.create<Context>(context, mContexts);
	newContext->setName(context);
	return newContext;
}

QSharedPointer<Context> HighlightDefinition::initialContext() const {
	return mHelper.find<Context>(mInitialContext, mContexts);
}

QSharedPointer<Context> HighlightDefinition::context(const QString &context) const {
	return mHelper.find<Context>(context, mContexts);
}

const QHash<QString, QSharedPointer<Context> > &HighlightDefinition::contexts() const {
	return mContexts;
}

const QHash<QString, QSharedPointer<KeywordList> >&HighlightDefinition::keywords() const {
	return mLists;
}

QSharedPointer<ItemData> HighlightDefinition::createItemData(const QString &itemData) {
	return mHelper.create<ItemData>(itemData, mItemsData);
}

QSharedPointer<ItemData> HighlightDefinition::itemData(const QString &itemData) const {
	return mHelper.find<ItemData>(itemData, mItemsData);
}

void HighlightDefinition::setSingleLineComment(const QString &start) {
	mSingleLineComment = start;
}

const QString &HighlightDefinition::singleLineComment() const {
	return mSingleLineComment;
}

void HighlightDefinition::setCommentAfterWhitespaces(const QString &after) {
	if(after == QLatin1String("afterwhitespace"))
		mSingleLineCommentAfterWhiteSpaces = true;
}

bool HighlightDefinition::isCommentAfterWhiteSpaces() const {
	return mSingleLineCommentAfterWhiteSpaces;
}

void HighlightDefinition::setMultiLineCommentStart(const QString &start) {
	mMultiLineCommentStart = start;
}

const QString &HighlightDefinition::multiLineCommentStart() const {
	return mMultiLineCommentStart;
}

void HighlightDefinition::setMultiLineCommentEnd(const QString &end) {
	mMultiLineCommentEnd = end;
}

const QString &HighlightDefinition::multiLineCommentEnd() const {
	return mMultiLineCommentEnd;
}

void HighlightDefinition::setMultiLineCommentRegion(const QString &region) {
	mMultiLineCommentRegion = region;
}

const QString &HighlightDefinition::multiLineCommentRegion() const {
	return mMultiLineCommentRegion;
}

void HighlightDefinition::removeDelimiters(const QString &characters) {
	for(int i = 0; i < characters.length(); ++i)
		mDelimiters.remove(characters.at(i));
}

void HighlightDefinition::addDelimiters(const QString &characters) {
	for(int i = 0; i < characters.length(); ++i) {
		if(!mDelimiters.contains(characters.at(i)))
			mDelimiters.insert(characters.at(i));
	}
}

bool HighlightDefinition::isDelimiter(const QChar &character) const {
	if(mDelimiters.contains(character))
		return true;
	return false;
}

void HighlightDefinition::setKeywordsSensitive(const QString &sensitivity) {
	if(!sensitivity.isEmpty())
		mKeywordCaseSensitivity = toCaseSensitivity(toBool(sensitivity));
}

Qt::CaseSensitivity HighlightDefinition::keywordsSensitive() const{
	return mKeywordCaseSensitivity;
}

void HighlightDefinition::setIndentationBasedFolding(const QString &indentationBasedFolding) {
	mIndentationBasedFolding = toBool(indentationBasedFolding);
}

bool HighlightDefinition::isIndentationBasedFolding() const {
	return mIndentationBasedFolding;
}
