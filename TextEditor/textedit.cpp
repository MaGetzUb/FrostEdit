#include "textedit.hpp"
#include <QPainter>
#include <QGradient>
#include <QTextBlock>
#include <QTextCursor>
#include <QMenu>
#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QWidgetList>
#include <QAbstractTextDocumentLayout>
#include <QStringListModel>
#include "qate/highlighter.h"
#include "qate/highlightdefinition.h"
#include "qate/keywordlist.h"

int TextEdit::gVirtualTabSize = 5;
using namespace TextEditor::Internal;


void TextBlockSelection::moveAnchor(int blockNumber, int visualColumn) {
	if (visualColumn >= 0) {
		if (mAnchor % 2) {
			mLastVisualColumn = visualColumn;
			if (mLastVisualColumn < mFirstVisualColumn) {
				qSwap(mFirstVisualColumn, mLastVisualColumn);
				mAnchor = (Anchor) (mAnchor - 1);
			}
		} else {
			mFirstVisualColumn = visualColumn;
			if (mFirstVisualColumn > mLastVisualColumn) {
				qSwap(mFirstVisualColumn, mLastVisualColumn);
				mAnchor = (Anchor) (mAnchor + 1);
			}
		}
	}

	if (blockNumber >= 0 && blockNumber < mFirstBlock.document()->blockCount()) {
		if (mAnchor <= TopRight) {
			mFirstBlock.setPosition(mFirstBlock.document()->findBlockByNumber(blockNumber).position());
			if (mFirstBlock.blockNumber() > mLastBlock.blockNumber()) {
				qSwap(mFirstBlock, mLastBlock);
				mAnchor = (Anchor) (mAnchor + 2);
			}
		} else {
			mLastBlock.setPosition(mFirstBlock.document()->findBlockByNumber(blockNumber).position());
			if (mLastBlock.blockNumber() < mFirstBlock.blockNumber()) {
				qSwap(mFirstBlock, mLastBlock);
				mAnchor = (Anchor) (mAnchor - 2);
			}
		}
	}
	mFirstBlock.movePosition(QTextCursor::StartOfBlock);
	mLastBlock.movePosition(QTextCursor::EndOfBlock);
}

QTextCursor TextBlockSelection::selection(const TextEditor::TabSettings& ts) const {
	QTextCursor cursor = mFirstBlock;
	if (mAnchor <= TopRight) {
		cursor.setPosition(mLastBlock.block().position() + ts.positionAtColumn(mLastBlock.block().text(), mLastVisualColumn));
		cursor.setPosition(mFirstBlock.block().position() + ts.positionAtColumn(mFirstBlock.block().text(), mFirstVisualColumn),
				   QTextCursor::KeepAnchor);
	} else {
		cursor.setPosition(mFirstBlock.block().position() + ts.positionAtColumn(mFirstBlock.block().text(), mFirstVisualColumn));
		cursor.setPosition(mLastBlock.block().position() + ts.positionAtColumn(mLastBlock.block().text(), mLastVisualColumn),
				   QTextCursor::KeepAnchor);
	}
	return cursor;
}

void TextBlockSelection::fromSelection(const TextEditor::TabSettings& ts, const QTextCursor& selection) {
	mFirstBlock = selection;
	mFirstBlock.setPosition(selection.selectionStart());
	mFirstVisualColumn = ts.columnAt(mFirstBlock.block().text(), mFirstBlock.positionInBlock());
	mLastBlock = selection;
	mLastBlock.setPosition(selection.selectionEnd());
	mLastVisualColumn = ts.columnAt(mLastBlock.block().text(), mLastBlock.positionInBlock());
	if (selection.anchor() > selection.position())
		mAnchor = TopLeft;
	else
		mAnchor = BottomRight;

	mFirstBlock.movePosition(QTextCursor::StartOfBlock);
	mLastBlock.movePosition(QTextCursor::EndOfBlock);
}


