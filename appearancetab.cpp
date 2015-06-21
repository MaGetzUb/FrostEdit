#include "appearancetab.hpp"
#include "ui_appearancetab.h"


#include "settings.hpp"
#include "colorscheme.hpp"
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QFileInfo>
#include <QDebug>
#include <QFile>

AppearanceTab::AppearanceTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AppearanceTab),
	mCurrentItem(nullptr)
{
	ui->setupUi(this);

	mBasicText = new FormattedListItem("Basic text", ui->syntaxFormats);
	mKeyword = new FormattedListItem("Keywords", ui->syntaxFormats);
	mDataType = new FormattedListItem("Data types", ui->syntaxFormats);
	mString = new FormattedListItem("String literals", ui->syntaxFormats);
	mComment = new FormattedListItem("Comments", ui->syntaxFormats);
	mFunction = new FormattedListItem("Functions", ui->syntaxFormats);
	mOthers = new FormattedListItem("Others", ui->syntaxFormats);
	mOperator = new FormattedListItem("Operators", ui->syntaxFormats);
	mNumber = new FormattedListItem("Numbers", ui->syntaxFormats);
	mSelection = new FormattedListItem("Selection", ui->syntaxFormats);
	mLocal = new FormattedListItem("Locals", ui->syntaxFormats);
	mGlobal = new FormattedListItem("Globals", ui->syntaxFormats);
	mField = new FormattedListItem("Fields", ui->syntaxFormats);
	mUserDefinedFunction = new FormattedListItem("User defined function", ui->syntaxFormats);
	mUserDefinedStructure = new FormattedListItem("User defined structure", ui->syntaxFormats);
	mLabel = new FormattedListItem("Labels", ui->syntaxFormats);

	mParenthesesMatch = new FormattedListItem("Matching parenthesis ([{}])", ui->syntaxFormats);
	mLineNumber = new FormattedListItem("Line number", ui->syntaxFormats);
	mSimilarOccurance = new FormattedListItem("Similar occurance", ui->syntaxFormats);
	mRegionVisualizer = new FormattedListItem("Region visualizer [-]", ui->syntaxFormats);
	mRegionVisualizerSelected = new FormattedListItem("Region visualizer selected [-]", ui->syntaxFormats);


	mIssueError = new FormattedListItem("Error", ui->issueFormats);
	mIssueWarning = new FormattedListItem("Warning", ui->issueFormats);

	mStdErr = new FormattedListItem("Standard error", ui->consoleFormats);
	mStdOut = new FormattedListItem("Standard output", ui->consoleFormats);

	connect(ui->syntaxFormats, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectionChanged(QListWidgetItem*)));
	connect(ui->consoleFormats, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectionChanged(QListWidgetItem*)));
	connect(ui->issueFormats, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectionChanged(QListWidgetItem*)));
	ui->textForegroundColor->setDisabled(true);
	ui->textBackgroundColor->setDisabled(true);
	ui->dontUseBGButton->setDisabled(true);
	ui->dontUseFGButton->setDisabled(true);

	QString sheet = Settings::get("Appearance/Stylesheet").toString();
	if(!sheet.isEmpty())
		ui->stylesheetLabel->setText(tr("Current stylesheet: %1").arg(QFileInfo(sheet).fileName()));
	else
		ui->stylesheetLabel->setText(tr("Current stylesheet: %1").arg(""));
}

AppearanceTab::~AppearanceTab()
{
	delete ui;
}


void AppearanceTab::setSyntaxStyle(Colorscheme* style) {
	mColorScheme = style;
	updateSchemePreciseThings();
}

void AppearanceTab::setTextEditFont(const QFont& font) {
	mFont = font;
	ui->fontNameView->setText(tr("Current font: '%1' size: %2").arg(font.family()).arg(font.pointSize()));


	for(int i = 0; i < ui->syntaxFormats->count(); i++) {
		FormattedListItem* item = static_cast<FormattedListItem*>(ui->syntaxFormats->item(i));
		item->setFont(font);
	}
	for(int i = 0; i < ui->issueFormats->count(); i++) {
		FormattedListItem* item = static_cast<FormattedListItem*>(ui->syntaxFormats->item(i));
		item->setFont(font);
	}
	for(int i = 0; i < ui->consoleFormats->count(); i++) {
		FormattedListItem* item = static_cast<FormattedListItem*>(ui->syntaxFormats->item(i));
		item->setFont(font);
	}

}


