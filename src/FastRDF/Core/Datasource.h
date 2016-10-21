#pragma once

#include "ConstantSizeLiteralNode.h"

namespace FastRDF
{
	struct Datasource
	{
		uint32 DatasourceId;
		ConstantSizeLiteralNode::GetLiteralDataFunction GetDataCallback;
		NodeId DatasourceNode;
		uint32 ByteSize;
		uint64 Timestamp;
		Guid DatasourceGuid;
		
		Dataset* Dataset;

		void update() const;
	};
}