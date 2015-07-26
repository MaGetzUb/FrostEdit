#ifndef SETTINGSMENU_HPP
#define SETTINGSMENU_HPP

#include <QDialog>
#include "appearancetab.hpp"

namespace Ui {
	class SettingsMenu;
}

class SettingsMenu : public QDialog {
		friend class Colorscheme;
		Q_OBJECT
		Ui::SettingsMenu* ui;

	public:
		explicit SettingsMenu(QWidget *parent = 0);
		~SettingsMenu();

		AppearanceTab* appearanceTab();

		void closeEvent(QCloseEvent *) override;
	signals:
		void settingsApplied();

	private slots:

		void on_okButton_clicked();

		void on_applyButton_clicked();

		void on_cancelButton_clicked();
};

#endif // SETTINGSMENU_HPP
