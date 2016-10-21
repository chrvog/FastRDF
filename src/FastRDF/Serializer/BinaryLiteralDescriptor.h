#pragma once

#include "../Core/NodeId.h"

namespace FastRDF
{
	struct BinaryLiteralDescriptor
	{

		BinaryLiteralDescriptor()
			: Guid(-1)
		{}

		BinaryLiteralDescriptor(NodeId nodeId, Guid guid, uint32 offset, bool constantLiteral, NodeId datatype)
			: LiteralNodeId(nodeId), Guid(guid), DatasourceOffset(offset), Datatype(datatype), ConstantLiteral(constantLiteral)
		{}

		
		NodeId LiteralNodeId;
		uint32 Guid;
		uint32 DatasourceOffset;
		NodeId Datatype;
		bool ConstantLiteral;
	};

	typedef std::vector<BinaryLiteralDescriptor> BinaryLiteralDescriptorList;
}