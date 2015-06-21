#ifndef APPEARANCETAB_HPP
#define APPEARANCETAB_HPP

#include <QWidget>
#include <QTextCharFormat>
#include <QListWidgetItem>

namespace Ui {
	class AppearanceTab;
}

class FormattedListItem: public QListWidgetItem {
		QTextCharFormat mFormat;
	public:
		FormattedListItem(const QListWidgetItem& b): QListWidgetItem(b) {}
		FormattedListItem(QListWidget* view = 0, int type = Type): QListWidgetItem(view, type) {}
		FormattedListItem(const QString& txt, QListWidget* view = 0, int type = Type): QListWidgetItem(txt, view, type) {}
		FormattedListItem(const QIcon& icon, const QString& txt, QListWidget* parent = 0): QListWidgetItem(icon, txt, parent) {}
		~FormattedListItem() {}
		void setFormat(const QTextCharFormat& fmt);
		const QTextCharFormat& getFormat() const;
};

class Colorscheme;
class AppearanceTab : public QWidget
{
		friend class SettingsMenu;
		Q_OBJECT
		Colorscheme* mColorScheme;

		FormattedListItem* mBasicText;
		FormattedListItem* mKeyword;
		FormattedListItem* mDataType;
		FormattedListItem* mString;
		FormattedListItem* mComment;
		FormattedListItem* mFunction;
		FormattedListItem* mOthers;
		FormattedListItem* mOperator;
		FormattedListItem* mNumber;
		FormattedListItem* mSelection;
		FormattedListItem* mLocal;
		FormattedListItem* mGlobal;
		FormattedListItem* mField;
		FormattedListItem* mUserDefinedFunction;
		FormattedListItem* mUserDefinedStructure;
		FormattedListItem* mLabel;
		//TextEdit colors:
		FormattedListItem* mLineNumber;
		FormattedListItem* mSelectedLineNumber;
		FormattedListItem* mSimilarOccurance;
		FormattedListItem* mRegionVisualizer;
		FormattedListItem* mRegionVisualizerSelected;
		FormattedListItem* mParenthesesMatch;
		//Console colors
		FormattedListItem* mStdErr;
		FormattedListItem* mStdOut;

		FormattedListItem* mIssueError;
		FormattedListItem* mIssueWarning;

		FormattedListItem* mCurrentItem;

		QFont mFont;
	public:
		explicit AppearanceTab(QWidget *parent = 0);
		~AppearanceTab();

		void setSyntaxStyle(Colorscheme* style);
		void setTextEditFont(const QFont& font);

	private slots:

		void on_fontFormatItalic_toggled(bool checked);

		void on_fontFormatBold_toggled(bool checked);

		void on_textForegroundColor_clicked();

		void on_textBackgroundColor_clicked();

		void selectionChanged(QListWidgetItem*);

		void on_textFont_clicked();

		void on_dontUseBGButton_clicked();

		void on_dontUseFGButton_clicked();


		void on_toolButton_clicked();

		void on_selectStyleSheetButton_clicked();


		void on_resetColorSchemeButton_clicked();

		void on_openStyleSheet_clicked();


		void on_saveStyleSheet_clicked();

		void on_saveStyleSheetAs_clicked();

	private:
		Ui::AppearanceTab *ui;

		void applyCharFormatToItem(QListWidgetItem* item, const QTextCharFormat& fmt);

		void updateSchemePreciseThings();

		void useSyntaxStyle();

		void updateSyntaxStyle();

		FormattedListItem* toFormattedListItem(QListWidgetItem* item);
};

#endif // APPEARANCETAB_HPP