void AppearanceTab::useSyntaxStyle() {
	QPalette palette = ui->syntaxFormats->palette();
	palette.setBrush(QPalette::Active, QPalette::Base, mColorScheme->mFormats[Colorscheme::BasicText].background());
	palette.setBrush(QPalette::Inactive, QPalette::Base, mColorScheme->mFormats[Colorscheme::BasicText].background());
	palette.setBrush(QPalette::Active, QPalette::Text, mColorScheme->mFormats[Colorscheme::BasicText].foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Text, mColorScheme->mFormats[Colorscheme::BasicText].foreground());
	ui->syntaxFormats->setPalette(palette);
	ui->consoleFormats->setPalette(palette);
	ui->issueFormats->setPalette(palette);

	mBasicText->setFormat(mColorScheme->mFormats[Colorscheme::BasicText]);
	mKeyword->setFormat(mColorScheme->mFormats[Colorscheme::KeyWord]);
	mDataType->setFormat(mColorScheme->mFormats[Colorscheme::DataType]);
	mString->setFormat(mColorScheme->mFormats[Colorscheme::String]);
	mComment->setFormat(mColorScheme->mFormats[Colorscheme::Comment]);
	mFunction->setFormat(mColorScheme->mFormats[Colorscheme::Function]);
	mOthers->setFormat(mColorScheme->mFormats[Colorscheme::Others]);
	mOperator->setFormat(mColorScheme->mFormats[Colorscheme::Operator]);
	mNumber->setFormat(mColorScheme->mFormats[Colorscheme::Number]);
	mSelection->setFormat(mColorScheme->mFormats[Colorscheme::Selection]);

	mUserDefinedFunction->setFormat(mColorScheme->mFormats[Colorscheme::UserDefinedFunction]);
	mUserDefinedStructure->setFormat(mColorScheme->mFormats[Colorscheme::UserDefinedStructure]);

	mLocal->setFormat(mColorScheme->mFormats[Colorscheme::Local]);
	mGlobal->setFormat(mColorScheme->mFormats[Colorscheme::Global]);
	mField->setFormat(mColorScheme->mFormats[Colorscheme::Field]);

	mLineNumber->setFormat(mColorScheme->mFormats[Colorscheme::LineNumber]);
	mSimilarOccurance->setFormat(mColorScheme->mFormats[Colorscheme::SimilarOccurance]);
	mRegionVisualizer->setFormat(mColorScheme->mFormats[Colorscheme::RegionVisualizer]);
	mRegionVisualizerSelected->setFormat(mColorScheme->mFormats[Colorscheme::RegionVisualizerSelected]);
	mParenthesesMatch->setFormat(mColorScheme->mFormats[Colorscheme::Parentheses]);

	mIssueError->setFormat(mColorScheme->mFormats[Colorscheme::IssueError]);
	mIssueWarning->setFormat(mColorScheme->mFormats[Colorscheme::IssueWarning]);

	mStdErr->setFormat(mColorScheme->mFormats[Colorscheme::StdErr]);
	mStdOut->setFormat(mColorScheme->mFormats[Colorscheme::StdOut]);
}

void AppearanceTab::updateSyntaxStyle() {
	mColorScheme->mFormats[Colorscheme::BasicText] = mBasicText->getFormat();
	mColorScheme->mFormats[Colorscheme::KeyWord] = mKeyword->getFormat();
	mColorScheme->mFormats[Colorscheme::DataType] = mDataType->getFormat();
	mColorScheme->mFormats[Colorscheme::String] = mString->getFormat();
	mColorScheme->mFormats[Colorscheme::Comment] = mComment->getFormat();
	mColorScheme->mFormats[Colorscheme::Function] = mFunction->getFormat();
	mColorScheme->mFormats[Colorscheme::Others] = mOthers->getFormat();
	mColorScheme->mFormats[Colorscheme::Operator] = mOperator->getFormat();
	mColorScheme->mFormats[Colorscheme::Number] = mNumber->getFormat();
	mColorScheme->mFormats[Colorscheme::Selection] = mSelection->getFormat();
	mColorScheme->mFormats[Colorscheme::Parentheses] = mParenthesesMatch->getFormat();

	mColorScheme->mFormats[Colorscheme::LineNumber] = mLineNumber->getFormat();
	mColorScheme->mFormats[Colorscheme::SimilarOccurance] = mSimilarOccurance->getFormat();
	mColorScheme->mFormats[Colorscheme::RegionVisualizer] = mRegionVisualizer->getFormat();
	mColorScheme->mFormats[Colorscheme::RegionVisualizerSelected] = mRegionVisualizerSelected->getFormat();

	mColorScheme->mFormats[Colorscheme::UserDefinedFunction] = mUserDefinedFunction->getFormat();
	mColorScheme->mFormats[Colorscheme::UserDefinedStructure] = mUserDefinedStructure->getFormat();
	mColorScheme->mFormats[Colorscheme::Local] = mLocal->getFormat();
	mColorScheme->mFormats[Colorscheme::Global] = mGlobal->getFormat();
	mColorScheme->mFormats[Colorscheme::Field] = mField->getFormat();

	mColorScheme->mFormats[Colorscheme::IssueError] = mIssueError->getFormat();
	mColorScheme->mFormats[Colorscheme::IssueWarning] = mIssueWarning->getFormat();

	mColorScheme->mFormats[Colorscheme::StdErr] = mStdErr->getFormat();
	mColorScheme->mFormats[Colorscheme::StdOut] = mStdOut->getFormat();
}



