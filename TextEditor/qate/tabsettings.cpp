
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "tabsettings.h"

#include <QDebug>
#include <QSettings>
#include <QString>
#include <QTextCursor>
#include <QTextDocument>

static const char spacesForTabsKey[] = "SpacesForTabs";
static const char autoSpacesForTabsKey[] = "AutoSpacesForTabs";
static const char tabSizeKey[] = "TabSize";
static const char indentSizeKey[] = "IndentSize";
static const char groupPostfix[] = "TabSettings";
static const char paddingModeKey[] = "PaddingMode";

namespace TextEditor {

	TabSettings::TabSettings() :
		mTabPolicy(SpacesOnlyTabPolicy),
		mTabSize(8),
		mIndentSize(4),
		mContinuationAlignBehavior(ContinuationAlignWithSpaces)
	{
	}

	TabSettings::TabSettings(const TabSettings& b)  :
		mTabPolicy(b.mTabPolicy),
		mTabSize(b.mTabSize),
		mIndentSize(b.mIndentSize),
		mContinuationAlignBehavior(b.mContinuationAlignBehavior)
	{
	}

	bool TabSettings::cursorIsAtBeginningOfLine(const QTextCursor &cursor) {
		QString text = cursor.block().text();
		int fns = firstNonSpace(text);
		return (cursor.position() - cursor.block().position() <= fns);
	}

	int TabSettings::lineIndentPosition(const QString &text) const {
		int i = 0;
		while(i < text.size()) {
			if(!text.at(i).isSpace())
				break;
			++i;
		}
		int column = columnAt(text, i);
		return i - (column % mIndentSize);
	}

	int TabSettings::firstNonSpace(const QString &text) {
		int i = 0;
		while(i < text.size()) {
			if(!text.at(i).isSpace())
				return i;
			++i;
		}
		return i;
	}

	QString TabSettings::indentationString(const QString &text) const {
		return text.left(firstNonSpace(text));
	}


	int TabSettings::indentationColumn(const QString &text) const {
		return columnAt(text, firstNonSpace(text));
	}

	int TabSettings::maximumPadding(const QString &text) {
		int fns = firstNonSpace(text);
		int i = fns;
		while(i > 0) {
			if(text.at(i-1) != QLatin1Char(' '))
				break;
			--i;
		}
		return fns - i;
	}


	int TabSettings::trailingWhitespaces(const QString &text) {
		int i = 0;
		while(i < text.size()) {
			if(!text.at(text.size()-1-i).isSpace())
				return i;
			++i;
		}
		return i;
	}

