#ifndef SYMBOL
#define SYMBOL

#include <QString>

class Symbol {
	public:
		enum Type {
			FunctionSymbol,
			VariableSymbol,
			GlobalVariableSymbol,
			TypeSymbol,
			StructSymbol
		};

		Symbol(Type type, const QString &name) : mType(type), mName(name) {}
		virtual ~Symbol() {}
		Type type() const { return mType; }
		QString name() const { return mName; }
		void setName(const QString &name) { mName = name; }

	protected:
		Type mType;
		QString mName;

};

#endif // SYMBOL

