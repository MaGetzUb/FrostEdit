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

#include "itemdata.h"
#include "reuse.h"

using namespace TextEditor;
using namespace Internal;

ItemData::ItemData() :
	mItalic(false),
	mItalicSpecified(false),
	mBold(false),
	mBoldSpecified(false),
	mUnderlined(false),
	mUnderlinedSpecified(false),
	mStrikedOut(false),
	mStrikeOutSpecified(false),
	mIsCustomized(false)
{}

void ItemData::setStyle(const QString &style) {
	mStyle = style;
}

const QString &ItemData::style() const {
	return mStyle;
}

void ItemData::setColor(const QString &color) {
	if (!color.isEmpty()) {
		mColor.setNamedColor(color);
		mIsCustomized = true;
	}
}

const QColor &ItemData::color() const {
	return mColor;
}

void ItemData::setSelectionColor(const QString &color) {
	if (!color.isEmpty()) {
		mSelectionColor.setNamedColor(color);
		mIsCustomized = true;
	}
}

const QColor &ItemData::selectionColor() const {
	return mSelectionColor;
}

void ItemData::setItalic(const QString &italic) {
	if (!italic.isEmpty()) {
		mItalic = toBool(italic);
		mItalicSpecified = true;
		mIsCustomized = true;
	}
}

bool ItemData::isItalic() const {
	return mItalic;
}

bool ItemData::isItalicSpecified() const {
	return mItalicSpecified;
}

void ItemData::setBold(const QString &bold) {
	if (!bold.isEmpty()) {
		mBold = toBool(bold);
		mBoldSpecified = true;
		mIsCustomized = true;
	}
}

bool ItemData::isBold() const {
	return mBold;
}

bool ItemData::isBoldSpecified() const {
	return mBoldSpecified;
}

void ItemData::setUnderlined(const QString &underlined) {
	if (!underlined.isEmpty()) {
		mUnderlined = toBool(underlined);
		mUnderlinedSpecified = true;
		mIsCustomized = true;
	}
}

bool ItemData::isUnderlined() const {
	return mUnderlined;
}

bool ItemData::isUnderlinedSpecified() const {
	return mUnderlinedSpecified;
}

void ItemData::setStrikeOut(const QString &strike) {
	if(!strike.isEmpty()) {
		mStrikedOut = toBool(strike);
		mStrikeOutSpecified = true;
		mIsCustomized = true;
	}
}

bool ItemData::isStrikeOut() const {
	return mStrikedOut;
}

bool ItemData::isStrikeOutSpecified() const {
	return mStrikeOutSpecified;
}

bool ItemData::isCustomized() const {
	return mIsCustomized;
}
