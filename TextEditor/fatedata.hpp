#ifndef FATEDATA
#define FATEDATA
#include <QString>
#include <QList>

namespace Fate {

struct Identifier {
	QString mName;
	int mRow, mColumn;
	void operator=(const Identifier& i) {
		mName = i.mName;
		mRow = i.mRow;
		mColumn = i.mColumn;
	}

	bool operator==(const Identifier& b) const {
		return mName == b.mName && mRow == b.mRow && mColumn == b.mColumn;
	}
};
struct IdentStructure;
struct IdentVariable : Identifier {
	QString mType;
	int mRegion;
	IdentStructure* mParentStructure;
	void operator=(const IdentVariable& v) {
		Identifier::operator =(v);
		mRegion = v.mRegion;
		mType = v.mType;
		mParentStructure = v.mParentStructure;
	}

	bool operator==(const IdentVariable& b) const {
		return Identifier::operator ==(b) && mType == b.mType && mRegion == b.mRegion && mParentStructure == b.mParentStructure;
	}

};

struct IdentStructure : Identifier {
	QList<IdentVariable> mFields;
	void operator=(const IdentStructure& s) {
		Identifier::operator =(s);
		mFields = s.mFields;
	}
};

struct IdentFunction : Identifier {
	QString mReturnType;
	QList<IdentVariable> mParameters;

	void operator=(const IdentFunction& f)  {
		Identifier::operator =(f);
		mReturnType = f.mReturnType;
		mParameters = f.mParameters;
	}

};

}


#endif // FATEDATA

