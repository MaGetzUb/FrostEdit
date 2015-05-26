/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "highlightersettings.h"

//#include <coreplugin/icore.h>

#include <QSettings>
#include <QLatin1String>
#include <QLatin1Char>
#include <QDir>
#include <QFile>
#include <QStringList>
#ifdef Q_OS_UNIX
#include <QProcess>
#endif

namespace TextEditor {
	namespace Internal {

		QString findFallbackDefinitionsLocation() {
			QDir dir;
			dir.setNameFilters(QStringList(QLatin1String("*.xml")));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
			static const QLatin1String kateSyntax[] = {
				QLatin1String("/share/apps/katepart/syntax"),
				QLatin1String("/share/kde4/apps/katepart/syntax")
			};
			static const int kateSyntaxCount =
					sizeof(kateSyntax) / sizeof(kateSyntax[0]);

			// Some wild guesses.
			for (int i = 0; i < kateSyntaxCount; ++i) {
				QStringList paths;
				paths << QLatin1String("/usr") + kateSyntax[i]
					 << QLatin1String("/usr/local") + kateSyntax[i]
					    << QLatin1String("/opt") + kateSyntax[i];
				foreach (const QString &path, paths) {
					dir.setPath(path);
					if (dir.exists() && !dir.entryInfoList().isEmpty())
						return dir.path();
				}
			}

			// Try kde-config.
			QStringList programs;
			programs << QLatin1String("kde-config") << QLatin1String("kde4-config");
			foreach (const QString &program, programs) {
				QProcess process;
				process.start(program, QStringList(QLatin1String("--prefix")));
				if (process.waitForStarted(5000)) {
					process.waitForFinished(5000);
					QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
					output.remove(QLatin1Char('\n'));
					for (int i = 0; i < kateSyntaxCount; ++i) {
						dir.setPath(output + kateSyntax[i]);
						if (dir.exists() && !dir.entryInfoList().isEmpty())
							return dir.path();
					}
				}
			}
#endif

			//dir.setPath(Core::ICore::instance()->resourcePath() + QLatin1String("/generic-highlighter"));

			dir.setPath("C:/Qt/Tools/QtCreator/share/qtcreator/generic-highlighter");
			if (dir.exists() && !dir.entryInfoList().isEmpty())
				return dir.path();

			return QString();
		}

	} // namespace Internal
} // namespace TextEditor

namespace {

	static const QLatin1String kDefinitionFilesPath("UserDefinitionFilesPath");
	static const QLatin1String kFallbackDefinitionFilesPath("FallbackDefinitionFilesPath");
	static const QLatin1String kAlertWhenDefinitionIsNotFound("AlertWhenDefinitionsIsNotFound");
	static const QLatin1String kUseFallbackLocation("UseFallbackLocation");
	static const QLatin1String kIgnoredFilesPatterns("IgnoredFilesPatterns");
	static const QLatin1String kGroupPostfix("HighlighterSettings");

	QString groupSpecifier(const QString &postFix, const QString &category) {
		if (category.isEmpty())
			return postFix;
		return QString(category + postFix);
	}

} // namespace anonymous

using namespace TextEditor;
using namespace Internal;

HighlighterSettings::HighlighterSettings() :
	mAlertWhenNoDefinition(true), mUseFallbackLocation(true)
{}

void HighlighterSettings::toSettings(const QString &category, QSettings *s) const {
	const QString &group = groupSpecifier(kGroupPostfix, category);
	s->beginGroup(group);
	s->setValue(kDefinitionFilesPath, mDefinitionFilesPath);
	s->setValue(kFallbackDefinitionFilesPath, mFallbackDefinitionFilesPath);
	s->setValue(kAlertWhenDefinitionIsNotFound, mAlertWhenNoDefinition);
	s->setValue(kUseFallbackLocation, mUseFallbackLocation);
	s->setValue(kIgnoredFilesPatterns, ignoredFilesPatterns());
	s->endGroup();
}

