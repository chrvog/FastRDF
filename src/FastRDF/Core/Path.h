#pragma once


namespace FastRDF
{
	class Path
	{
	public:
		explicit Path(bool relative);
		Path();
		Path(const String& pathString);
		Path(const ConstantString& pathString);


		void append(const String& path);
		void append(const ConstantString& path);
		void append(const Path& path);

		void popFront();
		void popBack();

		void clear();

		bool isRelative() const;

		const ConstantString& operator[](uint32 index) const;

		uint32 getLength() const;

		String toString() const;

	private:
		std::vector<ConstantString> mPathPieces;
		bool mRelative;

		void appendWithoutCheck(const String& path);
	};

}