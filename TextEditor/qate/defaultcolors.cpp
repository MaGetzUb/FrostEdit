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

#include "defaultcolors.h"

#include <Qt>
#include "highlighter.h"

using namespace Qate;

DefaultColors::DefaultColors() {
	mKeywordFormat.setForeground(Qt::darkBlue);

	mDataTypeFormat.setForeground(Qt::blue);
	
	mDecimalFormat.setForeground(Qt::darkYellow);

	mBaseNFormat.setForeground(Qt::darkRed);
	
	mFloatFormat.setForeground(Qt::darkRed);
	
	mCharFormat.setForeground(Qt::red);
	
	mStringFormat.setForeground(Qt::red);

	mCommentFormat.setForeground(QColor(0x00,0xA0,0x70));
	mCommentFormat.setFontItalic(true);

	mAlertFormat.setForeground(Qt::red);
	mAlertFormat.setFontUnderline(true);
	mAlertFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
	
	mErrorFormat.setForeground(Qt::red);
	mErrorFormat.setFontUnderline(true);
	mErrorFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
	
	mFunctionFormat.setForeground(QColor(0x0, 0x60, 0xA0));

	mRegionMarkerFormat.setForeground(Qt::green);
	
	mOthersFormat.setForeground(Qt::darkMagenta);
}

DefaultColors &DefaultColors::instance() {
	static DefaultColors DefaultColors;
	return DefaultColors;
}

QString DefaultColors::name(const QTextCharFormat &format) const {
	if (format == QTextCharFormat())
		return "Default format";
	else if (format == mKeywordFormat)
		return "Keyword";
	else if (format == mDataTypeFormat)
		return "Data type format";
	else if (format == mDecimalFormat)
		return "Decimal format";
	else if (format == mBaseNFormat)
		return "Base N format";
	else if (format == mFloatFormat)
		return "Float format";
	else if (format == mCharFormat)
		return "Char format";
	else if (format == mStringFormat)
		return "String format";
	else if (format == mCommentFormat)
		return "Comment format";
	else if (format == mAlertFormat)
		return "Alert format";
	else if (format == mErrorFormat)
		return "Error format";
	else if (format == mFunctionFormat)
		return "Function format";
	else if (format == mRegionMarkerFormat)
		return "Region Marker format";
	else if (format == mOthersFormat)
		return "Others format";
	else
		return "Unidentified format";
}

using namespace TextEditor::Internal;
void DefaultColors::ApplyToHighlighter(TextEditor::Internal::Highlighter *hl) {
	//	hl->configureFormat(Highlighter::Normal,           instance().m_othersFormat       );
	//	hl->configureFormat(Highlighter::VisualWhitespace, instance().m_othersFormat       );
	hl->configureFormat(Highlighter::Keyword,          instance().mKeywordFormat      );
	hl->configureFormat(Highlighter::DataType,         instance().mDataTypeFormat     );
	hl->configureFormat(Highlighter::Decimal,          instance().mDecimalFormat      );
	hl->configureFormat(Highlighter::BaseN,            instance().mBaseNFormat        );
	hl->configureFormat(Highlighter::Float,            instance().mFloatFormat        );
	hl->configureFormat(Highlighter::Char,             instance().mCharFormat         );
	hl->configureFormat(Highlighter::String,           instance().mStringFormat       );
	hl->configureFormat(Highlighter::Comment,          instance().mCommentFormat      );
	hl->configureFormat(Highlighter::Alert,            instance().mAlertFormat        );
	hl->configureFormat(Highlighter::Error,            instance().mErrorFormat        );
	hl->configureFormat(Highlighter::Function,         instance().mFunctionFormat     );
	hl->configureFormat(Highlighter::RegionMarker,     instance().mRegionMarkerFormat );
	hl->configureFormat(Highlighter::Others,           instance().mOthersFormat       );
}

