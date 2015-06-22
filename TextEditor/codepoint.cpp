#include "codepoint.hpp"
#include "document.hpp"
QString CodePoint::toString() const {
	 return QStringLiteral("%1 [%2, %3]").arg(mLineStartPoint->mDocument->getFileName(), QString::number(mLineStartPoint->mLineNumber), QString::number(mColumn));
}
QLinkedList<LineStartPoint>::Iterator CodePoint::lineStartPoint() const
{
    return mLineStartPoint;
}

