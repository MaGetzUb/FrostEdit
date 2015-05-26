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

#ifndef QATE_DEFAULT_COLORS_H
#define QATE_DEFAULT_COLORS_H

#include <QTextCharFormat>

namespace TextEditor{
	namespace Internal {
		class Highlighter;
	}
}


namespace Qate {

	class DefaultColors
	{
		public:
			static DefaultColors &instance();

			const QTextCharFormat &keywordFormat() const { return mKeywordFormat; }
			const QTextCharFormat &dataTypeFormat() const { return mDataTypeFormat; }
			const QTextCharFormat &decimalFormat() const { return mDecimalFormat; }
			const QTextCharFormat &baseNFormat() const { return mBaseNFormat; }
			const QTextCharFormat &floatFormat() const { return mFloatFormat; }
			const QTextCharFormat &charFormat() const { return mCharFormat; }
			const QTextCharFormat &stringFormat() const { return mStringFormat; }
			const QTextCharFormat &commentFormat() const { return mCommentFormat; }
			const QTextCharFormat &alertFormat() const { return mAlertFormat; }
			const QTextCharFormat &errorFormat() const { return mErrorFormat; }
			const QTextCharFormat &functionFormat() const { return mFunctionFormat; }
			const QTextCharFormat &regionMarketFormat() const { return mRegionMarkerFormat; }
			const QTextCharFormat &othersFormat() const { return mOthersFormat; }

			QString name(const QTextCharFormat &format) const;
			static void ApplyToHighlighter(TextEditor::Internal::Highlighter *hl);

		private:
			DefaultColors();
			Q_DISABLE_COPY(DefaultColors);

			QTextCharFormat mKeywordFormat;
			QTextCharFormat mDataTypeFormat;
			QTextCharFormat mDecimalFormat;
			QTextCharFormat mBaseNFormat;
			QTextCharFormat mFloatFormat;
			QTextCharFormat mCharFormat;
			QTextCharFormat mStringFormat;
			QTextCharFormat mCommentFormat;
			QTextCharFormat mAlertFormat;
			QTextCharFormat mErrorFormat;
			QTextCharFormat mFunctionFormat;
			QTextCharFormat mRegionMarkerFormat;
			QTextCharFormat mOthersFormat;
	};

}

#endif // QATE_DEFAULT_COLORS_H
