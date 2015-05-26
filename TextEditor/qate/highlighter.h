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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "basetextdocumentlayout.h"
#include "syntaxhighlighter.h"

#include <QString>
#include <QVector>
#include <QStack>
#include <QSharedPointer>
#include <QStringList>

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace TextEditor {

	class TabSettings;

	namespace Internal {

		class Rule;
		class Context;
		class HighlightDefinition;
		class ProgressData;

		class Highlighter : public TextEditor::SyntaxHighlighter
		{
				Q_OBJECT

			public:
				Highlighter(QTextDocument *parent = 0);
				virtual ~Highlighter();

				enum TextFormatId {
					Normal,
					VisualWhitespace,
					Keyword,
					DataType,
					Decimal,
					BaseN,
					Float,
					Char,
					String,
					Comment,
					Alert,
					Error,
					Function,
					RegionMarker,
					Others
				};

				void configureFormat(TextFormatId id, const QTextCharFormat &format);
				const QTextCharFormat& getFormat(TextFormatId) const;
				void setTabSettings(const TabSettings &ts);
				void setDefaultContext(const QSharedPointer<Context> &defaultContext);
				const QSharedPointer<HighlightDefinition>& getDefinition() const;

			protected:
				struct BlockData : TextBlockUserData {
						BlockData();
						virtual ~BlockData();

						int mFoldingIndentDelta;
						int mOriginalObservableState;
						QStack<QString> mFoldingRegions;
						QSharedPointer<Context> mContextToContinue;
				};


				virtual void highlightBlock(const QString &text);
				virtual BlockData *initializeBlockData();
				struct KateFormatMap
				{
						KateFormatMap();
						QHash<QString, TextFormatId> mIds;
				};
				static const KateFormatMap gKateFormats;
				QHash<TextFormatId, QTextCharFormat> mCreatorFormats;


			private:

				void setupDataForBlock(const QString &text);
				void setupDefault();
				void setupFromWillContinue();
				void setupFromContinued();
				void setupFromPersistent();

				void iterateThroughRules(const QString &text,
							 const int length,
							 ProgressData *progress,
							 const bool childRule,
							 const QList<QSharedPointer<Rule> > &rules);

				void assignCurrentContext();
				bool contextChangeRequired(const QString &contextName) const;
				void handleContextChange(const QString &contextName,
							 const QSharedPointer<HighlightDefinition> &definition,
							 const bool setCurrent = true);
				void changeContext(const QString &contextName,
						   const QSharedPointer<HighlightDefinition> &definition,
						   const bool assignCurrent = true);

				QString currentContextSequence() const;
				void mapPersistentSequence(const QString &contextSequence);
				void mapLeadingSequence(const QString &contextSequence);
				void pushContextSequence(int state);

				void pushDynamicContext(const QSharedPointer<Context> &baseContext);

				void createWillContinueBlock();
				void analyseConsistencyOfWillContinueBlock(const QString &text);

				void applyFormat(int offset,
						 int count,
						 const QString &itemDataName,
						 const QSharedPointer<HighlightDefinition> &definition);

				void applyRegionBasedFolding() const;
				void applyIndentationBasedFolding(const QString &text) const;
				int neighbouringNonEmptyBlockIndent(QTextBlock block, const bool previous) const;

				// Mapping from Kate format strings to format ids.



				static BlockData *blockData(QTextBlockUserData *userData);

				// Block states are composed by the region depth (used for code folding) and what I call
				// observable states. Observable states occupy the 12 least significant bits. They might have
				// the following values:
				// - Default [0]: Nothing special.
				// - WillContinue [1]: When there is match of the LineContinue rule (backslash as the last
				//   character).
				// - Continued [2]: Blocks that happen after a WillContinue block and continue from their
				//   context until the next line end.
				// - Persistent(s) [Anything >= 3]: Correspond to persistent contexts which last until a pop
				//   occurs due to a matching rule. Every sequence of persistent contexts seen so far is
				//   associated with a number (incremented by a unit each time).
				// Region depths occupy the remaining bits.
				enum ObservableBlockState {
					Default = 0,
					WillContinue,
					Continued,
					PersistentsStart
				};
				int computeState(const int observableState) const;

				static int extractRegionDepth(const int state);
				static int extractObservableState(const int state);

				int mRegionDepth;
				mutable int mRegions;
				bool mIndentationBasedFolding;
				const TabSettings *mTabSettings;

				int mPersistentObservableStatesCounter;
				int mDynamicContextsCounter;

				bool mIsBroken;

				QSharedPointer<Context> mDefaultContext;
				QSharedPointer<Context> mCurrentContext;
				QVector<QSharedPointer<Context> > mContexts;

				// Mapping from context sequences to the observable persistent state they represent.
				QHash<QString, int> mPersistentObservableStates;
				// Mapping from context sequences to the non-persistent observable state that led to them.
				QHash<QString, int> mLeadingObservableStates;
				// Mapping from observable persistent states to context sequences (the actual "stack").
				QHash<int, QVector<QSharedPointer<Context> > > mPersistentContexts;

				// Captures used in dynamic rules.
				QStringList mCurrentCaptures;
		};

	} // namespace Internal
} // namespace TextEditor

#endif // HIGHLIGHTER_H
