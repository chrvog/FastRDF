#pragma once

#include "NodeId.h"
#include "LiteralNode.h"
#include "TemplateBinding.h"

namespace FastRDF
{
	class Dataset;

	class ConstantSizeLiteralNode : public LiteralNode
	{
		friend class Dataset;
	public:
		typedef std::function<void(void*,TemplateBinding*)> GetLiteralDataFunction;
		typedef std::function<String(void*)> ToStringFunction;

		
		virtual NodeId getLiteralDatatypeNode() const;

		uint32 getByteSize() const;

		void getData(void* data, TemplateBinding* binding);

		ConstantSizeLiteralNode(const ConstantSizeLiteralNode& other);
		virtual ~ConstantSizeLiteralNode();

		virtual void update();

		virtual Guid getDataGuid() const;
		virtual Guid generateDataGuid();
		virtual void setDataGuid(Guid dataGuid);

		virtual String convertToString(TemplateBinding* binding) const;

	private:
		ConstantSizeLiteralNode(NodeId nodeId, Dataset* dataset);
	};
}