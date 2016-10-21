#pragma once

#include "NodeId.h"
#include "LiteralNode.h"

namespace FastRDF
{
	class Dataset;
	struct Datasource;

	class RelativeConstantSizeLiteralNode : public LiteralNode
	{
		friend class Dataset;
	public:
		typedef std::function<String(void*)> ToStringFunction;


		NodeId getLiteralDatatypeNode() const;

		uint32 getByteSize() const;
		uint32 getByteOffset() const;

		void getData(void* data);

		const Datasource& getDatasource() const;

		uint32 getDatasourceGUID() const;

		RelativeConstantSizeLiteralNode(const RelativeConstantSizeLiteralNode& other);
		virtual ~RelativeConstantSizeLiteralNode();

		void update();

		virtual Guid getDataGuid() const;
		virtual Guid generateDataGuid();
		virtual void setDataGuid(Guid dataGuid);

		virtual String convertToString(TemplateBinding* binding) const;
	private:
		RelativeConstantSizeLiteralNode(NodeId nodeId, Dataset* dataset);
	};
}