	void TabSettings::removeTrailingWhitespace(QTextCursor cursor, QTextBlock &block) {
		if(const int trailing = trailingWhitespaces(block.text())) {
			cursor.setPosition(block.position() + block.length() - 1);
			cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, trailing);
			cursor.removeSelectedText();
		}
	}

	bool TabSettings::isIndentationClean(const QTextBlock &block) const {
		int i = 0;
		int spaceCount = 0;
		QString text = block.text();
		bool spacesForTabs = guessSpacesForTabs(block);
		while(i < text.size()) {
			QChar c = text.at(i);
			if(!c.isSpace())
				return true;

			if(c == QLatin1Char(' ')) {
				++spaceCount;
				if(!spacesForTabs && spaceCount == mTabSize)
					return false;
			} else if(c == QLatin1Char('\t')) {
				if(spacesForTabs || spaceCount != 0)
					return false;
			}
			++i;
		}
		return true;
	}

	int TabSettings::columnAt(const QString &text, int position) const {
		int column = 0;
		for (int i = 0; i < position; ++i) {
			if(text.at(i) == QLatin1Char('\t'))
				column = column - (column % mTabSize) + mTabSize;
			else
				++column;
		}
		return column;
	}

	int TabSettings::positionAtColumn(const QString &text, int column, int *offset) const {
		int col = 0;
		int i = 0;
		while(i < text.size() && col < column) {
			if(text.at(i) == QLatin1Char('\t'))
				col = col - (col % mTabSize) + mTabSize;
			else
				++col;
			++i;
		}
		if(offset)
			*offset = column - col;
		return i;
	}

	int TabSettings::spacesLeftFromPosition(const QString &text, int position) {
		int i = position;
		while(i > 0) {
			if(!text.at(i-1).isSpace())
				break;
			--i;
		}
		return position - i;
	}

	int TabSettings::indentedColumn(int column, bool doIndent) const {
		int aligned = (column / mIndentSize) * mIndentSize;
		if(doIndent)
			return aligned + mIndentSize;
		if(aligned < column)
			return aligned;
		return qMax(0, aligned - mIndentSize);
	}

	bool TabSettings::guessSpacesForTabs(const QTextBlock &_block) const {
		if(mTabPolicy == MixedTabPolicy && _block.isValid()) {
			const QTextDocument *doc = _block.document();
			QVector<QTextBlock> currentBlocks(2, _block); // [0] looks back; [1] looks forward
			int maxLookAround = 100;
			while(maxLookAround-- > 0) {
				if(currentBlocks.at(0).isValid())
					currentBlocks[0] = currentBlocks.at(0).previous();
				if(currentBlocks.at(1).isValid())
					currentBlocks[1] = currentBlocks.at(1).next();
				bool done = true;
				foreach (const QTextBlock &block, currentBlocks) {
					if(block.isValid())
						done = false;
					if(!block.isValid() || block.length() == 0)
						continue;
					const QChar firstChar = doc->characterAt(block.position());
					if(firstChar == QLatin1Char(' '))
						return true;
					else if(firstChar == QLatin1Char('\t'))
						return false;
				}
				if(done)
					break;
			}
		}
		return mTabPolicy != TabsOnlyTabPolicy;
	}

	QString TabSettings::indentationString(int startColumn, int targetColumn, const QTextBlock &block) const {
		targetColumn = qMax(startColumn, targetColumn);
		if(guessSpacesForTabs(block))
			return QString(targetColumn - startColumn, QLatin1Char(' '));

		QString s;
		int alignedStart = startColumn - (startColumn % mTabSize) + mTabSize;
		if(alignedStart > startColumn && alignedStart <= targetColumn) {
			s += QLatin1Char('\t');
			startColumn = alignedStart;
		}
		if(int columns = targetColumn - startColumn) {
			int tabs = columns / mTabSize;
			s += QString(tabs, QLatin1Char('\t'));
			s += QString(columns - tabs * mTabSize, QLatin1Char(' '));
		}
		return s;
	}

	void TabSettings::indentLine(QTextBlock block, int newIndent, int padding) const {
		const QString text = block.text();
		const int oldBlockLength = text.size();

		if(mContinuationAlignBehavior == NoContinuationAlign) {
			newIndent -= padding;
			padding = 0;
		} else if(mContinuationAlignBehavior == ContinuationAlignWithIndent) {
			padding = 0;
		}

		// Quickly check whether indenting is required.
		// fixme: after changing "use spaces for tabs" the change was not reflected
		// because of the following optimisation. Commenting it out for now.
		//    if(indentationColumn(text) == newIndent)
		//        return;

		QString indentString;

		if(mTabPolicy == TabsOnlyTabPolicy) {
			// user likes tabs for spaces and uses tabs for indentation, preserve padding
			indentString = indentationString(0, newIndent - padding, block);
			indentString += QString(padding, QLatin1Char(' '));
		} else {
			indentString = indentationString(0, newIndent, block);
		}

		if(oldBlockLength == indentString.length() && text == indentString)
			return;

		QTextCursor cursor(block);
		cursor.beginEditBlock();
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, firstNonSpace(text));
		cursor.removeSelectedText();
		cursor.insertText(indentString);
		cursor.endEditBlock();
	}

	void TabSettings::reindentLine(QTextBlock block, int delta) const {
		const QString text = block.text();
		const int oldBlockLength = text.size();

		int oldIndent = indentationColumn(text);
		int newIndent = qMax(oldIndent + delta, 0);

		if(oldIndent == newIndent)
			return;

		QString indentString;
		if(mTabPolicy == TabsOnlyTabPolicy && mTabSize == mIndentSize) {
			// user likes tabs for spaces and uses tabs for indentation, preserve padding
			int padding = qMin(maximumPadding(text), newIndent);
			indentString = indentationString(0, newIndent - padding, block);
			indentString += QString(padding, QLatin1Char(' '));
		} else {
			indentString = indentationString(0, newIndent, block);
		}

		if(oldBlockLength == indentString.length() && text == indentString)
			return;

		QTextCursor cursor(block);
		cursor.beginEditBlock();
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, firstNonSpace(text));
		cursor.removeSelectedText();
		cursor.insertText(indentString);
		cursor.endEditBlock();
	}

	bool TabSettings::equals(const TabSettings &ts) const {
		return mTabPolicy == ts.mTabPolicy
				&& mTabSize == ts.mTabSize
				&& mIndentSize == ts.mIndentSize
				&& mContinuationAlignBehavior == ts.mContinuationAlignBehavior;
	}

} // namespace TextEditor
