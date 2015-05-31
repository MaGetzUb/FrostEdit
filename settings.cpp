#include "settings.hpp"
#include <QVariant>
#include <QString>
#include <QDebug>




static QSettings gSettings(Settings::gSettingFile, QSettings::IniFormat);

namespace Settings {



	QSettings& instance() {
		return gSettings;
	}

	QVariant get(const QString& key, const QVariant& defaultValue) {
		return gSettings.value(key, defaultValue);
	}

	void Settings::set(const QString& key, const QVariant& variant) {
		gSettings.setValue(key, variant);
	}

	void Settings::sync(){
		gSettings.sync();
	}


	const QString getDefaultCompilerPath() {
		QString path = gSettings.value("DefaultCompiler/Path", "").toString();
		return path.right(1) == "/" ? path : path + "/";
	}


	const QString getDefaultCompilerEnvironment() {
		QStringList list = gSettings.value("DefaultCompiler/Environment", "").toStringList();
		qDebug() << list;
		return list.join(";");
	}

}
