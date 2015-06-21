#ifndef LINENUMBERAREA
#define LINENUMBERAREA

#include "textedit.hpp"

class LineNumberArea : public QWidget {
	friend class TextEdit;
		Q_OBJECT

		TextEdit *mTextEdit;
		QAction* mBookMarkAction;
		QAction* mBreakPointAction;
		bool mIsSelection;
	public:
		LineNumberArea(TextEdit *editor):
			QWidget(editor),
			mTextEdit(editor),
			mBookMarkAction(new QAction("Toggle bookmark", this)),
			mBreakPointAction(new QAction("Toggle breakpoint", this)){
			mBookMarkAction->setCheckable(true);
			mBreakPointAction->setCheckable(true);
			setMouseTracking(true);
		}
		QSize sizeHint() const {
			return QSize(mTextEdit->lineNumberAreaWidth(), 0);
		}
	protected:
		void paintEvent(QPaintEvent *e) {
			mTextEdit->lineNumberAreaPaintEvent(e);
		}

		void mousePressEvent(QMouseEvent *e) {
			mTextEdit->lineNumberAreaMousePressEvent(e);
		}

		void mouseReleaseEvent(QMouseEvent *e) {
			mTextEdit->lineNumberAreaMouseReleaseEvent(e);
		}

		void mouseMoveEvent(QMouseEvent* e) {
			mTextEdit->lineNumberAreaMouseMoveEvent(e);
		}

		void wheelEvent(QWheelEvent *e) {
			mTextEdit->lineNumberAreaWheelEvent(e);
		}

		void contextMenuEvent(QContextMenuEvent* e) {
			mTextEdit->lineNumberAreaContextMenuEvent(e);
		}

};

#endif // LINENUMBERAREA

