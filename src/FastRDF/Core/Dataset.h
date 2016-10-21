#pragma once

#include <set>

#include "UriNode.h"
#include "BlankNode.h"
#include "LiteralNode.h"
#include "ConstantLiteral.h"
#include "ConstantLiteralNode.h"
#include "ConstantSizeLiteral.h"
#include "RelativeConstantSizeLiteral.h"
#include "RelativeConstantSizeLiteralNode.h"
#include "UriPlaceHolderNode.h"
#include "DynamicLiteral.h"
#include "Datasource.h"
#include "XSDTypeHelper.h"

namespace FastRDF
{
	struct Datatype;
	class Dataset;
	class NativeClassGraphTemplate;

	
	class Dataset
	{
		friend class ReferentNode;
		friend class LiteralNode;
		friend class DynamicLiteralNode;
		friend class ConstantLiteralNode;
		friend class ConstantSizeLiteralNode;
		friend class RelativeConstantSizeLiteralNode;
		friend class UriNode;
		friend class Graph;

	public:
		boost::signals2::signal<void(Graph*)> OnGraphCreated;
		boost::signals2::signal<void(Graph*)> OnGraphDeleting;


		Dataset(uint32 timeStamp);

		void setTimestamp(uint64 timestamp)
		{
			mCurrentTimestamp = timestamp;
		}

		uint64 getCurrentTimestamp() const
		{
			return mCurrentTimestamp;
		}

		Graph* createGraph(const ReferentNode& nameNode);
		void deleteGraph(Graph* graph);

		void addNamespace(const String& name, const Uri& uri);
		const std::map<ConstantString, Uri>& getNamespaces() const;

		UriNode createUriNode(const String& uri);
		UriNode createUriNode(const String& nameSpace, const String& relativePath);
		UriNode createUriNode(const Uri& uri);

		UriNode getUriNode(NodeId nodeId) const;

		UriNode findUriNode(const String& uri);
		UriNode findUriNode(const String& nameSpace, const String& relativePath);
		UriNode findUriNode(const Uri& uri);

		ReferentNode getReferentNode(NodeId nodeId);

		BlankNode createBlankNode();
		BlankNode getBlankNode(NodeId blankNodeId);


		ConstantLiteralNode createConstantLiteralNode(void* data, uint32 size, const UriNode& type);
		ConstantLiteralNode getConstantLiteralNode(NodeId nodeId) const;

		template<typename LiteralType>
		ConstantLiteralNode createConstantLiteralNode(const LiteralType& data, UriNode type = UriNode())
		{
			if (type.isInvalid())
				type = getXSDType<LiteralType>(this);

			return createConstantLiteralNode((void*)&data, sizeof(data), type);
		}
		ConstantLiteralNode createConstantLiteralNode(const String& data, const UriNode& type);

		template <typename LiteralType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(std::function<LiteralType()> getFunction, UriNode type = UriNode())
		{
			if (type.isInvalid())
				type = getXSDType<LiteralType>(this);
			std::function<void (void*, TemplateBinding* binding)> getterBinding = [getFunction](void* value, TemplateBinding* binding)
			{  
				*((LiteralType*)value) = getFunction();
			};
			return createConstantSizeLiteralNode(sizeof(LiteralType), getterBinding, type);
		}

		template <typename OwnerType, typename LiteralType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(std::function<void(OwnerType*, void*)> getFunction, OwnerType* owner, UriNode type = UriNode())
		{
			if (type.isInvalid())
				type = getXSDType<LiteralType>(this);

			ConstantSizeLiteralNode::GetLiteralDataFunction getterBinding = [getFunction,owner](void* value, TemplateBinding* binding)
			{
				LiteralType tempValue = getFunction(owner);
				*((LiteralType*)value) = tempValue;
			};

			return createConstantSizeLiteralNode(sizeof(LiteralType), getterBinding, type);
		}

		template <typename OwnerType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, std::function<void(void*)> getFunction, const UriNode& type)
		{
			auto getterBinding = [getFunction](void* value, TemplateBinding* binding) {
				getFunction(value);
			};
			return createConstantSizeLiteralNode(size, getterBinding, type);
		}

