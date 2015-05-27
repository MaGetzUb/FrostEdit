#ifndef CODEPOINT
#define CODEPOINT

class Document;

class CodePoint {
	public:
		CodePoint() : mColumn(0), mLine(0), mDocument(0) {}
		CodePoint(int column, int line, Document *doc) : mColumn(column), mLine(line), mDocument(doc) {}
		~CodePoint() {}

		int column() const { return mColumn; }
		int line() const { return mLine; }
		Document *document() const { return mDocument; }

	private:
		int mColumn;
		int mLine;
		Document *mDocument;
};

#endif // CODEPOINT

