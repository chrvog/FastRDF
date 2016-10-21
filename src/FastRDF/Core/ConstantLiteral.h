#pragma once

#include "Literal.h"

namespace FastRDF
{
	struct ConstantLiteral : public Literal
	{
		void* Data;
		uint32 ByteSize;
		NodeId Datatype;
	};
}