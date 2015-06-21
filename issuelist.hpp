#ifndef ISSUELIST_HPP
#define ISSUELIST_HPP

#include <QListWidget>
#include "settingsmenu.hpp"

class IssueItem : public FormattedListItem {


		QString mExplanation;
		QString mFile;
		int mRow, mCol;
		int mType;

	public:
		enum Type {
			Error,
			Warning
		};

		IssueItem(const QIcon& icon, const QString& txt, QListWidget* parent = 0) :
			FormattedListItem(icon, txt, parent),
			mExplanation(""),
			mFile(""),
			mRow(0),
			mCol(0),
			mType(0)
		{}
		~IssueItem(){}

		void setExplanation(const QString& str);
		void setFile(const QString& filepath);
		void setRow(int row);
		void setColumn(int col);



		const QString& getExplanation();
		const QString& getFile();
		int getRow();
		int getColumn();

};


class IssueList : public QListWidget {
	Q_OBJECT
	friend class IssueItem;

	QIcon mWarningIco, mErrorIco;


	QTextCharFormat mErrorFormat;
	QTextCharFormat mWarningFormat;
	public:
		IssueList(QWidget* parent = 0);

		void setErrorFormat(const QTextCharFormat&);
		void setWarningFormat(const QTextCharFormat&);

		void addError(const QString&, const QString&, const QString&, int, int);
		void addWarning(const QString&, const QString&, const QString&, int, int);
		~IssueList();
};



#endif // ISSUELIST_HPP
