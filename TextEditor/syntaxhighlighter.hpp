#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>


class SyntaxHighLighter: public QSyntaxHighlighter {
		Q_OBJECT
	public:
		explicit SyntaxHighLighter(QTextDocument* parent = 0);
		~SyntaxHighLighter();

		void setDefenition();
		void setParser();

	protected:
		void highlightBlock(const QString &str);

};

#endif // SYNTAXHIGHLIGHTER_HPP
