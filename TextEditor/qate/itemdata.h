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

#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QString>
#include <QColor>

namespace TextEditor {
	namespace Internal {

		class ItemData
		{
			public:
				ItemData();

				void setStyle(const QString &style);
				const QString &style() const;

				void setColor(const QString &color);
				const QColor &color() const;

				void setSelectionColor(const QString &color);
				const QColor &selectionColor() const;

				void setItalic(const QString &italic);
				bool isItalic() const;
				bool isItalicSpecified() const;

				void setBold(const QString &bold);
				bool isBold() const;
				bool isBoldSpecified() const;

				void setUnderlined(const QString &underlined);
				bool isUnderlined() const;
				bool isUnderlinedSpecified() const;

				void setStrikeOut(const QString &strike);
				bool isStrikeOut() const;
				bool isStrikeOutSpecified() const;

				bool isCustomized() const;

			private:
				bool mItalic;
				bool mItalicSpecified;
				bool mBold;
				bool mBoldSpecified;
				bool mUnderlined;
				bool mUnderlinedSpecified;
				bool mStrikedOut;
				bool mStrikeOutSpecified;
				bool mIsCustomized;
				QString mStyle;
				QColor mColor;
				QColor mSelectionColor;
		};

	} // namespace Internal
} // namespace TextEditor

#endif // ITEMDATA_H
