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

#include "highlightdefinitionmetadata.h"

using namespace TextEditor;
using namespace Internal;

const QLatin1String HighlightDefinitionMetaData::kPriority("priority");
const QLatin1String HighlightDefinitionMetaData::kName("name");
const QLatin1String HighlightDefinitionMetaData::kExtensions("extensions");
const QLatin1String HighlightDefinitionMetaData::kMimeType("mimetype");
const QLatin1String HighlightDefinitionMetaData::kVersion("version");
const QLatin1String HighlightDefinitionMetaData::kUrl("url");

HighlightDefinitionMetaData::HighlightDefinitionMetaData() : mPriority(0) {}

void HighlightDefinitionMetaData::setPriority(const int priority) {
	mPriority = priority;
}

int HighlightDefinitionMetaData::priority() const {
	return mPriority;
}

void HighlightDefinitionMetaData::setId(const QString &id) {
	mId = id;
}

const QString &HighlightDefinitionMetaData::id() const {
	return mId;
}

void HighlightDefinitionMetaData::setName(const QString &name) {
	mName = name;
}

const QString &HighlightDefinitionMetaData::name() const {
	return mName;
}

void HighlightDefinitionMetaData::setVersion(const QString &version) {
	mVersion = version;
}

const QString &HighlightDefinitionMetaData::version() const {
	return mVersion;
}

void HighlightDefinitionMetaData::setFileName(const QString &fileName) {
	mFileName = fileName;
}

const QString &HighlightDefinitionMetaData::fileName() const {
	return mFileName;
}

void HighlightDefinitionMetaData::setPatterns(const QStringList &patterns) {
	mPatterns = patterns;
}

const QStringList &HighlightDefinitionMetaData::patterns() const {
	return mPatterns;
}

void HighlightDefinitionMetaData::setMimeTypes(const QStringList &mimeTypes) {
	mMimeTypes = mimeTypes;
}

const QStringList &HighlightDefinitionMetaData::mimeTypes() const {
	return mMimeTypes;
}

void HighlightDefinitionMetaData::setUrl(const QUrl &url) {
	mUrl = url;
}

const QUrl &HighlightDefinitionMetaData::url() const {
	return mUrl;
}
