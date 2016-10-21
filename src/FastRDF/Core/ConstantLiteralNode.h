#pragma once

#include "NodeId.h"
#include "LiteralNode.h"

namespace FastRDF
{
	class Dataset;

	class ConstantLiteralNode : public LiteralNode
	{
		friend class Dataset;
	public:
		typedef std::function<String(void*, uint32)> ToStringFunction;

		ConstantLiteralNode(const ConstantLiteralNode& other);
		virtual ~ConstantLiteralNode();

		uint32 getLiteralByteSize() const;

		NodeId getLiteralDatatypeNode() const;

		const void* getData() const;

		void update();


		virtual Guid getDataGuid() const;
		virtual Guid generateDataGuid();
		virtual void setDataGuid(Guid dataGuid);

		virtual String convertToString(TemplateBinding* binding = NULL) const;

	private:
		ConstantLiteralNode(Dataset* dataset, NodeId nodeId);
	};
}