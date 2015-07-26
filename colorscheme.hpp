#ifndef SYNTAXSTYLE_HPP
#define SYNTAXSTYLE_HPP

#include <QtXml>
#include <QTextCharFormat>
#include "TextEditor/qate/defaultcolors.h"
#include "TextEditor/fatehighlighter.hpp"
#include "settingsmenu.hpp"
#include "console.hpp"

class IssueList;
class TextEdit;
class Colorscheme {
	friend class AppearanceTab;


	QMap<int, QString> mKeyByID;
	QMap<QString, int> mIDByKey;


	enum Format {
		BasicText = 0,
		KeyWord,
		DataType,
		Char,
		String,
		Comment,
		Alert,
		Error,
		Function,
		RegionMarker,
		Others,
		Operator,
		Number,
		Selection,
		Local,
		Global,
		Field,
		UserDefinedStructure,
		UserDefinedFunction,
		LineNumber,
		SimilarOccurance,
		SelectedLineNumber,
		RegionVisualizer,
		RegionVisualizerSelected,
		Parentheses,
		StdOut,
		StdErr,
		ConsoleText,
		IssueError,
		IssueWarning,
		Formats
	};



	QTextCharFormat mFormats[Formats];
	QString mFile, mName;
	QDomDocument mXMLDoc;


	void addIdKey(int fmt, const QString& key);
	public:



		Colorscheme();
		Colorscheme(const QString&);

		void loadDefaults();
		bool load(const QString&);
		bool save(const QString& path = "");
		void readAttribute(const QString&);

		const QString& defenitionFile();

		void applyToHighlighter(TextEditor::Internal::Highlighter*);
		void applyToTextEdit(TextEdit*);
		void applyToConsole(Console*);
		void applyToIssueList(IssueList*);
	private:
		void readScheme(QDomElement);
		QTextCharFormat* formatByName(const QString&, int);
};

#endif // SYNTAXSTYLE_HPP
