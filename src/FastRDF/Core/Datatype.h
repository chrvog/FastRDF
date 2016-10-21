#pragma once

#include <boost/function.hpp>
#include <Typedefs.h>
#include <Core/Uri.h>

namespace FastRDF
{
	struct Datatype
	{
		Uri DatatypeUri;
		Uri BinaryTypeUri;
		virtual bool isConstantSize() const = 0;
	};

	struct ConstantSizeDatatype : public Datatype
	{
		typedef std::function<String(const void*, uint32)> StringSerializerFunction;
		typedef std::function<bool(const String&, void*)> StringDeserializerFunction;

		uint32 ByteSize;
		uint32 MaxStringSize;

		StringSerializerFunction ToString;
		StringDeserializerFunction FromString;

		virtual bool isConstantSize() const
		{
			return true;
		}

		virtual ~ConstantSizeDatatype()
		{}
	};

	struct DynamicSizeDatatype : public Datatype
	{
		typedef std::function<String(const void*,uint32)> StringSerializerFunction;
		typedef std::function<bool(const String&, void*&, uint32& size)> StringDeserializerFunction;

		StringSerializerFunction ToString;
		StringDeserializerFunction FromString;

		virtual bool isConstantSize() const
		{
			return false;
		}

		virtual ~DynamicSizeDatatype()
		{}
	};

}