FormattedListItem* AppearanceTab::toFormattedListItem(QListWidgetItem* item) {
	return dynamic_cast<FormattedListItem*>(item);
}

void AppearanceTab::on_fontFormatItalic_toggled(bool checked) {
	if(mCurrentItem == nullptr)
		return;
	QTextCharFormat format = mCurrentItem->getFormat();
	format.setFontItalic(checked);
	mCurrentItem->setFormat(format);
}

void AppearanceTab::on_fontFormatBold_toggled(bool checked) {
	if(mCurrentItem == nullptr)
		return;
	QTextCharFormat format = mCurrentItem->getFormat();
	format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
	mCurrentItem->setFormat(format);
}


void AppearanceTab::selectionChanged(QListWidgetItem* item) {
	mCurrentItem = toFormattedListItem(item);
	QTextCharFormat format = mCurrentItem->getFormat();
	ui->fontFormatBold->setChecked(format.fontWeight() == QFont::Bold);
	ui->fontFormatItalic->setChecked(format.fontItalic());

	ui->textForegroundColor->setEnabled(true);
	ui->textBackgroundColor->setEnabled(true);

	ui->dontUseBGButton->setEnabled((mCurrentItem != nullptr && mCurrentItem != mBasicText) ? true : false);
	ui->dontUseFGButton->setEnabled((mCurrentItem != nullptr && mCurrentItem != mBasicText) ? true : false);

	QString qss("");
	if(item->foreground().isOpaque()) {
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(item->foreground().color().name()).arg(item->foreground().color().dark(200).name());
	} else {
		QListWidget* wid = item->listWidget();
		QPalette palette = wid->palette();
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(palette.text().color().name()).arg(palette.foreground().color().dark(200).name());
	}
	ui->textForegroundColor->setStyleSheet(qss);

	if(item->background().isOpaque()) {
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(item->background().color().name()).arg(item->background().color().dark(200).name());
	} else {
		QListWidget* wid = item->listWidget();
		QPalette palette = wid->palette();
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(palette.base().color().name()).arg(palette.background().color().dark(200).name());
	}
	ui->textBackgroundColor->setStyleSheet(qss);



}


void FormattedListItem::setFormat(const QTextCharFormat& fmt) {
	mFormat = fmt;
	QFont fnt = font();
	fnt.setWeight(mFormat.fontWeight());
	fnt.setItalic(mFormat.fontItalic());
	setFont(fnt);
	setForeground(mFormat.foreground());
	setBackground(mFormat.background());

}

const QTextCharFormat& FormattedListItem::getFormat() const {
	return mFormat;
}

void AppearanceTab::on_textForegroundColor_clicked() {
	if(mCurrentItem==nullptr)
		return;
	QTextCharFormat format = mCurrentItem->getFormat();

	QColor dcolor =  QColorDialog::getColor(format.foreground().color(), this, "Select a foreground color");
	QColor color;
	if(dcolor.isValid())
		color = dcolor;
	else
		color = format.foreground().color();

	format.setForeground(color);
	mCurrentItem->setFormat(format);

	if(mCurrentItem == mBasicText) {
		QPalette palette = ui->syntaxFormats->palette();
		palette.setBrush(QPalette::Active, QPalette::Text, mBasicText->getFormat().foreground());
		palette.setBrush(QPalette::Inactive, QPalette::Text, mBasicText->getFormat().foreground());
		ui->syntaxFormats->setPalette(palette);
		ui->consoleFormats->setPalette(palette);
		ui->issueFormats->setPalette(palette);
	}
}

void AppearanceTab::on_textBackgroundColor_clicked() {
	if(mCurrentItem==nullptr)
		return;
	QTextCharFormat format = mCurrentItem->getFormat();
	QColor dcolor = QColorDialog::getColor(format.background().color(), this, "Select a background color");
	QColor color;
	if(dcolor.isValid())
		color = dcolor;
	else
		color = format.background().color();
	format.setBackground(color);
	mCurrentItem->setFormat(format);

	if(mCurrentItem == mBasicText) {
		QPalette palette = ui->syntaxFormats->palette();
		palette.setBrush(QPalette::Active, QPalette::Base, mBasicText->getFormat().background());
		palette.setBrush(QPalette::Inactive, QPalette::Base, mBasicText->getFormat().background());
		ui->syntaxFormats->setPalette(palette);
		ui->consoleFormats->setPalette(palette);
		ui->issueFormats->setPalette(palette);
	}
}

