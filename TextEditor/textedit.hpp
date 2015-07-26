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
#include <QTextBrowser>

#include "colorscheme.hpp"
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

class DocumentMap;
class FrostEdit;
class FindReplaceDialog;
class Colorscheme;
class LineNumberArea;
class TextBlockUserData;
class TextEdit : public QPlainTextEdit {
		Q_OBJECT
		LineNumberArea* mLineNumberWidget;
		DocumentMap* mDocumentWatcher;

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

		FrostEdit* mFrostEditInstance;

		QTextCharFormat mLineNumberFormat;
		QTextCharFormat mSelectedLineNumberFormat;
		QTextCharFormat mSimilarOccurance;
		QTextCharFormat mCurrentLine;
		QTextCharFormat mRegionVisualizerFormat;
		QTextCharFormat mRegionVisualizerSelectedFormat;
		QTextCharFormat mSelectedParenthesis;
		QTextCharFormat mErrorUnderlineFormat;

		FindReplaceDialog* mFindReplaceDialog;
		TextBlockSelection mBlockSelection;
		bool mIsBlockSelection;

		Colorscheme* mSyntaxStyle;

		QFont mFont;

		QTextCursor mFoundPosition;

	public:
		explicit TextEdit(QWidget* parent = 0);
		explicit TextEdit(QWidget* parent, Document* doc);
		~TextEdit();
		void setFindReplaceInstance(FindReplaceDialog* instance);


		void setTabSettings(const TextEditor::TabSettings& settings);

		void lineNumberAreaPaintEvent(QPaintEvent*);
		void documentWatchPaintEvent(QPaintEvent*);
		LineNumberArea* lineNumberArea();

		const QString textUnderCursor();

		QWidget* getParentWidget();
		QRect calculateSize() const;

		void setLineNumberFormat(const QTextCharFormat& fmt);
		void setSimilarOccuranceFormat(const QTextCharFormat& fmt);
		void setSelectedLine(const QTextCharFormat& fmt);
		void setRegionVisualizerFormat(const QTextCharFormat& fmt);
		void setRegionVisualizerSelectedFormat(const QTextCharFormat& fmt);
		void setSelectedParenthesesFormat(const QTextCharFormat& fmt);
		void setErrorUnderlineFormat(const QTextCharFormat& fmt);

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

		void setBlocksVisible(QList<QTextBlock>& blocks, bool visible);
		void updateHeight();
	private:

		void initCompleter();
		QPair<int, int> searchCounterBracket(int pos);

		void drawIndentationPipes(QPainter&, QTextBlock&, int top, int bottom, int space, const QTextCharFormat&);
		void drawBookMark(QPainter&, QTextBlock&, int top, int bottom, int space);
		void drawModification(QPainter&, QTextBlock&, int top, int bottom, int space);
		void autoIndentCursor(QTextCursor&);
		void updateFont();


		bool startsRegion(const QTextBlock&, int&);
		inline bool startsRegion(const QTextBlock& block) {int tmp; return startsRegion(block, tmp);}
		bool endsRegion(const QTextBlock&, int&, int&);
		inline bool endsRegion(const QTextBlock& block) {int a, b; return endsRegion(block, a, b);}
		bool endsRegion(const QTextBlock&, int&, int&, bool&);
		inline bool endsRegion(const QTextBlock& block, bool& perfect) {int a, b; return endsRegion(block, a, b, perfect);}
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
		void find();
		void findReplace();
		void replaceAll();
		void showOccurances(const QString& str);

	private slots:

		void insertCompletion(const QString&);
		void updateDocumentLength(int newBlockCount);
		void updateLineNumberArea(const QRect&, int);
		void highlightCurrentLine();
		void handleBlockSelection(int rowdiff, int coldiff);

};


#endif // TEXTEDIT_HPP
