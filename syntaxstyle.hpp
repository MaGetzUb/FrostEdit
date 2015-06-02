#ifndef SYNTAXSTYLE_HPP
#define SYNTAXSTYLE_HPP

#include <QtXml>
#include <QTextCharFormat>
#include "TextEditor/qate/defaultcolors.h"
#include "TextEditor/qate/highlighter.h"
#include "TextEditor/textedit.hpp"

class SyntaxStyle {
	QTextCharFormat mBasicTextFormat;
	QTextCharFormat mKeywordFormat;
	QTextCharFormat mDataTypeFormat;
	QTextCharFormat mDecimalFormat;
	QTextCharFormat mBaseNFormat;
	QTextCharFormat mFloatFormat;
	QTextCharFormat mCharFormat;
	QTextCharFormat mStringFormat;
	QTextCharFormat mCommentFormat;
	QTextCharFormat mAlertFormat;
	QTextCharFormat mErrorFormat;
	QTextCharFormat mFunctionFormat;
	QTextCharFormat mRegionMarkerFormat;
	QTextCharFormat mOthersFormat;
	QTextCharFormat mLineNumber;
	QTextCharFormat mSimilarOccurance;
	QTextCharFormat mRegionVisualizer;
	QTextCharFormat mRegionVisualizerSelected;

	QString mFile;
	QDomDocument mXMLDoc;

	public:

		SyntaxStyle();
		SyntaxStyle(const QString&);

		void loadDefaults();
		bool load(const QString&);
		bool save(const QString&);
		void readAttribute(const QString&);


		void applyToHighlighter(TextEditor::Internal::Highlighter* hiltter);
		void applyToTextEdit(TextEdit* edit);
	private:
		void readScheme(QDomElement element);
		QTextCharFormat* formatByName(const QString&);
};

#endif // SYNTAXSTYLE_HPP