void HighlighterSettings::fromSettings(const QString &category, QSettings *s) {
	const QString &group = groupSpecifier(kGroupPostfix, category);
	s->beginGroup(group);
	mDefinitionFilesPath = s->value(kDefinitionFilesPath, QString()).toString();
	if (!s->contains(kDefinitionFilesPath))
		assignDefaultDefinitionsPath();
	else
		mDefinitionFilesPath = s->value(kDefinitionFilesPath).toString();
	if (!s->contains(kFallbackDefinitionFilesPath)) {
		mFallbackDefinitionFilesPath = findFallbackDefinitionsLocation();
		if (mFallbackDefinitionFilesPath.isEmpty())
			mUseFallbackLocation = false;
		else
			mUseFallbackLocation = true;
	} else {
		mFallbackDefinitionFilesPath = s->value(kFallbackDefinitionFilesPath).toString();
		mUseFallbackLocation = s->value(kUseFallbackLocation, true).toBool();
	}
	mAlertWhenNoDefinition = s->value(kAlertWhenDefinitionIsNotFound, true).toBool();
	if (!s->contains(kIgnoredFilesPatterns))
		assignDefaultIgnoredPatterns();
	else
		setIgnoredFilesPatterns(s->value(kIgnoredFilesPatterns, QString()).toString());
	s->endGroup();
}

void HighlighterSettings::setIgnoredFilesPatterns(const QString &patterns) {
	setExpressionsFromList(patterns.split(QLatin1Char(','), QString::SkipEmptyParts));
}

QString HighlighterSettings::ignoredFilesPatterns() const {
	return listFromExpressions().join(QLatin1String(","));
}

void HighlighterSettings::assignDefaultIgnoredPatterns() {
	QStringList patterns;
	patterns << QLatin1String("*.txt")
		 << QLatin1String("LICENSE*")
		 << QLatin1String("README")
		 << QLatin1String("INSTALL")
		 << QLatin1String("COPYING")
		 << QLatin1String("NEWS")
		 << QLatin1String("qmldir");
	setExpressionsFromList(patterns);
}

void HighlighterSettings::assignDefaultDefinitionsPath() {
	const QString &path = QLatin1String("C:/Qt/Tools/QtCreator/share/qtcreator/generic-highlighter");
	//Core::ICore::instance()->userResourcePath() + QLatin1String("/generic-highlighter");
	if (QFile::exists(path) || QDir().mkpath(path))
		mDefinitionFilesPath = path;
}

bool HighlighterSettings::isIgnoredFilePattern(const QString &fileName) const {
	foreach (const QRegExp &regExp, mIgnoredFiles)
		if (regExp.indexIn(fileName) != -1)
			return true;

	return false;
}

bool HighlighterSettings::equals(const HighlighterSettings &highlighterSettings) const {
	return mDefinitionFilesPath == highlighterSettings.mDefinitionFilesPath &&
			mFallbackDefinitionFilesPath == highlighterSettings.mFallbackDefinitionFilesPath &&
			mAlertWhenNoDefinition == highlighterSettings.mAlertWhenNoDefinition &&
			mUseFallbackLocation == highlighterSettings.mUseFallbackLocation &&
			mIgnoredFiles == highlighterSettings.mIgnoredFiles;
}

void HighlighterSettings::setExpressionsFromList(const QStringList &patterns) {
	mIgnoredFiles.clear();
	QRegExp regExp;
	regExp.setCaseSensitivity(Qt::CaseInsensitive);
	regExp.setPatternSyntax(QRegExp::Wildcard);
	foreach (const QString &s, patterns) {
		regExp.setPattern(s);
		mIgnoredFiles.append(regExp);
	}
}

QStringList HighlighterSettings::listFromExpressions() const {
	QStringList patterns;
	foreach (const QRegExp &regExp, mIgnoredFiles)
		patterns.append(regExp.pattern());
	return patterns;
}
