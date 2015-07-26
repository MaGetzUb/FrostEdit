#ifndef COMBOBOX_HPP
#define COMBOBOX_HPP

#include <QComboBox>

class ComboBox : public QComboBox {
		Q_OBJECT

	public:
		explicit ComboBox(QWidget *parent = 0);
		~ComboBox();

	protected:

		void paintEvent(QPaintEvent *e) override;

};

#endif // COMBOBOX_HPP
