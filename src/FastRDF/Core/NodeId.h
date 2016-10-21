#pragma once

#include <Typedefs.h>

namespace FastRDF
{
	struct NodeId
	{
		enum NodeType : uint32
		{
			NODE_URI = 0,
			NODE_BLANK	 = 1,
			NODE_CONSTANT_LITERAL = 2,
			NODE_CONSTANT_SIZE_LITERAL = 3,
			NODE_RELATIVE_CONSTANT_SIZE_LITERAL = 4,
			NODE_DYNAMIC_LITERAL = 5,
			NODE_URI_PLACEHOLDER = 6,
			NODE_INVALID = 7
		};

		NodeId()
			: Type(NODE_INVALID)
			, Id(-1)
		{}

		NodeId(NodeType type , uint32 id)
		: Type(type)
		, Id(id)
		{}

		uint32 Type : 3;
		uint32 Id : 29;

		bool operator<(const NodeId& other) const
		{
			return *((uint32*)this) < *((uint32*)&other);
		}

		bool operator==(const NodeId& other) const
		{
			return *((uint32*)this) == *((uint32*)&other);
		}

		bool operator>(const NodeId& other) const
		{
			return *((uint32*)this) > *((uint32*)&other);
		}

		explicit operator uint32()
		{
			return *((uint32*)this);
		}

		bool isInvalid() const
		{
			return (Type == NODE_INVALID || Id == -1);
		}

	};
}