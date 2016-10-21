#pragma once


#include "ConstantSizeLiteralNode.h"
#include "LiteralNode.h"
#include "Literal.h"

namespace FastRDF
{
	struct RelativeConstantSizeLiteral : public Literal
	{
		uint32 ByteSize;
		uint32 ByteOffset;
		NodeId Datatype;
		uint32 DatasourceId;
	};
}