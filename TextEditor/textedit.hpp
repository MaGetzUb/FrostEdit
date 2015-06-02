#ifndef TEXTEDIT_HPP
#define TEXTEDIT_HPP

#include <QObject>
#include <QPlainTextEdit>
#include <QDockWidget>
#include "document.hpp"
#include <QRegularExpression>
#include <QString>
#include <QCompleter>
#include <QStandardItemModel>
#include <QFocusEvent>
#include <QAction>

#include <QMenu>
#include <QContextMenuEvent>
#include <QWheelEvent>

#include "qate/qateblockdata.h"
#include "qate/tabsettings.h"
class TextBlockSelection {
	public:

		bool isValid() const{ return !mFirstBlock.isNull() && !mLastBlock.isNull(); }
		void clear() { mFirstBlock = mLastBlock = QTextCursor(); }

		QTextCursor mFirstBlock; // defines the first block
		QTextCursor mLastBlock; // defines the last block
		int mFirstVisualColumn; // defines the first visual column of the selection
		int mLastVisualColumn; // defines the last visual column of the selection

		enum Anchor {TopLeft = 0,
			     TopRight,
			     BottomLeft,
			     BottomRight};
		Anchor mAnchor;
		TextBlockSelection():mFirstVisualColumn(0), mLastVisualColumn(0), mAnchor(BottomRight){}
		void moveAnchor(int blockNumber, int visualColumn);
		inline int anchorColumnNumber() const { return (mAnchor % 2) ? mLastVisualColumn : mFirstVisualColumn; }
		inline int anchorBlockNumber() const {
			return (mAnchor <= TopRight ? mFirstBlock.blockNumber() : mLastBlock.blockNumber()); }
		QTextCursor selection(const TextEditor::TabSettings &ts) const;
		void fromSelection(const TextEditor::TabSettings &ts, const QTextCursor &selection);
};




class LineNumberArea;
class TextBlockUserData;
class TextEdit : public QPlainTextEdit {

		Q_OBJECT
		QWidget* mLineNumberWidget;
		QWidget* mDocumentWatcher;

		QRect mViewportRect;
		QWidget* mParentWidget;

		static int gVirtualTabSize;
		QCompleter* mCompleter;
		QStandardItemModel* mCompleterModel;

		void customTextModification(QKeyEvent*);
		QTextBlock blockAt(const QPoint& pnt);

		QList<QTextBlock> mRegionUnderCursor;
		QList<QTextCursor> mExtraCursors;
		bool mClicked;
		int mCurrentBlockCount;

		QTextCharFormat mLineNumberFormat;
		QTextCharFormat mSelectedLineNumberFormat;
		QTextCharFormat mSimilarOccurance;
		QTextCharFormat mCurrentLine;
		QTextCharFormat mRegionVisualizerFormat;
		QTextCharFormat mRegionVisualizerSelectedFormat;

		TextBlockSelection mBlockSelection;
		bool mIsBlockSelection;
	public:
		explicit TextEdit(QWidget* parent = 0);
		explicit TextEdit(QWidget* parent, Document* doc);
		~TextEdit();


		void initCompleter();
		void setTabSettings(const TextEditor::TabSettings& settings);

		void lineNumberAreaPaintEvent(QPaintEvent*);
		void documentWatchPaintEvent(QPaintEvent*);

		const QString textUnderCursor();

		QWidget* getParentWidget();
		QRect calculateSize() const;

		void setLineNumberFormat(const QTextCharFormat& fmt);
		void setSimilarOccuranceFormat(const QTextCharFormat& fmt);
		void setSelectedLine(const QTextCharFormat& fmt);
		void setRegionVisualizerFormat(const QTextCharFormat& fmt);
		void setRegionVisualizerSelectedFormat(const QTextCharFormat& fmt);

		void setFont(const QFont &);

		void documentWatchMousePressEvent(QMouseEvent*);
		void documentWatchMouseReleaseEvent(QMouseEvent*);
		void documentWatchMouseMoveEvent(QMouseEvent*);

		void lineNumberAreaMousePressEvent(QMouseEvent*);
		void lineNumberAreaMouseReleaseEvent(QMouseEvent*);
		void lineNumberAreaMouseMoveEvent(QMouseEvent*);
		void lineNumberAreaWheelEvent(QWheelEvent*);
		void lineNumberAreaContextMenuEvent(QContextMenuEvent* e);

		int lineNumberAreaWidth();
		int getDigitCount();
		int documentWatcherWidth();

		static TextBlockUserData* blockData(const QTextBlock&);

		QList<QTextBlock> getRegionSubBlocks(const QTextBlock&);
		QTextBlock getBlockRegionStart(const QTextBlock&);
		QTextBlock getBlockRegionEnd(const QTextBlock&);


		void setCursorPosition(int row, int column);

		void find(const QString& str);
		void setBlocksVisible(QList<QTextBlock>& blocks, bool visible);
		void updateHeight();
	private:
		void drawIndentationPipes(QPainter&, QTextBlock&, int top, int bottom, int space, const QTextCharFormat&);
		void drawBookMark(QPainter&, QTextBlock&, int top, int bottom, int space);
		void drawModification(QPainter&, QTextBlock&, int top, int bottom, int space);

		bool startsRegion(const QTextBlock&, int&);
		bool endsRegion(const QTextBlock&, int&, int&);
		bool endsRegion(const QTextBlock&, int&, int&, bool&);
		bool continuesRegion(QTextBlock&, int&);
		Document* toDocument(QTextDocument* doc);
	protected:
		void resizeEvent(QResizeEvent*);
		void contextMenuEvent(QContextMenuEvent *e);
		void keyPressEvent(QKeyEvent *e);
		void focusInEvent(QFocusEvent* e);
		void mouseDoubleClickEvent(QMouseEvent* e);
		void mousePressEvent(QMouseEvent* e);
		void mouseReleaseEvent(QMouseEvent *e);
		void wheelEvent(QWheelEvent* e);
		void paintEvent(QPaintEvent *e);
	signals:

	public slots:
		void autoIndent();
	private slots:
		void insertCompletion(const QString&);
		void updateDocumentLength(int newBlockCount);
		void updateLineNumberArea(const QRect&, int);
		void highlightCurrentLine();
		void handleBlockSelection(int rowdiff, int coldiff);
};



class DocumentMap : public QWidget {

		Q_OBJECT

		TextEdit* mTextEdit;
	public:
		DocumentMap(TextEdit *editor):QWidget(editor), mTextEdit(editor){}
		QSize sizeHint() const {
			return QSize(mTextEdit->documentWatcherWidth(), 0);
		}


	protected:

		void paintEvent(QPaintEvent *e) {
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


class LineNumberArea : public QWidget {
	friend class TextEdit;
		Q_OBJECT

		TextEdit *mTextEdit;
		QAction* mBookMarkAction;
		QAction* mBreakPointAction;


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


#endif // TEXTEDIT_HPP
