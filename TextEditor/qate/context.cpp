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

#include "context.h"
#include "rule.h"
#include "reuse.h"
#include "dynamicrule.h"
#include "highlightdefinition.h"

using namespace TextEditor;
using namespace Internal;

Context::Context() : mFallthrough(false), mDynamic(false)
{}

Context::Context(const Context &context) :
	mId(context.mId), mName(context.mName), mLineBeginContext(context.mLineBeginContext),
	mLineEndContext(context.mLineEndContext), mFallthroughContext(context.mFallthroughContext),
	mItemData(context.mItemData), mFallthrough(context.mFallthrough),
	mDynamic(context.mDynamic), mInstructions(context.mInstructions),
	mDefinition(context.mDefinition)
{
	// Rules need to be deeply copied because of dynamic contexts.
	foreach (const QSharedPointer<Rule> &rule, context.mRules)
		mRules.append(QSharedPointer<Rule>(rule->clone()));
}

const Context &Context::operator=(Context copy) {
	swap(copy);
	return *this;
}

Context::~Context() {}

void Context::swap(Context &context) {
	qSwap(mId, context.mId);
	qSwap(mName, context.mName);
	qSwap(mLineBeginContext, context.mLineBeginContext);
	qSwap(mLineEndContext, context.mLineEndContext);
	qSwap(mFallthroughContext, context.mFallthroughContext);
	qSwap(mItemData, context.mItemData);
	qSwap(mFallthrough, context.mFallthrough);
	qSwap(mDynamic, context.mDynamic);
	qSwap(mRules, context.mRules);
	qSwap(mInstructions, context.mInstructions);
	qSwap(mDefinition, context.mDefinition);
}

void Context::configureId(const int unique) {
	mId.append(QString::number(unique));
}

const QString &Context::id() const {
	return mId;
}

void Context::setName(const QString &name) {
	mName = name;
	mId = name;
}

const QString &Context::name() const {
	return mName;
}

void Context::setLineBeginContext(const QString &context) {
	mLineBeginContext = context;
}

const QString &Context::lineBeginContext() const {
	return mLineBeginContext;
}

void Context::setLineEndContext(const QString &context) {
	mLineEndContext = context;
}

const QString &Context::lineEndContext() const {
	return mLineEndContext;
}

void Context::setFallthroughContext(const QString &context) {
	mFallthroughContext = context;
}

const QString &Context::fallthroughContext() const {
	return mFallthroughContext;
}

void Context::setItemData(const QString &itemData) {
	mItemData = itemData;
}

const QString &Context::itemData() const {
	return mItemData;
}

void Context::setFallthrough(const QString &fallthrough) {
	mFallthrough = toBool(fallthrough);
}

bool Context::isFallthrough() const {
	return mFallthrough;
}

void Context::setDynamic(const QString &dynamic) {
	mDynamic = toBool(dynamic);
}

bool Context::isDynamic() const {
	return mDynamic;
}

void Context::updateDynamicRules(const QStringList &captures) const {
	TextEditor::Internal::updateDynamicRules(mRules, captures);
}

void Context::addRule(const QSharedPointer<Rule> &rule) {
	mRules.append(rule);
}

void Context::addRule(const QSharedPointer<Rule> &rule, int index) {
	mRules.insert(index, rule);
}

const QList<QSharedPointer<Rule> > & Context::rules() const {
	return mRules;
}

void Context::addIncludeRulesInstruction(const IncludeRulesInstruction &instruction) {
	mInstructions.append(instruction);
}

const QList<IncludeRulesInstruction> &Context::includeRulesInstructions() const {
	return mInstructions;
}

void Context::clearIncludeRulesInstructions() {
	mInstructions.clear();
}

void Context::setDefinition(const QSharedPointer<HighlightDefinition> &definition) {
	mDefinition = definition;
}

const QSharedPointer<HighlightDefinition> &Context::definition() const {
	return mDefinition;
}
