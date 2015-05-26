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
	bool m_foldingStartIncluded;
	bool m_foldingEndIncluded;
	int m_foldingIdent;
	int m_regionId;

	public:
	TextBlockUserData(): Qate::BlockData(), m_foldingStartIncluded(false),m_foldingEndIncluded(false), m_foldingIdent(0), m_regionId(0) {}
	virtual ~TextBlockUserData(){}

	void setFoldingStartIncluded(const bool b) {m_foldingStartIncluded = b;}
	void setFoldingEndIncluded(const bool b) {m_foldingEndIncluded = b;}
	void setFoldingIndent(const int i) {m_foldingIdent = i;}
	void setRegionId(int id) {m_regionId = id;}

	bool isFoldingStartIncluded() {return m_foldingStartIncluded;}
	bool isFoldingEndIncluded() {return m_foldingEndIncluded;}
	int getFoldingIndent() {return m_foldingIdent;}
	int getRegionId() {return m_regionId;}
};

#endif // BASETEXTDOCUMENTLAYOUT_H