void TextEdit::customTextModification(QKeyEvent* e) {
	switch(e->key()) {
		case Qt::Key_Backtab: {
			QTextCursor cur = textCursor();
			int pos = cur.position();
			int anchor = cur.anchor();
			cur.setPosition(pos);
			cur.setPosition(pos-1, QTextCursor::KeepAnchor);

			if(cur.selectedText() == "\t") {
				cur.removeSelectedText();
				cur.setPosition(anchor-1);
				cur.setPosition(pos-1, QTextCursor::KeepAnchor);
			} else {
				cur.setPosition(anchor);
				cur.setPosition(anchor-1, QTextCursor::KeepAnchor);
				if(cur.selectedText() == "\t") {
					cur.removeSelectedText();
					cur.setPosition(anchor-1);
					cur.setPosition(pos-1, QTextCursor::KeepAnchor);
				} else {
					cur.setPosition(anchor);
					cur.setPosition(pos, QTextCursor::KeepAnchor);
				}
			}
			e->accept();
		} break;
		case Qt::Key_Right:
		case Qt::Key_Left:
			if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
				int diff_row = 0;
				int diff_col = 0;
				if (e->key() == Qt::Key_Up)
					diff_row = -1;
				else if (e->key() == Qt::Key_Down)
					diff_row = 1;
				else if (e->key() == Qt::Key_Left)
					diff_col = -1;
				else if (e->key() == Qt::Key_Right)
					diff_col = 1;
				handleBlockSelection(diff_row, diff_col);
				e->accept();
				return;
			} else {
				// leave block selection mode
				if (mIsBlockSelection) {
					mIsBlockSelection = false;
					mBlockSelection.clear();
					viewport()->update();
				}
				QPlainTextEdit::keyPressEvent(e);
			}
		break;
		case Qt::Key_Backspace:{
			QTextCursor cur = textCursor();
			QTextBlock block = cur.block();
			int tmp1;
			if(startsRegion(block, tmp1)) {
				QList<QTextBlock> blocks = getRegionSubBlocks(block.next());
				setBlocksVisible(blocks, true);
			}
			QTextCursor decim = cur;
			QPlainTextEdit::keyPressEvent(e);

		}break;
		case Qt::Key_Return:
		case Qt::Key_Enter: {
			QTextCursor cur = textCursor();
			QTextBlock block = cur.block();
			int tmp1, tmp2;
			if(startsRegion(block, tmp1)) {
				QList<QTextBlock> blocks = getRegionSubBlocks(block.next());
				setBlocksVisible(blocks, true);
			}
			QTextCursor decim = cur;
			QPlainTextEdit::keyPressEvent(e);


			bool perfect;
			if(endsRegion(cur.block().previous(), tmp1, tmp2, perfect) && perfect) {
				decim.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
				decim.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
				decim.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
				decim.removeSelectedText();
				TextBlockUserData* data = blockData(decim.block().next());
				decim.insertText(QString(data->getFoldingIndent(), QChar::Tabulation));
			}


			//Let's add indentation to new line
			TextBlockUserData* data = blockData(cur.block());
			cur.insertText(QString(data->getFoldingIndent(), QChar::Tabulation));
			setTextCursor(cur);
			e->accept();
			return;
		} break;
		case  Qt::Key_D: {
			if(e->modifiers() & Qt::ControlModifier) {
				QTextCursor linecur = textCursor();
				linecur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
				linecur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
				QTextCursor cur = textCursor();
				cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
				cur.insertText(tr("\n")+linecur.selectedText());
				e->accept();
				return;
			} else {
				QPlainTextEdit::keyPressEvent(e);
			}
		} break;
		default:
			QPlainTextEdit::keyPressEvent(e);
	}
}

QTextBlock TextEdit::blockAt(const QPoint& pnt) {
	QTextBlock block = firstVisibleBlock();
	int top = 0;
	int maxHeight = visibleRegion().boundingRect().height();

	while(block.isValid() && top < maxHeight) {
		int bheight = blockBoundingRect(block).height();
		if(pnt.y() > top && pnt.y() < top+bheight) {
			return block;
		}
		if(block.isVisible())
			top += bheight;
		block = block.next();
	}

	return QTextBlock();
}

void TextEdit::insertCompletion(const QString& completion) {
	blockSignals(true);
	if(mCompleter->widget() != this)
		return;
	QTextCursor tc = textCursor();
	int extra = completion.length() - mCompleter->completionPrefix().length();


	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
	blockSignals(false);
}

TextEdit::TextEdit(QWidget *parent):
	QPlainTextEdit(parent),
	mParentWidget(parent),
	mClicked(false),
	mIsBlockSelection(false),
	mFont(font())
{
	setMouseTracking(true);
	setLineWrapMode(QPlainTextEdit::NoWrap);

	mLineNumberWidget = new LineNumberArea(this);
	//mDocumentWatcher = new DocumentMap(this);
	//mDocumentWatcher->setVisible(true);
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateDocumentLength(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));


	mCompleter = new QCompleter(this);

	mCompleterModel = new QStandardItemModel(mCompleter);
	mCompleter->setModel(mCompleterModel);
	mCompleter->setWidget(this);
	mCompleter->setCompletionMode(QCompleter::PopupCompletion);
	mCompleter->setCaseSensitivity(Qt::CaseInsensitive);

	mSimilarOccurance.setBackground(QColor(0, 255, 200));
	mSimilarOccurance.setForeground(QColor(255, 255, 255));
	mLineNumberFormat.setBackground(QColor(Qt::lightGray));
	mLineNumberFormat.setForeground(QColor(Qt::lightGray).dark(180));

	mRegionVisualizerFormat.setForeground(QColor(Qt::lightGray).dark(100));
	mRegionVisualizerFormat.setBackground(QColor(Qt::white));
	mRegionVisualizerSelectedFormat.setForeground(QColor(Qt::red));
	mRegionVisualizerSelectedFormat.setBackground(QColor(Qt::white));
}

