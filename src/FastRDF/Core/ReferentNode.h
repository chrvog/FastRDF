#pragma once

#include "NodeId.h"

namespace FastRDF
{
	class Dataset;
	class BlankNode;

	class ReferentNode 
	{
		friend class Dataset;
	public:

		Dataset* getDataset() const
		{
			return mDataset;
		}


		NodeId getNodeId() const
		{
			return mNodeId;
		}

		ReferentNode(const ReferentNode& other);

		ReferentNode();
		virtual ~ReferentNode();


		bool isBlankNode();

		bool isUriNode();

		NodeId::NodeType getNodeType();

		bool isInvalid() const;

		bool operator==(const ReferentNode& other);

	protected:
		ReferentNode(Dataset* dataset, NodeId nodeId);

		ReferentNode& operator=(const ReferentNode& other);

		NodeId mNodeId;
		Dataset* mDataset;

	};
}