		template <typename OwnerType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, std::function<void(OwnerType*,void*)> getFunction, OwnerType* getDataOwner, const UriNode& type)
		{
			auto getterBinding = [getFunction,getDataOwner](void* value, TemplateBinding* binding) {
				getFunction(getDataOwner,value);
			};
			return createConstantSizeLiteralNode(size, getterBinding, type);
		}

		


		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction, const UriNode& type);
		ConstantSizeLiteralNode getConstantSizeLiteralNode(NodeId nodeId) const;

		RelativeConstantSizeLiteralNode createRelativeLiteralNode(size_t size, uint32 offset, const UriNode& type, const Datasource& datasource);
		RelativeConstantSizeLiteralNode getRelativeConstantSizeLiteralNode(NodeId nodeId) const;

		template <typename OwnerType>
		DynamicLiteralNode createDynamicSizeLiteralNode(size_t size, std::function<void(OwnerType*, void*)> getDataFunction, std::function<size_t()> getSizeFunction, const UriNode& type)
		{
			auto getDataBinding = [getDataFunction](void* value, TemplateBinding* binding) {
				getDataFunction(value);
			};
			auto getSizeBinding = [getSizeFunction](TemplateBinding* binding) {
				return getSizeFunction();
			};
			return createConstantSizeLiteralNode(getDataBinding, getSizeBinding, type);
		}

		template <typename OwnerType>
		DynamicLiteralNode createDynamicSizeLiteralNode(size_t size, std::function<void(OwnerType*, void*)> getDataFunction, std::function<size_t(OwnerType*)> getSizeFunction, OwnerType* getterOwner, const UriNode& type)
		{
			auto getDataBinding = [getFunction, getDataOwner](void* value, TemplateBinding* binding) {
				getFunction(getterOwner, value);
			};
			auto getSizeBinding = [getSizeFunction](TemplateBinding* binding) {
				return getSizeFunction(getterOwner);
			};
			return createConstantSizeLiteralNode(getDataBinding, getSizeBinding, type);
		}

		DynamicLiteralNode createDynamicSizeLiteralNode(const DynamicLiteralNode::GetLiteralDataFunction & getDataFunction, const DynamicLiteralNode::GetLiteralByteSizeFunction & getSizeFunction, const UriNode & type);
		DynamicLiteralNode getDynamicSizeLiteralNode(NodeId nodeId) const;

		UriPlaceholderNode createUriPlaceHolderNode(const String& placeholderName);


		template <typename OwnerType>
		const Datasource& createDatasource(NodeId sourceNode, uint32 byteSize, std::function<void(OwnerType*, void*)> getDataFunction, const UriNode& type)
		{
			auto getDataBinding = [getDataFunction](void* value, TemplateBinding* binding) {
				getDataFunction(value);
			};
			return createDatasource(getDataBinding, getSizeBinding, type);
		}
		
		const Datasource& createDatasource(NodeId sourceNode, uint32 byteSize, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction);
		const Datasource& getDatasourceById(uint32 datasourceId) const;
		const Datasource& getDatasourceByGuid(Guid datasourceGuid) const;

		const Datatype* getDatatype(NodeId nodeId) const;

		void addDatatype(const UriNode& stringTypeNode, const UriNode& binaryTypeNode, Datatype* datatype);
		inline const Datatype* getDatatype(UriNode node) const
		{
			return getDatatype(node.getNodeId());
		}

		const Datatype* getDatatype(const String& uri) const;

		std::set<const Datatype*> getAllDatatypes() const;

		// unsecure
		const Literal* getLiteral(NodeId nodeId) const;

		void updateDatasource(uint32 datasourceId);

		NativeClassGraphTemplate* createGraphTemplate(const String& templateName, ReferentNode& nameNode);
			
		String getUriPlaceholderName(NodeId nodeId) const;
		

	private:
		uint64 mCurrentTimestamp;
		uint32 mNextBlankNodeId;
		uint32 mNextDynamicLiteralNodeId;
		uint32 mNextConstantLiteralNodeId;
		uint32 mNextConstantSizeLiteralNodeId;
		uint32 mNextRelativeConstantSizeLiteralNodeId;
		uint32 mNextUriNodeId;
		uint32 mNextUriPlaceholderNodeId;
		uint32 mNextDatasourceId;

		std::set<NodeId> mBlankNodes;
		std::map<NodeId, Uri> mUris;
		std::map<Uri, NodeId> mUriToNodeMapping;
		std::map<NodeId, DynamicLiteral> mDynamicLiterals;
		std::map<NodeId, ConstantLiteral> mConstantLiterals;
		std::map<NodeId, ConstantSizeLiteral> mConstantSizeLiterals;
		std::map<NodeId, RelativeConstantSizeLiteral> mRelativeConstantSizeLiterals;
		std::map<ConstantString, Uri> mNamespaces;
		std::map<NodeId, Datatype*> mDatatypes;

		std::map<uint32, Datasource> mDatasourcesById;
		std::map<Guid, Datasource> mDatasourcesByGuid;

		std::map<NodeId, Graph*> mNamedGraphs;
		std::map<String, NativeClassGraphTemplate*> mGraphTemplates;

		std::map<NodeId, String> mUriPlaceHolderNodes;

		std::map<NodeId, uint32> mReferenceCount;

		void registerDefaultDatatypes();
		Guid generateDataGuid(NodeId nodeId);

		void increaseReferenceCount(NodeId nodeId);
		void decreaseReferenceCount(NodeId nodeId);

	};
}