TextEdit::TextEdit(QWidget* parent, Document* doc):
	TextEdit(parent)
{
	setDocument(qobject_cast<QTextDocument*>(doc));

	mLineNumberWidget = new LineNumberArea(this);
	mDocumentWatcher = new DocumentMap(this);
	mDocumentWatcher->setVisible(false);

	initCompleter();
}


void TextEdit::lineNumberAreaMousePressEvent(QMouseEvent* e) {

}

void TextEdit::lineNumberAreaMouseReleaseEvent(QMouseEvent* e) {
	int space = fontMetrics().width(QLatin1Char('9'));
	if(e->button() == Qt::LeftButton) {
		if(e->x() < (lineNumberAreaWidth() - space*(getDigitCount()-2))) {
			QTextBlock block = blockAt(e->pos());
			TextBlockUserData* data = blockData(block);
			if(data != nullptr) {
				data->setBookmark(!data->isBookmark());

				repaint();
			}

		}
		else if(e->x() > lineNumberAreaWidth() - space*2 && e->x() < lineNumberAreaWidth()) {
			QTextBlock block = blockAt(e->pos());
			Document* doc = toDocument(document());
			int id;
			if(startsRegion(block, id) && id != 0) {
				QList<QTextBlock> list = getRegionSubBlocks(block);
				list = list.mid(1, list.length()-2);
				setBlocksVisible(list, !block.next().isVisible());
				repaint();
			}

		}
	}
}

void TextEdit::setCursorPosition(int row, int column) {
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, column);
	setTextCursor(cursor);
}

void TextEdit::find(const QString& str) {

}

void TextEdit::setBlocksVisible(QList<QTextBlock>& blocks, bool visible) {
	Document* doc = toDocument(document());
	if(blocks.isEmpty())
		return;


	int blockCount = document()->blockCount();
	QPlainTextDocumentLayout* layout = qobject_cast<QPlainTextDocumentLayout*>(doc->documentLayout());

	for(QTextBlock& b: blocks) {
		b.setVisible(visible);
	}
	document()->markContentsDirty(blocks.first().position(), blocks.last().position() - blocks.first().position());

	viewport()->update();
}



void TextEdit::drawIndentationPipes(QPainter& painter, QTextBlock& block, int top, int bottom, int space, const QTextCharFormat& fmt) {
	QTextBlock nextblock = block.next();
	int blockheight = blockBoundingGeometry(block).height();


	int size = qMin(int(float(space)*1.5f), int((float)blockheight*0.75));
	int spaceStep = (int)(float(space)/5.0f);
	int regionid;
	int nextregion;
	painter.setPen(fmt.foreground().color());
	if(startsRegion(block, regionid)) {
		QRect rect(lineNumberAreaWidth() - space - size/2, top+blockheight/2 - size/2, size, size);
		painter.fillRect(rect, fmt.background().color());
		painter.drawRect(rect);
		if(nextblock.isValid()) {
			if(nextblock.isVisible()) {
				painter.drawLine(rect.left()+1 + spaceStep, rect.top()+rect.height()/2, rect.right() - spaceStep, rect.top()+rect.height()/2);
			} else {
				painter.drawLine(rect.left()+1 + spaceStep, rect.top()+rect.height()/2, rect.right() - spaceStep, rect.top()+rect.height()/2);
				painter.drawLine(rect.left()+rect.width()/2, rect.top()+1 + spaceStep, rect.left()+rect.width()/2, rect.bottom() - space/5);
			}
		}
	} else if(endsRegion(block, regionid, nextregion) && regionid != 0 && nextregion != 0) {
		painter.setPen(fmt.foreground().color());
		painter.drawLine(lineNumberAreaWidth() - space, top+blockheight/2, lineNumberAreaWidth(), top+blockheight/2);
		painter.drawLine(lineNumberAreaWidth() - space, top, lineNumberAreaWidth() - space, bottom);
	} else if(endsRegion(block, regionid, nextregion) && regionid != 0 && nextregion == 0 ) {
		painter.drawLine(lineNumberAreaWidth() - space, top+blockheight/2, lineNumberAreaWidth(), top+blockheight/2);
		painter.drawLine(lineNumberAreaWidth() - space, top, lineNumberAreaWidth() - space, top+blockheight/2);
	} else if(continuesRegion(block, regionid) && regionid != 0) {
		painter.drawLine(lineNumberAreaWidth() - space, top, lineNumberAreaWidth() - space, bottom);
	}
}






