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

#ifndef BASETEXTDOCUMENTLAYOUT_H
#define BASETEXTDOCUMENTLAYOUT_H

#include <QTextBlockUserData>
#include "qateblockdata.h"

// Replaces the "real" basetextdocumentlayout.h file.

class TextBlockUserData : public Qate::BlockData
{
	bool mFoldingStartIncluded;
	bool mFoldingEndIncluded;
	int mFoldingIdent;
	int mRegionId;


	bool mRegionStarts;
	bool mRegionEnds;
	public:
	TextBlockUserData(): Qate::BlockData(), mFoldingStartIncluded(false),mFoldingEndIncluded(false), mFoldingIdent(0), mRegionId(0) {}
	virtual ~TextBlockUserData(){}

	void setFoldingStartIncluded(const bool b) {mFoldingStartIncluded = b;}
	void setFoldingEndIncluded(const bool b) {mFoldingEndIncluded = b;}
	void setFoldingIndent(const int i) {mFoldingIdent = i;}
	void setRegionId(int id) {mRegionId = id;}

	void setRegionStart(bool b) {mRegionStarts = b;}
	void setRegionEnd(bool b) {mRegionEnds = b;}
	bool isRegionStart() {return mRegionStarts;}
	bool isRegionEnd() {return mRegionEnds;}

	bool isFoldingStartIncluded() {return mFoldingStartIncluded;}
	bool isFoldingEndIncluded() {return mFoldingEndIncluded;}
	int foldingIndent() {return mFoldingIdent;}
	int getRegionId() {return mRegionId;}
};

#endif // BASETEXTDOCUMENTLAYOUT_H
