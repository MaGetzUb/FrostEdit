#include "syntaxstyle.hpp"

SyntaxStyle::SyntaxStyle():
	mFile("")
{

	loadDefaults();
}

SyntaxStyle::SyntaxStyle(const QString& path) :
	SyntaxStyle()
{
	load(path);
}

void SyntaxStyle::loadDefaults() {
	using namespace Qate;
	mBasicTextFormat.setForeground(Qt::black);
	mBasicTextFormat.setBackground(Qt::white);
	mKeywordFormat = DefaultColors::instance().keywordFormat();
	mDataTypeFormat = DefaultColors::instance().dataTypeFormat();
	mDecimalFormat = DefaultColors::instance().decimalFormat();
	mBaseNFormat = DefaultColors::instance().baseNFormat();
	mFloatFormat = DefaultColors::instance().floatFormat();
	mCharFormat = DefaultColors::instance().charFormat();
	mStringFormat = DefaultColors::instance().stringFormat();
	mCommentFormat = DefaultColors::instance().commentFormat();
	mAlertFormat = DefaultColors::instance().alertFormat();
	mErrorFormat = DefaultColors::instance().errorFormat();
	mFunctionFormat = DefaultColors::instance().functionFormat();
	mRegionMarkerFormat = DefaultColors::instance().regionMarkerFormat();
	mOthersFormat = DefaultColors::instance().othersFormat();
}

bool SyntaxStyle::load(const QString& path) {

	QFile file(path);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		mFile = path;
		if(!mXMLDoc.setContent(&file)) {
			qDebug() << "Parsing failed!";
			file.close();
			loadDefaults();
			return false;
		}

		file.close();
	}

	readScheme(mXMLDoc.firstChildElement());
	return true;
}

bool SyntaxStyle::save(const QString& path) {
	QDomElement element = mXMLDoc.firstChildElement();
	QDomNodeList list = element.elementsByTagName("style");
	for(int i = 0; i < list.size(); i++) {
		QDomNode node = list.at(i);
		if(node.isElement()) {

		}
	}
	return true;
}

void SyntaxStyle::applyToHighlighter(TextEditor::Internal::Highlighter* hiltter) {
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Keyword, mKeywordFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::DataType, mDataTypeFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Decimal, mDecimalFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::BaseN, mBaseNFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Float, mFloatFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Char, mCharFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::String, mStringFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Comment, mCommentFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Alert, mAlertFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Error, mErrorFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Function, mFunctionFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::RegionMarker, mRegionMarkerFormat);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Others, mOthersFormat);
}

void SyntaxStyle::applyToTextEdit(TextEdit* edit) {
	QPalette palette = edit->palette();
	palette.setBrush(QPalette::Active, QPalette::Base, mBasicTextFormat.background());
	palette.setBrush(QPalette::Inactive, QPalette::Base, mBasicTextFormat.background());
	palette.setBrush(QPalette::Active, QPalette::Text, mBasicTextFormat.foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Text, mBasicTextFormat.foreground());
	edit->setPalette(palette);

	edit->setLineNumberFormat(mLineNumber);
	edit->setSimilarOccuranceFormat(mSimilarOccurance);
	edit->setRegionVisualizerFormat(mRegionVisualizer);
	edit->setRegionVisualizerSelectedFormat(mRegionVisualizerSelected);
}

QTextCharFormat*SyntaxStyle::formatByName(const QString& str) {
	if(str == "Text")
		return &mBasicTextFormat;
	else if(str == "Keyword")
		return &mKeywordFormat;
	else if(str == "Datatype")
		return &mDataTypeFormat;
	else if(str == "Number")
		return &mDecimalFormat;
	else if(str == "BaseN")
		return &mBaseNFormat;
	else if(str == "Float")
		return &mFloatFormat;
	else if(str == "Char")
		return &mCharFormat;
	else if(str == "String")
		return &mStringFormat;
	else if(str == "Comment")
		return &mCommentFormat;
	else if(str == "Alert")
		return &mAlertFormat;
	else if(str == "Error")
		return &mErrorFormat;
	else if(str == "Function")
		return &mFunctionFormat;
	else if(str == "RegionMarker")
		return &mRegionMarkerFormat;
	else if(str == "Others")
		return &mOthersFormat;
	else if(str == "SimilarOccurance")
		return &mSimilarOccurance;
	else if(str == "LineNumber")
		return &mLineNumber;
	else if(str == "RegionVisualizer")
		return &mRegionVisualizer;
	else if(str == "RegionVisualizerSelected")
		return &mRegionVisualizerSelected;
	return nullptr;
}

void SyntaxStyle::readScheme(QDomElement element) {
	QDomNodeList items = element.elementsByTagName("style");
	for(int i = 0; i < items.count(); i++) {
		QDomNode node = items.at(i);
		if(node.isElement()) {
			QDomElement elem = node.toElement();
			QString name = elem.attribute("name");
			if(!name.isEmpty()) {
				QString foreground = elem.attribute("foreground");
				QString background = elem.attribute("background");
				QString bold = elem.attribute("bold", "false");
				QString italic = elem.attribute("bold", "false");

				qDebug() << name << foreground << background << bold << italic;

				QTextCharFormat* fmt = formatByName(name);
				if(fmt != nullptr) {
					bool ok = false;
					if(!foreground.isEmpty()) {
						fmt->setForeground(QColor(foreground));
					}
					if(!background.isEmpty()) {
						fmt->setBackground(QColor(background));
					}
					if(bold == "false")
						fmt->setFontWeight(QFont::Normal);
					else if(bold == "true")
						fmt->setFontWeight(QFont::Bold);
					if(italic == "false")
						fmt->setFontItalic(false);
					else if(italic == "true")
						fmt->setFontItalic(true);
				}
			}
		}
	}
}