void TextEdit::drawBookMark(QPainter& painter, QTextBlock& block, int top, int bottom, int space) {
	int blockheight = blockBoundingGeometry(block).height();
	TextBlockUserData* data = blockData(block);
	if(data->isBookmark()) {
		painter.fillRect(lineNumberAreaWidth() - space*(getDigitCount()), top, space*2-4, blockheight, QColor(Qt::blue));
	}
}

void TextEdit::drawModification(QPainter& painter, QTextBlock& block, int top, int bottom, int space) {
	int blockheight = blockBoundingGeometry(block).height();
	TextBlockUserData* data = blockData(block);
	if(data->isModified())
		painter.fillRect(lineNumberAreaWidth() - space*2+1, top, 2, blockheight,  QColor(0, 196, 0));
}

bool TextEdit::startsRegion(const QTextBlock& block, int& id) {
	id = 0;
	if(!block.isValid())
		return false;
	if(!block.next().isValid())
		return false;

	TextBlockUserData* curdata = blockData(block);
	TextBlockUserData* nextdata = blockData(block.next());

	if(curdata->getFoldingIndent() < nextdata->getFoldingIndent()) {
		id = nextdata->getFoldingIndent();
		return true;
	}
	return false;
}

bool TextEdit::endsRegion(const QTextBlock& block, int& cid, int& nid) {
	bool b;
	return endsRegion(block, cid, nid, b);
}

bool TextEdit::endsRegion(const QTextBlock& block, int& cid, int& nid, bool& perfect) {
	cid = 0;
	nid = 0;
	if(!block.isValid())
		return false;
	if(!block.next().isValid()) {
		cid = blockData(block)->getFoldingIndent();
		nid = 0;
		cid = blockData(block)->getFoldingIndent();
		perfect = false;
		return true;
	}

	TextBlockUserData* curdata = blockData(block);
	TextBlockUserData* nextdata = blockData(block.next());

	if(curdata->getFoldingIndent() > nextdata->getFoldingIndent()) {
		cid = curdata->getFoldingIndent();
		nid = nextdata->getFoldingIndent();
		perfect = true;
		return true;
	}
	return false;
}

bool TextEdit::continuesRegion(QTextBlock& block, int& id) {
	id = 0;
	if(!block.isValid())
		return false;
	if(!block.next().isValid())
		return false;

	TextBlockUserData* curdata = blockData(block);
	TextBlockUserData* nextdata = blockData(block.next());

	if(curdata->getFoldingIndent() == nextdata->getFoldingIndent()) {
		id = curdata->getFoldingIndent();
		return true;
	}
	return false;
}

Document* TextEdit::toDocument(QTextDocument* doc) {
	return qobject_cast<Document*>(doc);
}

void TextEdit::resizeEvent(QResizeEvent* e) {
	QPlainTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	mLineNumberWidget->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
	mDocumentWatcher->setGeometry(QRect(cr.right() -  documentWatcherWidth(), cr.top(), documentWatcherWidth(), cr.height()));

}

void TextEdit::contextMenuEvent(QContextMenuEvent* e) {

	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();
	menu->addAction(tr("Show line numbers"));
	menu->addAction(tr("Auto indent selection"));
	menu->exec(e->globalPos());
	delete menu;

}

void TextEdit::keyPressEvent(QKeyEvent* e) {

	if(mCompleter && mCompleter->popup()->isVisible()) {
		switch (e->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return; // let the completer do default behavior
			default:
				break;
		 }
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
	if (!mCompleter || !isShortcut)
		customTextModification(e);

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!mCompleter || (ctrlOrShift && e->text().isEmpty()))
		return;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if(!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 2
	   || eow.contains(e->text().right(1)))) {
		mCompleter->popup()->hide();
		return;
	}

	if (completionPrefix != mCompleter->completionPrefix()) {
		mCompleter->setCompletionPrefix(completionPrefix);
		mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
	}

	QRect cr = cursorRect();
	cr.setWidth(mCompleter->popup()->sizeHintForColumn(0) + mCompleter->popup()->verticalScrollBar()->sizeHint().width());
	mCompleter->complete(cr);

}

