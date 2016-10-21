#include "stdafx.h"
#include "Path.h"


using namespace std;
using namespace FastRDF;

Path::Path(bool relative)
: mRelative(relative)
{}

FastRDF::Path::Path()
	: mRelative(true)
{
}

Path::Path(const String& pathString)
{
	mRelative = pathString.size() > 0 && pathString[0] != '/';

	appendWithoutCheck(pathString);
}

Path::Path(const ConstantString& pathString)
: Path(pathString.get())
{
}


void Path::clear()
{
	mPathPieces.clear();
}

bool Path::isRelative() const
{
	return mRelative;
}


void Path::popFront()
{
	assert(!mPathPieces.empty());
	mPathPieces.front() = std::move(mPathPieces.back());
	mPathPieces.pop_back();
	mRelative = true;
}

void Path::popBack()
{
	assert(!mPathPieces.empty());
	mPathPieces.pop_back();
}

void Path::append(const String& pathString)
{
	assert(!pathString.empty());
	assert(pathString[0] != '/');

	appendWithoutCheck(pathString);
}

void Path::append(const ConstantString& pathString)
{
	append(pathString.get());
}

void Path::appendWithoutCheck(const String& pathString)
{
	if (pathString.empty())
		return;
	// reserve path size
	uint32 appendSize = 0;
	if (pathString.empty())
		appendSize = 0;

	appendSize = std::count(pathString.begin(), pathString.end(), '/') + 1;

	uint32 startIndex = 0;
	uint32 endIndex = pathString.length() - 1;

	if (appendSize > 0 && pathString[0] == '/')
	{
		appendSize = appendSize - 1;
		startIndex = 1;
	}
	if (appendSize > 0 && pathString[pathString.length() - 1] == '/')
	{
		appendSize = appendSize - 1;
		endIndex = endIndex - 1;
	}

	if (appendSize == 0)
		return;

	mPathPieces.reserve(getLength() + appendSize);


	for (uint32 iter = 1; iter <= endIndex; ++iter)
	{
		if (pathString[iter] == '/')
		{
			mPathPieces.push_back(ConstantString(pathString.substr(startIndex, iter - startIndex)));
			startIndex = iter + 1;
		}
	}
	mPathPieces.push_back(ConstantString(pathString.substr(startIndex, endIndex - startIndex + 1)));
}

void Path::append(const Path& path)
{
	assert(path.isRelative());

	mPathPieces.reserve(mPathPieces.size() + path.mPathPieces.size());
	mPathPieces.insert(mPathPieces.end(), path.mPathPieces.begin(), path.mPathPieces.end());
}

uint32 Path::getLength() const
{
	return mPathPieces.size();
}

String FastRDF::Path::toString() const
{

	uint32 size = mRelative ? 0 : 1;

	for (ConstantString segment : mPathPieces)
		size += segment.get().length() + 1;

	String returnValue;
	returnValue.reserve(size);

	if (!mRelative)
		returnValue += "/";

	for (ConstantString segment : mPathPieces)
	{
		returnValue += segment.get();
		returnValue += "/";
	}
	return returnValue;
}


