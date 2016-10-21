#pragma once

#include "ConstantSizeLiteralNode.h"
#include "Literal.h"
#include "FunctionBinder.h"


namespace FastRDF
{
	struct ConstantSizeLiteral : public Literal
	{
		ConstantSizeLiteralNode::GetLiteralDataFunction GetDataCallback;

		uint32 ByteSize;
		NodeId Datatype;
	};
}