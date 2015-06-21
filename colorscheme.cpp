#include "colorscheme.hpp"
#include "TextEditor/textedit.hpp"
#include "issuelist.hpp"
#include <QDomAttr>
#include <QDomElement>
#include <QDomNode>
#include <QFileInfo>
#include <QBrush>

void Colorscheme::addIdKey(int fmt, const QString& key)
{
	mIDByKey[key] = fmt;
	mKeyByID[fmt] = key;
	qDebug() << "ID: " << fmt << "Added with key: " << key;
}

Colorscheme::Colorscheme():
	mFile("")
{

	addIdKey(Selection, "Selection");
	addIdKey(BasicText, "Text");
	addIdKey(KeyWord, "Keyword");
	addIdKey(DataType, "Datatype");
	addIdKey(Number, "Number");
	addIdKey(Char, "Char");
	addIdKey(String, "String");
	addIdKey(Comment, "Comment");
	addIdKey(Alert, "Alert");
	addIdKey(Error, "Error");
	addIdKey(Function, "Function");
	addIdKey(RegionMarker, "RegionMarker");
	addIdKey(Others, "Others");
	addIdKey(Local, "Local");
	addIdKey(Global, "Global");
	addIdKey(Field, "Field");
	addIdKey(UserDefinedStructure, "UserDefinedStructure");
	addIdKey(UserDefinedFunction, "UserDefinedFunction");
	addIdKey(Operator, "Operator");
	addIdKey(SimilarOccurance, "SimilarOccurance");
	addIdKey(LineNumber, "LineNumber");
	addIdKey(RegionVisualizer, "RegionVisualizer");
	addIdKey(RegionVisualizerSelected, "RegionVisualizerSelected");
	addIdKey(Selection, "Selection");
	addIdKey(Parentheses, "Parentheses");
	addIdKey(StdOut, "StdOut");
	addIdKey(StdErr, "StdErr");
	addIdKey(ConsoleText, "ConsoleText");
	addIdKey(IssueError, "IssueError");
	addIdKey(IssueWarning, "IssueWarning");
	loadDefaults();
}





Colorscheme::Colorscheme(const QString& path) :
	Colorscheme()
{
	load(path);

}

void Colorscheme::loadDefaults() {
	using namespace Qate;
	mFile = "Default scheme";

	//Set all backgrounds without brus first.
	for(int i = 0; i < Format::Formats; i++)
		mFormats[i].setBackground(QBrush(Qt::NoBrush));

	mFormats[Selection].setBackground(Qt::darkBlue);
	mFormats[Selection].setForeground(Qt::white);
	mFormats[BasicText].setForeground(Qt::black);
	mFormats[BasicText].setBackground(Qt::white);
	mFormats[KeyWord] = DefaultColors::instance().keywordFormat();
	mFormats[KeyWord].setBackground(QBrush());
	mFormats[DataType] = DefaultColors::instance().dataTypeFormat();
	mFormats[Char] = DefaultColors::instance().charFormat();
	mFormats[String] = DefaultColors::instance().stringFormat();
	mFormats[Comment] = DefaultColors::instance().commentFormat();
	mFormats[Alert] = DefaultColors::instance().alertFormat();
	mFormats[Error] = DefaultColors::instance().errorFormat();
	mFormats[Function] = DefaultColors::instance().functionFormat();
	mFormats[RegionMarker] = DefaultColors::instance().regionMarkerFormat();
	mFormats[Others] = DefaultColors::instance().othersFormat();
	mFormats[LineNumber].setForeground(QColor(Qt::gray));
	mFormats[LineNumber].setBackground(QColor(Qt::gray).dark(200));
	mFormats[SimilarOccurance].setForeground(QColor(255, 255, 255));
	mFormats[SimilarOccurance].setBackground(QColor(0, 200, 255));
	mFormats[RegionVisualizer].setForeground(QColor(Qt::gray));
	mFormats[RegionVisualizer].setBackground(QColor(255, 255, 255));
	mFormats[RegionVisualizerSelected].setForeground(QColor(Qt::red));
	mFormats[RegionVisualizerSelected].setBackground(QColor(255, 255, 255));

	mFormats[Parentheses].setForeground(QColor(0, 128, 0));

	mFormats[Number].setForeground(QColor(Qt::darkRed));
	mFormats[Operator].setForeground(QColor(Qt::gray).dark(180));

	mFormats[StdErr].setForeground(QColor(Qt::red));
	mFormats[StdOut].setForeground(mFormats[BasicText].foreground());

	mFormats[IssueError] = mFormats[StdErr];
	mFormats[IssueWarning] = mFormats[StdOut];

}

