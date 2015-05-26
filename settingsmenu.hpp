#ifndef SETTINGSMENU_HPP
#define SETTINGSMENU_HPP

#include <QDialog>

namespace Ui {
	class SettingsMenu;
}

class SettingsMenu : public QDialog
{
		Q_OBJECT

	public:
		explicit SettingsMenu(QWidget *parent = 0);
		~SettingsMenu();

	private:
		Ui::SettingsMenu *ui;
};

#endif // SETTINGSMENU_HPP
