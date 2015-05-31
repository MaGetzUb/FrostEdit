#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <QString>
#include <QSettings>
#include <QFile>

namespace Settings {
	static QString gSettingFile  = "FrostEdit.ini";

	QVariant get(const QString&, const QVariant& defaultValue = QVariant());
	void set(const QString&, const QVariant&);
	QSettings& instance();
	void sync();
	static inline bool firstTime() {return !QFile::exists(gSettingFile);}
	const QString getDefaultCompilerPath();
	const QString getDefaultCompilerEnvironment();
}


#endif // SETTINGS_HPP