void TextEdit::focusInEvent(QFocusEvent* e) {
	if(mCompleter)
		mCompleter->setWidget(this);
	QPlainTextEdit::focusInEvent(e);
}

void TextEdit::mouseDoubleClickEvent(QMouseEvent* e) {
	QPlainTextEdit::mouseDoubleClickEvent(e);

	if(e->button() == Qt::LeftButton) {
		QString str = textCursor().selectedText();
		QTextCursor cur = textCursor();
		cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

		QTextCursor iter = this->document()->find(str, cur, QTextDocument::FindWholeWords);

		QList<QTextEdit::ExtraSelection> selections;
		while(!iter.isNull()) {
			QTextEdit::ExtraSelection sel;
			sel.cursor = iter;
			sel.format = mSimilarOccurance;
			selections.append(sel);
			iter = document()->find(str, iter, QTextDocument::FindWholeWords);
		}
		setExtraSelections(selections);
	}
}

void TextEdit::mousePressEvent(QMouseEvent* e) {
	QPlainTextEdit::mousePressEvent(e);
	if(e->button() == Qt::LeftButton && mClicked == false) {
		mClicked == true;
		if((e->modifiers() & (Qt::AltModifier & Qt::ShiftModifier)) ==
		   (Qt::AltModifier & Qt::ShiftModifier)) {
			mExtraCursors.append(textCursor());
		} else {
			mExtraCursors.clear();
		}
	}
	lineNumberAreaMousePressEvent(e);
}

void TextEdit::mouseReleaseEvent(QMouseEvent* e) {
	QPlainTextEdit::mouseReleaseEvent(e);
	if(e->button() == Qt::LeftButton && mClicked == true) {
		mClicked = false;
	}
}

void TextEdit::wheelEvent(QWheelEvent* e) {

	if (e->modifiers() & Qt::ControlModifier) {
		const int delta = e->delta();
		if (delta < 0)
			zoomOut();
		else if (delta > 0)
			zoomIn();
		return;
	}
	QPlainTextEdit::wheelEvent(e);
	QPlainTextEdit::setFont(mFont);
	viewport()->update();
}

void TextEdit::paintEvent(QPaintEvent* e) {
	QPlainTextEdit::paintEvent(e);
	QPainter p(viewport());

	QTextBlock block = firstVisibleBlock();
	int y  = 0;
	while(block.isValid() && y < viewport()->height()) {
		y = blockBoundingGeometry(block).top();
		int height = blockBoundingGeometry(block).height();
		if(block.isVisible())
			y += height;
		if(block.isVisible() && block.next().isValid() && !block.next().isVisible()) {
			p.setPen(QColor(Qt::cyan).light(60));
			p.drawLine(0, y, viewport()->width(), y);
		}

		block = block.next();
	}

//	QPainter p(viewport());
//	QRect viewportRect = viewport()->rect();
//	QRectF blockSelectionCursorRect;
//	QPointF offset(contentOffset());
//	QTextBlock block = firstVisibleBlock();
//	Document* doc = toDocument(document());
//	QPlainTextDocumentLayout* doclayout = qobject_cast<QPlainTextDocumentLayout*>(doc->documentLayout());
//	QRectF r = blockBoundingRect(block).translated(offset);
//	while(block.isValid()) {


//		QTextLayout *layout = block.layout();



//		if (mIsBlockSelection
//		    && block.position() >= mBlockSelection.mFirstBlock.block().position()
//		    && block.position() <= mBlockSelection.mLastBlock.block().position()) {
//			if (block == textCursor().block()) {
//				QRectF rr = layout->lineForTextPosition(textCursor().positionInBlock()).rect();
//				rr.moveTop(rr.top() + r.top());
//				rr.setLeft(0);
//				rr.setRight(viewportRect.width() - offset.x());
//				QColor color = QColor(196, 200, 230);
//				// set alpha, otherwise we cannot see block highlighting and find scope underneath
//				color.setAlpha(128);
//				p.fillRect(rr, color);
//			}



//			QString text = block.text();
//			const TextEditor::TabSettings &ts = toDocument(document())->getTabSettings();
//			qreal spacew = QFontMetricsF(font()).width(QLatin1Char(' '));

//			int offset = 0;
//			int relativePos  =  ts.positionAtColumn(text, mBlockSelection.mFirstVisualColumn, &offset);
//			QTextLine line = layout->lineForTextPosition(relativePos);
//			qreal x = line.cursorToX(relativePos) + offset * spacew;

//			int eoffset = 0;
//			int erelativePos  =  ts.positionAtColumn(text, mBlockSelection.mLastVisualColumn, &eoffset);
//			QTextLine eline = layout->lineForTextPosition(erelativePos);
//			qreal ex = eline.cursorToX(erelativePos) + eoffset * spacew;

//			QRectF rr = line.naturalTextRect();
//			rr.moveTop(rr.top() + r.top());
//			rr.setLeft(r.left() + x);
//			if (line.lineNumber() == eline.lineNumber())
//				rr.setRight(r.left() + ex);
//			p.fillRect(rr, palette().highlight());
//			if ((mBlockSelection.mAnchor == TextBlockSelection::TopLeft
//			     && block == mBlockSelection.mFirstBlock.block())
//			    || (mBlockSelection.mAnchor == TextBlockSelection::BottomLeft
//				&& block == mBlockSelection.mLastBlock.block())
//			    ) {
//				rr.setRight(rr.left()+2);
//				blockSelectionCursorRect = rr;
//			}
//			for (int i = line.lineNumber() + 1; i < eline.lineNumber(); ++i) {
//				rr = layout->lineAt(i).naturalTextRect();
//				rr.moveTop(rr.top() + r.top());
//				rr.setLeft(r.left() + x);
//				p.fillRect(rr, palette().highlight());
//			}

//			rr = eline.naturalTextRect();
//			rr.moveTop(rr.top() + r.top());
//			rr.setRight(r.left() + ex);
//			if (line.lineNumber() != eline.lineNumber())
//				p.fillRect(rr, palette().highlight());
//			if ((mBlockSelection.mAnchor == TextBlockSelection::TopRight
//			     && block == mBlockSelection.mFirstBlock.block())
//			    || (mBlockSelection.mAnchor == TextBlockSelection::BottomRight
//				&& block == mBlockSelection.mLastBlock.block())) {
//				rr.setLeft(rr.right()-2);
//				blockSelectionCursorRect = rr;
//			}
//		}
//		r.setY(r.y() + blockBoundingRect(block).height());
//		block = block.next();
//	}


	//doclayout->draw(&p, this->getPaintContext());

}

