#pragma once

#include "NodeId.h"
#include "Datatype.h"
#include "LiteralNode.h"
#include "TemplateBinding.h"

namespace FastRDF
{
	class Dataset;

	class DynamicLiteralNode : public LiteralNode
	{
		friend class Dataset;
	public:
		typedef std::function<String(void*)> ToStringFunction;
		typedef std::function<size_t(TemplateBinding*)> GetLiteralByteSizeFunction;
		typedef std::function<void(void*,TemplateBinding*)> GetLiteralDataFunction;

		Dataset* getDataset() const
		{
			return mDataset;
		}


		NodeId getNodeId() const
		{
			return mNodeId;
		}

		const GetLiteralByteSizeFunction& getGetLiteralSizeFunction() const;
		void setGetLiteralSizeFunction(const GetLiteralByteSizeFunction& function);

		const GetLiteralDataFunction& getGetLiteralDataFunction() const;
		void setGetLiteralDataFunction(const GetLiteralDataFunction& function);


		uint32 getByteSize(TemplateBinding* binding) const;

		void getData(void* data,TemplateBinding* binding);

		DynamicLiteralNode(const DynamicLiteralNode& other);
		virtual ~DynamicLiteralNode();

		const DynamicSizeDatatype* getLiteralDatatype() const;
		NodeId getLiteralDatatypeNode() const;

		void update();

		virtual Guid getDataGuid() const;
		virtual Guid generateDataGuid();
		virtual void setDataGuid(Guid dataGuid);

		virtual String convertToString(TemplateBinding* binding) const;


		bool hasUnboundGetSizeFunction() const
		{
			return mUnboundGetSizeFunction;
		}

	private:
		bool mUnboundGetSizeFunction;
		DynamicLiteralNode(Dataset* dataset, NodeId nodeId);
	};
}