#pragma once


#include "ReferentNode.h"

namespace FastRDF
{
	class BlankNode : public ReferentNode
	{
		friend class Dataset;
	public:

		BlankNode(const BlankNode& other)
			: ReferentNode(other)
		{}

		BlankNode()
			: ReferentNode()
		{}

		virtual ~BlankNode()
		{}

	protected:
		BlankNode(Dataset* dataset, NodeId nodeId)
			: ReferentNode(dataset,nodeId)
		{}

	};
}