bool Colorscheme::load(const QString& path) {
	mXMLDoc.clear();
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
	QDomElement mainelement = mXMLDoc.firstChildElement("colorscheme");
	mName = mainelement.attribute("name");
	qDebug() << mName;
	readScheme(mainelement.toElement());

	return true;
}


const QString& Colorscheme::defenitionFile() {
	return mFile;
}

void Colorscheme::applyToHighlighter(TextEditor::Internal::Highlighter* hiltter) {
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Keyword, mFormats[KeyWord]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::DataType, mFormats[DataType]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Decimal, mFormats[Number]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::BaseN, mFormats[Number]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Float, mFormats[Number]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Char, mFormats[Char]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::String, mFormats[String]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Comment, mFormats[Comment]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Alert, mFormats[Alert]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Error, mFormats[Error]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Function, mFormats[Function]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::RegionMarker, mFormats[RegionMarker]);
	hiltter->configureFormat(TextEditor::Internal::Highlighter::Others, mFormats[Others]);

	Fate::FateHighlighter* fHiltter = qobject_cast<Fate::FateHighlighter*>(hiltter);
	if(fHiltter != nullptr) {
		fHiltter->setOperatorFmt(mFormats[Operator]);
	}

}

void Colorscheme::applyToTextEdit(TextEdit* edit) {
	QPalette palette = edit->palette();
	palette.setBrush(QPalette::Active, QPalette::Background, mFormats[BasicText].background());
	palette.setBrush(QPalette::Inactive, QPalette::Background, mFormats[BasicText].background());
	palette.setBrush(QPalette::Active, QPalette::Foreground, mFormats[BasicText].foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Foreground, mFormats[BasicText].foreground());
	palette.setBrush(QPalette::Active, QPalette::Base, mFormats[BasicText].background());
	palette.setBrush(QPalette::Inactive, QPalette::Base, mFormats[BasicText].background());
	palette.setBrush(QPalette::Active, QPalette::Text, mFormats[BasicText].foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Text, mFormats[BasicText].foreground());

	palette.setBrush(QPalette::Highlight, mFormats[Selection].background());
	palette.setBrush(QPalette::HighlightedText, mFormats[Selection].foreground());
	edit->setPalette(palette);

	edit->setLineNumberFormat(mFormats[LineNumber]);
	edit->setSimilarOccuranceFormat(mFormats[SimilarOccurance]);
	edit->setRegionVisualizerFormat(mFormats[RegionVisualizer]);
	edit->setRegionVisualizerSelectedFormat(mFormats[RegionVisualizerSelected]);
	edit->setErrorUnderlineFormat(mFormats[Error]);
	edit->setSelectedParenthesesFormat(mFormats[Parentheses]);

}

void Colorscheme::applyToConsole(Console* console) {
	QPalette palette = console->palette();


	QTextCharFormat bg = mFormats[ConsoleText].background().isOpaque() ? mFormats[BasicText] :  mFormats[ConsoleText];
	QTextCharFormat fg = mFormats[ConsoleText].foreground().isOpaque() ? mFormats[BasicText] :  mFormats[ConsoleText];
	palette.setBrush(QPalette::Active, QPalette::Base, bg.background());
	palette.setBrush(QPalette::Inactive, QPalette::Base, bg.background());
	palette.setBrush(QPalette::Active, QPalette::Text, fg.foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Text, fg.foreground());
	palette.setBrush(QPalette::Highlight, mFormats[Selection].background());
	palette.setBrush(QPalette::HighlightedText, mFormats[Selection].foreground());
	console->setPalette(palette);
	console->setStdErrFormat(mFormats[StdErr]);
	console->setStdOutFormat(mFormats[StdOut]);
}

