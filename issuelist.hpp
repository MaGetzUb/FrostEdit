#ifndef ISSUELIST_HPP
#define ISSUELIST_HPP

#include <QListWidget>

class IssueList : public QListWidget {
	Q_OBJECT


		QIcon mErrorIcon;
		QIcon mWarningIcon;
	public:
		IssueList(QWidget* parent = 0);

		void addError(const QString&, const QString&, int, int);
		void addWarning(const QString&, const QString&, int, int);
		~IssueList();
};



#endif // ISSUELIST_HPP
