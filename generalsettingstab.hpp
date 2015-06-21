#ifndef GENERALSETTINGSTAB_HPP
#define GENERALSETTINGSTAB_HPP

#include <QWidget>

namespace Ui {
	class GeneralSettingsTab;
}

class GeneralSettingsTab : public QWidget
{
		Q_OBJECT

	public:
		explicit GeneralSettingsTab(QWidget *parent = 0);
		~GeneralSettingsTab();

	private slots:

		void on_showLineNumbers_toggled(bool checked);

	private:
		Ui::GeneralSettingsTab *ui;
};

#endif // GENERALSETTINGSTAB_HPP
