#ifndef FROSTCODEMODELCONTEXT_H
#define FROSTCODEMODELCONTEXT_H
#include <QSharedPointer>
#include <QSet>
#include <QMap>

#include "symbol.hpp"
#include "frosttoken.hpp"

namespace Frost {
class CodeModelContext
{
	public:
		CodeModelContext();
		~CodeModelContext();


		QMap<QString, QSharedPointer<Symbol> > globalSymbols() const;
		void setGlobalSymbols(const QMap<QString, QSharedPointer<Symbol> > &globalSymbols);

		QSharedPointer<Symbol> findSymbol(const QString &symbolName) const;


		QHash<QString, Token::Type> keywords() const;
		void setKeywords(const QHash<QString, Token::Type> &keywords);
		Token::Type keyword(const QString &name);

private:
		QMap<QString, QSharedPointer<Symbol>> mGlobalSymbols;

		QHash<QString, Token::Type> mKeywords;
};

}

#endif // FROSTCODEMODELCONTEXT_H
