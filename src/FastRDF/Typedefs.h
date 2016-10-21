#pragma once

#include <boost/flyweight.hpp>
#include <boost/signals2.hpp>
#include <limits>

namespace FastRDF
{
	typedef boost::flyweight<std::string> ConstantString;
	typedef std::string String;

	typedef char int08;
	typedef unsigned char uint08;

	typedef short int16;
	typedef unsigned short uint16;

	typedef int int32;
	typedef unsigned int uint32;

	typedef long long int64;
	typedef unsigned long long uint64;

	typedef double float64;

	typedef uint32 Guid;
	typedef boost::signals2::signal<void()> LiteralChangedSignalType;
}

#define MAX_INT08 std::numeric_limits<int08>::max()
#define MIN_INT08 std::numeric_limits<int08>::min()
#define MAX_UINT08 std::numeric_limits<uint08>::max()
#define MIN_UINT08 std::numeric_limits<uint08>::min()
#define MAX_INT16 std::numeric_limits<int16>::max()
#define MIN_INT16 std::numeric_limits<int16>::min()
#define MAX_UINT16 std::numeric_limits<uint16>::max()
#define MIN_UINT16 std::numeric_limits<uint16>::min()
#define MAX_INT32 std::numeric_limits<int32>::max()
#define MIN_INT32 std::numeric_limits<int32>::min()
#define MAX_UINT32 std::numeric_limits<uint32>::max()
#define MIN_UINT32 std::numeric_limits<uint32>::min()

