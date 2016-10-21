#pragma once

#include "NodeId.h"
#include "TemplateBinding.h"

namespace FastRDF
{
	class Graph;
	class Dataset;
	struct Datatype;

	class LiteralNode
	{
		friend class Dataset;
	public:
		NodeId getNodeId() const;

		Dataset* getDataset() const;

		LiteralNode(const LiteralNode& other);
		virtual ~LiteralNode();

		virtual void update() = 0;

		virtual Guid getDataGuid() const = 0;
		virtual Guid generateDataGuid() = 0;
		virtual void setDataGuid(Guid dataGuid) = 0;
		virtual NodeId getLiteralDatatypeNode() const = 0;

		const Datatype* getLiteralDatatype() const;

		virtual String convertToString(TemplateBinding* binding) const = 0;


	protected:
		LiteralNode(NodeId nodeId, Dataset* dataset);

		LiteralNode& operator=(const LiteralNode& other);

		Dataset* mDataset;
		NodeId mNodeId;
	};
}