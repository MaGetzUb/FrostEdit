#ifndef DOCUMENTMAP_HPP
#define DOCUMENTMAP_HPP

#include "textedit.hpp"

class DocumentMap : public QPlainTextEdit {
		friend class TextEdit;
		Q_OBJECT

		TextEdit* mTextEdit;
	public:
		DocumentMap(TextEdit *editor):
			QPlainTextEdit(qobject_cast<QWidget*>(editor)),
			mTextEdit(editor){
			QAbstractScrollArea* area = qobject_cast<QAbstractScrollArea*>(this);
			setLineWrapMode(QPlainTextEdit::NoWrap);
			setReadOnly(true);
			setFont(editor->font());
			setDocument(editor->document());
		}

		void setFont(QFont fnt) {
			QFont font = fnt;
			font.setPointSize(4);
			qobject_cast<QPlainTextEdit*>(this)->setFont(fnt);
		}

		QSize sizeHint() const {
			return QSize(mTextEdit->documentWatcherWidth(), 0);
		}


	protected:

		void paintEvent(QPaintEvent *e) {
			QPlainTextEdit::paintEvent(e);
			mTextEdit->documentWatchPaintEvent(e);
		}

		void mousePressEvent(QMouseEvent *e) {
			mTextEdit->documentWatchMousePressEvent(e);
		}

		void mouseReleaseEvent(QMouseEvent *e) {
			mTextEdit->documentWatchMouseReleaseEvent(e);
		}

		void mouseMoveEvent(QMouseEvent *e) {
			mTextEdit->documentWatchMouseMoveEvent(e);
		}
};


#endif // DOCUMENTMAP_HPP
