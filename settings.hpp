#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <QString>
#include <QSettings>

namespace Settings {
	QVariant get(const QString&, const QVariant& defaultValue = QVariant());
	void set(const QString&, const QVariant&);
	QSettings& instance();
	void sync();

	const QString getDefaultCompilerPath();
	const QString getDefaultCompilerEnvironment();
}


#endif // SETTINGS_HPP
