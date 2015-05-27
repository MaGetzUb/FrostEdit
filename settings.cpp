#include "settings.hpp"
#include <QVariant>
#include <QString>
#include <QDebug>

static QSettings gSettings("FrostEdit.ini", QSettings::IniFormat);

QSettings& Settings::instance() {
	return gSettings;
}

QVariant Settings::get(const QString& key, const QVariant& defaultValue) {
	return gSettings.value(key, defaultValue);
}

void Settings::set(const QString& key, const QVariant& variant) {
	gSettings.setValue(key, variant);
}

void Settings::sync(){
	gSettings.sync();
}


const QString Settings::getDefaultCompilerPath() {
	QString path = gSettings.value("DefaultCompiler/Path", "").toString();
	return path.right(1) == "/" ? path : path + "/";
}


const QString Settings::getDefaultCompilerEnvironment() {
	QStringList list = gSettings.value("DefaultCompiler/Environment", "").toStringList();
	qDebug() << list;
	return list.join(";");
}
