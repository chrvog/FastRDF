#pragma once

#include "DynamicLiteralNode.h"
#include "Literal.h"

namespace FastRDF
{
	struct DynamicLiteral : public Literal
	{
		DynamicLiteralNode::GetLiteralByteSizeFunction GetByteSizeCallback;
		DynamicLiteralNode::GetLiteralDataFunction GetDataCallback;

		void* GetSizeOwner;
		void* GetDataOwner;

		bool GetSizeUnbound;
		bool GetDataUnbound;

		uint32 ByteSize;
		NodeId Datatype;
	};
}