void AppearanceTab::on_textFont_clicked() {
	bool ok;
	QFont font = QFontDialog::getFont(&ok, mFont, this, "Select a font");
	if(ok) {
		setTextEditFont(font);
		return;
	}
}

void AppearanceTab::on_dontUseBGButton_clicked() {
	if(mCurrentItem==nullptr)
		return;
	QTextCharFormat fmt = mCurrentItem->getFormat();
	fmt.setBackground(QBrush(Qt::NoBrush));
	mCurrentItem->setFormat(fmt);
	QString qss;
	if(mCurrentItem->background().isOpaque()) {
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(mCurrentItem->background().color().name()).arg(mCurrentItem->background().color().dark(200).name());
	} else {
		QListWidget* wid = mCurrentItem->listWidget();
		qss = QString("border-radius: 3px; background-color: %1; color: %2;").arg(mColorScheme->mFormats[Colorscheme::BasicText].background().color().name()).arg(mColorScheme->mFormats[Colorscheme::BasicText].background().color().dark(200).name());
	}
	ui->textBackgroundColor->setStyleSheet(qss);
}

void AppearanceTab::on_dontUseFGButton_clicked() {
	if(mCurrentItem==nullptr)
		return;
	QTextCharFormat fmt = mCurrentItem->getFormat();
	fmt.setForeground(QBrush(Qt::NoBrush));
	mCurrentItem->setFormat(fmt);

	QString qss;
	if(mCurrentItem->foreground().isOpaque()) {
		qss = tr("border-radius: 3px; background-color: %1; color: %2;").arg(mCurrentItem->foreground().color().name()).arg(mCurrentItem->foreground().color().dark(200).name());
	} else {
		QListWidget* wid = mCurrentItem->listWidget();
		qss = tr("border-radius: 3px; background-color: %1; color: %2;").arg(mColorScheme->mFormats[Colorscheme::BasicText].foreground().color().name()).arg(mColorScheme->mFormats[Colorscheme::BasicText].foreground().color().dark(200).name());
	}
	ui->textForegroundColor->setStyleSheet(qss);
}


void AppearanceTab::on_toolButton_clicked() {
	ui->stylesheetLabel->setText(tr("Current stylesheet: %1").arg(""));
	ui->stylesheetLabel->setToolTip("");
	Settings::set("Appearance/Stylesheet", "");
	useSyntaxStyle();
}

void AppearanceTab::on_selectStyleSheetButton_clicked() {
	QString path = QFileDialog::getOpenFileName(this, "Select a stylesheet", QDir::currentPath(), "Cascading stylesheet (*.css; *.qss);;All files (*.*)");
	if(!path.isEmpty()) {
		QFileInfo file(path);
		ui->stylesheetLabel->setText(tr("Current stylesheet: %1").arg(file.fileName()));
		ui->stylesheetLabel->setToolTip(file.absoluteFilePath());
		Settings::set("Appearance/Stylesheet", path);
	}
	useSyntaxStyle();
}


void AppearanceTab::on_resetColorSchemeButton_clicked() {
	mColorScheme->loadDefaults();
	updateSchemePreciseThings();
}

void AppearanceTab::updateSchemePreciseThings() {
	QString scheme;
	if(QFile::exists(mColorScheme->mFile)) {
		scheme = QFileInfo(mColorScheme->mFile).fileName();
	} else {
		scheme = mColorScheme->mFile;
	}
	useSyntaxStyle();
	ui->colorSchemeView->setText(tr("Current color scheme: %1").arg(scheme));
}

void AppearanceTab::on_openStyleSheet_clicked() {
	QString file = QFileDialog::getOpenFileName(this, "Select a colorscheme", QDir::currentPath(), "XML files (*.xml);;All files(*.*)");
	if(!file.isEmpty()) {
		mColorScheme->load(file);
		updateSchemePreciseThings();
		Settings::set("Appearance/ColorScheme", file);
	}
}

void AppearanceTab::on_saveStyleSheet_clicked() {
	mColorScheme->save();
}

void AppearanceTab::on_saveStyleSheetAs_clicked() {
	QString str = QFileDialog::getSaveFileName(this, "Save color scheme as", QDir::currentPath(), "XML Files (*.xml);;All files (*.*)");
	if(!str.isEmpty()) {
		mColorScheme->save(str);
	}

}
