#include <stdafx.h>
#include "DefaultStringSerializer.h"

#pragma warning(push)
#pragma warning(disable : 4459)
#pragma warning(disable : 4456)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)

#include <sstream>
#include <iomanip>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/format.hpp>


using namespace std;
using namespace FastRDF;




String FastRDF::booleanToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[8];
	char *p = buffer;;
	generate(p, boost::spirit::karma::bool_, *((bool*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::int08ToString(const void* value, uint32 size)
{
	int16 tmp = *((int08*)value);
	return int16ToString(&tmp,sizeof(int16));
}

String FastRDF::uint08ToString(const void* value, uint32 size)
{
	uint16 tmp = *((uint08*)value);
	return uint16ToString(&tmp, sizeof(uint16));
}

String FastRDF::int16ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[16];
	char *p = buffer;;
	generate(p, boost::spirit::karma::short_, *((int16*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::uint16ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[16];
	char *p = buffer;;
	generate(p, boost::spirit::karma::ushort_, *((uint16*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::int32ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[32];
	char *p = buffer;;
	generate(p, boost::spirit::karma::int_, *((int32*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::uint32ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[32];
	char *p = buffer;;
	generate(p, boost::spirit::karma::uint_, *((uint32*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::int64ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[32];
	char *p = buffer;;
	generate(p, boost::spirit::karma::long_long, *((int64*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::uint64ToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[32];
	char *p = buffer;;
	generate(p, boost::spirit::karma::ulong_long, *((uint64*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::floatToString(const void* value, uint32 size)
{
	using boost::spirit::karma::generate;
	char buffer[64];
	char *p = buffer;;
	generate(p, boost::spirit::karma::float_, *((float*)value));
	*p = '\0';
	return buffer;
}

String FastRDF::doubleToString(const void* value, uint32 size)
{
	ostringstream stream;
	stream << fixed << setprecision(8) << *((double*)value);
	return stream.str();
}

String FastRDF::stringDataToString(const void* value, uint32 size)
{
	const char* stringValue = (const char*)value;
	
	// warning, fix, todo, fix this!!!!
	char* tmp = new char[size + 1];
	memcpy(tmp, stringValue, size);
	tmp[size] = '\0';
	String returnValue(tmp);
	delete[] tmp;

	return returnValue;
}


bool FastRDF::stringToBoolean(const String & value,void* result)
{
	if (value == "0")
		*((bool*)result) = false;
	else if (value == "1")
		*((bool*)result) = true;
	else {
		std::string::const_iterator iter = value.begin();
		if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::bool_, *((bool*)result)))
			return false;
	}
	return true;
}

bool FastRDF::stringToInt08(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	int16 tmp;
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::short_, tmp))
		return false;
	*((int08*)result) = (int08)tmp;
	return true;
}

bool FastRDF::stringToUInt08(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	uint16 tmp;
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::ushort_, tmp))
		return false;
	*((uint08*)result) = (uint08)tmp;
	return true;
}

bool FastRDF::stringToInt16(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::short_, *((int16*)result)))
		return false;
	return true;
}

bool FastRDF::stringToUInt16(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::ushort_, *((uint16*)result)))
		return false;
	return true;
}

bool FastRDF::stringToInt32(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::int_, *((int32*)result)))
		return false;
	return true;
}

bool FastRDF::stringToUInt32(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::uint_, *((uint32*)result)))
		return false;
	return true;
}

bool FastRDF::stringToInt64(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::long_long, *((int64*)result)))
		return false;
	return true;
}

bool FastRDF::stringToUInt64(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::ulong_long, *((uint64*)result)))
		return false;
	return true;
}

bool FastRDF::stringToFloat(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::float_, *((float*)result)))
		return false;
	return true;
}

bool FastRDF::stringToDouble(const String & value, void * result)
{
	std::string::const_iterator iter = value.begin();
	if (!boost::spirit::qi::parse(value.begin(), value.end(), boost::spirit::qi::double_, *((float64*)result)))
		return false;
	return true;
}

bool FastRDF::stringToNullTerminatedStringData(const String& value, void*& result, uint32& byteSize)
{
	if (result == NULL)
		result = new uint08[value.length()+1];
	memcpy(result, &(value[0]), value.length());
	((uint08*)result)[value.length()] = '\0';
	byteSize = value.length();
	return true;
}

#pragma warning(pop)

