#pragma once


namespace FastRDF
{
	String booleanToString(const void* value, uint32 size);

	String int08ToString(const void* value, uint32 size);
	String uint08ToString(const void* value, uint32 size);

	String int16ToString(const void* value, uint32 size);
	String uint16ToString(const void* value, uint32 size);

	String int32ToString(const void* value, uint32 size);
	String uint32ToString(const void* value, uint32 size);

	String int64ToString(const void* value, uint32 size);
	String uint64ToString(const void* value, uint32 size);

	String floatToString(const void* value, uint32 size);
	String doubleToString(const void* value, uint32 size);

	String stringDataToString(const void* value, uint32 size);

	bool stringToBoolean(const String& value, void* result);

	bool stringToInt08(const String& value, void* result);
	bool stringToUInt08(const String& value, void* result);
	
	bool stringToInt16(const String& value, void* result);
	bool stringToUInt16(const String& value, void* result);
	
	bool stringToInt32(const String& value, void* result);
	bool stringToUInt32(const String& value, void* result);
	
	bool stringToInt64(const String& value, void* result);
	bool stringToUInt64(const String& value, void* result);
	
	bool stringToFloat(const String& value, void* result);
	bool stringToDouble(const String& value, void* result);

	bool stringToNullTerminatedStringData(const String& value, void*& result, uint32& byteSize);

}