void Colorscheme::applyToIssueList(IssueList* list) {
	QPalette palette = list->palette();
	palette.setBrush(QPalette::Active, QPalette::Base, mFormats[BasicText].background());
	palette.setBrush(QPalette::Inactive, QPalette::Base, mFormats[BasicText].background());
	palette.setBrush(QPalette::Active, QPalette::Text, mFormats[BasicText].foreground());
	palette.setBrush(QPalette::Inactive, QPalette::Text, mFormats[BasicText].foreground());
	list->setPalette(palette);
	list->setErrorFormat(mFormats[IssueError]);
	list->setWarningFormat(mFormats[IssueWarning]);
}


void Colorscheme::readScheme(QDomElement element) {
	QDomNodeList items = element.elementsByTagName("style");
	for(int i = 0; i < items.count(); i++) {
		QDomNode node = items.at(i);
		if(node.isElement()) {
			QDomElement elem = node.toElement();
			QString name = elem.attribute("name");
			if(!name.isEmpty()) {
				QString foreground = elem.attribute("foreground", "");
				QString background = elem.attribute("background", "");
				QString bold = elem.attribute("bold", "false");
				QString italic = elem.attribute("italic", "false");

				qDebug() << name << foreground << background << bold << italic;

				if(mIDByKey.contains(name)) {
					qDebug() << "Format index found!";
					QTextCharFormat& fmt = mFormats[mIDByKey[name]];
					bool ok = false;
					if(!foreground.isEmpty())
						fmt.setForeground(QColor(foreground));

					if(!background.isEmpty())
						fmt.setBackground(QColor(background));
					else
						fmt.setBackground(QBrush(Qt::NoBrush));

					if(bold == "false")
						fmt.setFontWeight(QFont::Normal);
					else if(bold == "true")
						fmt.setFontWeight(QFont::Bold);
					if(italic == "false")
						fmt.setFontItalic(false);
					else if(italic == "true")
						fmt.setFontItalic(true);
				}
			}
		}
	}
}


bool Colorscheme::save(const QString& path) {


	mXMLDoc.clear();

	QDomElement first = mXMLDoc.createElement("colorscheme");
	first.setAttribute("verison", "1.0");
	first.setAttribute("name", "untitled"); //for now
	mXMLDoc.appendChild(first);
	qDebug() << "First 'colorscheme' element should be added.";
	for(int i = 0; i < Formats; i++) {
		QDomElement elem = mXMLDoc.createElement("style");


		QTextCharFormat& fmt = mFormats[i];

		bool bold = (fmt.fontWeight() == QFont::Bold) ? true : false;
		bool italic = fmt.fontItalic();

		QString isbold = bold ? "true" : "false";
		QString isitalic = italic ? "true" : "false";

		QBrush fgbrush = fmt.foreground();
		QBrush bgbrush = fmt.background();
		QString fg = fgbrush.color().name();
		QString bg = bgbrush.color().name();

		if(fgbrush.isOpaque())
			elem.setAttribute("foreground", fg);
		else
			elem.setAttribute("foreground", mFormats[BasicText].foreground().color().name());

		if(bgbrush.isOpaque())
			elem.setAttribute("background", bg);

		if(bold)
			elem.setAttribute("bold", isbold);
		if(italic)
			elem.setAttribute("italic", isitalic);
		first.appendChild(elem);
		elem.setAttribute("name", mKeyByID[i]);
	}


	QString filePath = path;
	if(filePath.isEmpty())
		filePath = mFile;

	if(QFile::exists(filePath)) {
		QFileInfo info(filePath);
		QString fileName = info.fileName();
		QString name = fileName.split(".")[0];
		if(name.right(5) != "_user") {
			QString type = fileName.split(".")[1];
			filePath = QString("%1/%2_user.%3").arg(info.absolutePath()).arg(name).arg(type);
		}
	} else {
		filePath = path;
	}


	mFile = filePath;
	qDebug() << mFile;

	QFile file(filePath);

	if(file.open(QIODevice::Text | QIODevice::WriteOnly)) {
		QTextStream outstream(&file);
		outstream << mXMLDoc.toString();
		return true;
	}

	return false;
}