void TextEdit::autoIndent() {

}

//void TextEdit::paintEvent(QPaintEvent* e) {
//	QPlainTextEdit::paintEvent(e);
//	QPainter p(this);
//	QTextBlock block = firstVisibleBlock();
//	int top = 0;
//	int maxHeight = e->rect().height();

//	while(block.isVisible() && block.isValid()  && top < maxHeight) {
//		top += blockBoundingGeometry(block).height();
//		if(block.next().isValid() && !block.next().isVisible()) {
//			p.setPen(QColor(255, 0, 0));
//			p.drawLine(0, top, e->rect().width(), top);
//		}
//		block = block.next();
//	}
//}


TextEdit::~TextEdit()
{

}

void TextEdit::initCompleter() {
	Document* doc = toDocument(document());

	if(doc->hasHighlighter()) {
		Highlighter* hiltter = doc->getHighlighter();
		QSharedPointer<HighlightDefinition> def = hiltter->getDefinition();


		if(!def.isNull()) {
			const QHash<QString, QSharedPointer<KeywordList> >& list = def->keywords();

			for(auto& i: list) {
				QString key = list.key(i);
				QStringList list = i->toStringList();
				for(QString& it: list) {

					QStandardItem* item(new QStandardItem());
					if(key == "functions") {
						item->setIcon(QIcon(":/icons/Icons/function.ico"));
						//item->setForeground(hiltter->getFormat(Highlighter::Function).foreground());
					} else if(key == "keywords"){
						item->setIcon(QIcon(":/icons/Icons/keyword.ico"));
						//item->setForeground(hiltter->getFormat(Highlighter::Keyword).foreground());
					} else if(key == "predefined constant"){
						//item->setForeground(hiltter->getFormat(Highlighter::Others).foreground());
					} else if(key == "datatypes"){
						//item->setForeground(hiltter->getFormat(Highlighter::DataType).foreground());
					}
					item->setText(it);
					mCompleterModel->appendRow(item);
				}

			}
			mCompleterModel->sort(1);

			mCompleter->setWidget(this);
			mCompleter->setCompletionMode(QCompleter::PopupCompletion);
			mCompleter->setCaseSensitivity(Qt::CaseInsensitive);

		}

		connect(mCompleter, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

	}


}

void TextEdit::setTabSettings(const TextEditor::TabSettings& settings) {
	Document* doc = toDocument(document());
	QFontMetrics metrics(font());
	int len = metrics.width(' ') * settings.mTabSize;
	setTabStopWidth(len);
	doc->setTabSettings(settings);
}


void TextEdit::updateLineNumberArea(const QRect& rect, int y) {
	mViewportRect = rect;
	if (y)
		mLineNumberWidget->scroll(0, y);
	else
		mLineNumberWidget->update(0, rect.y(), mLineNumberWidget->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateDocumentLength(0);
}




void TextEdit::updateDocumentLength(int ) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEdit::lineNumberAreaPaintEvent(QPaintEvent *e) {


	int space = fontMetrics().width(QLatin1Char('_'));


	QTextBlock prevblock = firstVisibleBlock();
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	QPainter painter(mLineNumberWidget);
	painter.fillRect(e->rect(), mLineNumberFormat.background().color());
	painter.fillRect(e->rect().right()-space*2, e->rect().top(), e->rect().right(), e->rect().bottom(), mLineNumberFormat.background().color().dark(130));
	//int blockid = 0;

	while (block.isValid() && top <= e->rect().bottom()) {

		int blockheight = blockBoundingRect(block).height();
//		if(textCursor().block().blockNumber() == block.blockNumber()) {
//			grad.setStart(e->rect().width(), top);
//			grad.setFinalStop(0, top);
//			painter.fillRect(0, top, (mLineNumberWidget->width()), fontMetrics().height(), QBrush(QColor(255, 255, 255, 96)));
//		}
		if (block.isVisible() && bottom >= e->rect().top()) {



			QString number = QString::number(blockNumber + 1);
			painter.setPen(mLineNumberFormat.foreground().color());
			QFont fnt = font();
			fnt.setBold(mLineNumberFormat.font().bold());
			fnt.setItalic(mLineNumberFormat.font().italic());

			painter.setFont(fnt);
			painter.drawText(0, top, lineNumberAreaWidth() - space*2 - 4, fontMetrics().height(), Qt::AlignRight, number);


			if(block.userData() != nullptr) {
				drawBookMark(painter, block, top, bottom, space);
				drawModification(painter, block, top, bottom, space);
				if(!mRegionUnderCursor.isEmpty() && mRegionUnderCursor.contains(block))
						drawIndentationPipes(painter, block, top, bottom, space, mRegionVisualizerSelectedFormat);
				else
						drawIndentationPipes(painter, block, top, bottom, space, mRegionVisualizerFormat);
			}


		}
		prevblock = block;
		block = block.next();



		if(prevblock.isVisible()) {
			top = bottom;
			bottom = top + (int) blockheight;
		}
		++blockNumber;

	}


}

void TextEdit::documentWatchPaintEvent(QPaintEvent *e) {
	QPainter p(mDocumentWatcher);
	p.fillRect(e->rect(), Qt::lightGray);

	QFont fnt = font();
	fnt.setPointSizeF(fnt.pointSizeF() / 5.0);
	QFontMetrics metrics(fnt);
	p.setFont(fnt);
	DocumentMap* watch = qobject_cast<DocumentMap*>(mDocumentWatcher);

	QTextBlock blck = toDocument(document())->begin();
	QTextBlock end = toDocument(document())->end();

	int y = 0;
	while(blck != end) {
		p.setFont(fnt);
		p.setBrush(blck.charFormat().foreground());
		p.drawText(0, y, blck.text());
		blck = blck.next();
		y += metrics.height();
	}

}

const QString TextEdit::textUnderCursor() {
	QTextCursor cur = textCursor();
	cur.select(QTextCursor::WordUnderCursor);
	return cur.selectedText();
}

QWidget* TextEdit::getParentWidget() {
	return mParentWidget;
}

void TextEdit::setLineNumberFormat(const QTextCharFormat& fmt) {
	mLineNumberFormat = fmt;
}

void TextEdit::setSimilarOccuranceFormat(const QTextCharFormat& fmt) {
	mSimilarOccurance = fmt;
}

void TextEdit::setSelectedLine(const QTextCharFormat& fmt) {
	mCurrentLine = fmt;
}

void TextEdit::setRegionVisualizerFormat(const QTextCharFormat& fmt) {
	mRegionVisualizerFormat = fmt;
}

void TextEdit::setRegionVisualizerSelectedFormat(const QTextCharFormat& fmt) {
	mRegionVisualizerSelectedFormat = fmt;
}

void TextEdit::setFont(const QFont& fnt) {
	mFont = fnt;
	QPlainTextEdit::setFont(mFont);
	QFontMetrics metrics(font());
	setTabStopWidth(metrics.width(' ')*gVirtualTabSize);
}


void TextEdit::documentWatchMousePressEvent(QMouseEvent*) {

}

void TextEdit::documentWatchMouseReleaseEvent(QMouseEvent*)
{

}

void TextEdit::documentWatchMouseMoveEvent(QMouseEvent*) {

}


int TextEdit::lineNumberAreaWidth() {

	int digits = getDigitCount();
	int space = 3+fontMetrics().width(QLatin1Char('_')) * digits;

	 return space;

}

int TextEdit::getDigitCount() {
	int digits = 8;
	int max = qMax(1, blockCount());
	while (max >= 1000) {
		max /= 1000;
		++digits;
	}
	return digits;
}


int TextEdit::documentWatcherWidth() {
	return width() / 5;
}

TextBlockUserData* TextEdit::blockData(const QTextBlock& block) {
	return static_cast<TextBlockUserData*>(block.userData());
}

QList<QTextBlock> TextEdit::getRegionSubBlocks(const QTextBlock& block) {
	QList<QTextBlock> list;
	if(!toDocument(document())->hasHighlighter())
		return list;
	if(!block.isValid())
		return list;

	TextBlockUserData* data = blockData(block);
	if(data == nullptr)
		return list;

	int regionid = data->getFoldingIndent();


	QTextBlock iblock = block;
	int id;
	if(startsRegion(iblock, id) && id >= regionid) {
		list.append(iblock);
		iblock = iblock.next();
	} else {
		iblock = getBlockRegionStart(block);
		list.append(iblock);
		iblock = iblock.next();
	}

	QTextBlock endblock = getBlockRegionEnd(iblock);

	while(iblock != endblock) {
		list.append(iblock);
		iblock = iblock.next();
	}

	return list;
}

QTextBlock TextEdit::getBlockRegionStart(const QTextBlock& block) {
	if(!block.isValid())
		return QTextBlock();
	int regionId = blockData(block)->getFoldingIndent();

	int id;
	QTextBlock iblock = block;
	while(iblock.isValid()) {
		if(blockData(iblock)->getFoldingIndent() >= regionId)
			iblock = iblock.previous();
		else
			break;
	}
	return iblock;
}

QTextBlock TextEdit::getBlockRegionEnd(const QTextBlock& block) {
	if(!block.isValid())
		return QTextBlock();
	int regionId = blockData(block)->getFoldingIndent();

	QTextBlock iblock = block;
	while(iblock.isValid()){
		if(blockData(iblock)->getFoldingIndent() >= regionId)
			iblock = iblock.next();
		else
			break;
	}
	return iblock;
}

void TextEdit::highlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(200, 220, 255);

		selection.format = mCurrentLine;
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		mRegionUnderCursor = getRegionSubBlocks(selection.cursor.block());
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
	mLineNumberWidget->repaint();
}


void TextEdit::handleBlockSelection(int rowdiff, int coldiff) {
	Document* doc = toDocument(document());
	if(!mIsBlockSelection) {
		mBlockSelection.fromSelection(doc->getTabSettings(), textCursor());
		mIsBlockSelection = true;
	}
	mBlockSelection.moveAnchor(mBlockSelection.anchorBlockNumber() + rowdiff, mBlockSelection.anchorColumnNumber() + coldiff);
	setTextCursor(mBlockSelection.selection(doc->getTabSettings()));
	viewport()->update();
}



void TextEdit::lineNumberAreaMouseMoveEvent(QMouseEvent* e) {
	QFontMetrics metrics(font());
	int width = metrics.width('9');
	if(e->x() > lineNumberAreaWidth() - width*2 && e->x() <  lineNumberAreaWidth())  {
		QTextBlock block = blockAt(e->pos());
		mRegionUnderCursor = getRegionSubBlocks(block);
	} else {
		mRegionUnderCursor.clear();
	}
	mLineNumberWidget->repaint();
}

void TextEdit::lineNumberAreaWheelEvent(QWheelEvent* e){
	wheelEvent(e);
}

void TextEdit::lineNumberAreaContextMenuEvent(QContextMenuEvent* e) {
	QTextBlock block = blockAt(e->pos());
	TextBlockUserData* data = blockData(block);
	bool book = data->isBookmark();
	bool deb = data->isDebug();
	LineNumberArea* linenumbers = qobject_cast<LineNumberArea*>(mLineNumberWidget);
	linenumbers->mBookMarkAction->setChecked(book);
	linenumbers->mBreakPointAction->setChecked(deb);

	QMenu* menu = new QMenu(mLineNumberWidget);
	menu->addActions({linenumbers->mBookMarkAction, linenumbers->mBreakPointAction});
	menu->exec(e->globalPos());
	delete menu;
	data->setBookmark(linenumbers->mBookMarkAction->isChecked());
	data->setDebug(linenumbers->mBreakPointAction->isChecked());
}
