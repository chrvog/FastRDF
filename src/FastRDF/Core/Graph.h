#pragma once

#include "Dataset.h"
#include "Triple.h"

#include <map>

namespace FastRDF
{
	class Graph
	{
		friend class Dataset;
	public:
		typedef std::vector<Triple> TripleContainer;
		boost::signals2::signal<void(const Triple& triple)> OnTripleCreated;
		boost::signals2::signal<void(const Triple& triple)> OnTripleRemoving;



		void addNamespace(const String& name, const Uri& uri);

		UriNode createUriNode(const String& uri);
		UriNode createUriNode(const String& nameSpace, const String& relativePath);
		UriNode createUriNode(const Uri& uri);

		UriNode getUriNode(NodeId nodeId) const;

		UriNode findUriNode(const String& uri);
		UriNode findUriNode(const String& nameSpace, const String& relativePath);
		UriNode findUriNode(const Uri& uri);

		ReferentNode getNameNode() const;

		
		BlankNode createBlankNode();
		BlankNode getBlankNode(NodeId blankNodeId);

		ConstantLiteralNode createConstantLiteralNode(void* data, uint32 size, const UriNode& type);
		ConstantLiteralNode getConstantLiteralNode(NodeId nodeId) const;


		template<typename LiteralType>
		ConstantLiteralNode createConstantLiteralNode(const LiteralType& data, UriNode type = UriNode())
		{
			if (type.isInvalid())
				type = getXSDType<LiteralType>(mDataset);
			return mDataset->createConstantLiteralNode<LiteralType>(data, type);
		}


		template <typename LiteralType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(std::function<void(LiteralType&)> getFunction, UriNode type = UriNode())
		{
			return mDataset->createConstantSizeLiteralNode<LiteralType>(getFunction,type);
		}

		template <typename OwnerType, typename LiteralType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(std::function<void(OwnerType*, void*)> getFunction, OwnerType* getDataOwner, UriNode type = UriNode())
		{
			return mDataset->createConstantSizeLiteralNode<LiteralType>(getFunction, getDataOwner, type);
		}

		template <typename OwnerType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, std::function<void(void*)> getFunction, const UriNode& type)
		{
			return mDataset->createConstantSizeLiteralNode<OwnerType>(size, getFunction, type);
		}

		template <typename OwnerType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, std::function<void(OwnerType*, void*)> getFunction, OwnerType* getDataOwner, const UriNode& type)
		{
			return mDataset->createConstantSizeLiteralNode<OwnerType>(size, getFunction, getDataOwner, type);
		}

		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction,  const UriNode& type);
		ConstantSizeLiteralNode getConstantSizeLiteralNode(NodeId nodeId) const;

		RelativeConstantSizeLiteralNode createRelativeLiteralNode(size_t size, uint32 offset, const UriNode& type, const Datasource& datasource);
		RelativeConstantSizeLiteralNode getRelativeConstantSizeLiteralNode(NodeId nodeId) const;

		const Datasource& createDatasource(NodeId sourceNode, uint32 byteSize, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction);
		const Datasource& getDatasourceById(uint32 datasourceId) const;
		const Datasource& getDatasourceByGuid(uint32 datasourceGuid) const;

		DynamicLiteralNode createDynamicSizeLiteralNode(const DynamicLiteralNode::GetLiteralDataFunction & getDataFunction, const DynamicLiteralNode::GetLiteralByteSizeFunction & getSizeFunction, const UriNode & type);
		DynamicLiteralNode getDynamicLiteralNode(NodeId nodeId) const;

		const Datatype* getDatatype(NodeId nodeId) const;

		inline const Datatype* getDatatype(UriNode node) const
		{
			return getDatatype(node.getNodeId());
		}



		void addTriple(const ReferentNode& subject, const UriNode& predicate, const ReferentNode& object);
		void addTriple(const ReferentNode& subject, const UriNode& predicate, const ConstantLiteralNode& object);
		void addTriple(const ReferentNode& subject, const UriNode& predicate, const ConstantSizeLiteralNode& object);
		void addTriple(const ReferentNode& subject, const UriNode& predicate, const RelativeConstantSizeLiteralNode& object);
		void addTriple(const ReferentNode& subject, const UriNode& predicate, const DynamicLiteralNode& object);
		void addTriple(NodeId subject, NodeId predicate, NodeId object);


		bool removeTriple(NodeId subjectId, NodeId predicateId, NodeId objectId);

		const TripleContainer& getTriple() const
		{
			return mTriple;
		}

		void sortTripleBySubject();

	
		const std::map<uint32, uint32>& getDatasourceReferences() const
		{
			return mDatasourceReferenceCount;
		}

		
		// you know what you are doing...
		void setTriples(const TripleContainer& triples);

		


		const Dataset* getDataset() const
		{
			return mDataset;
		}

		Dataset* getDataset()
		{
			return mDataset;
		}

		uint64 getCurrentTimestamp() const;

		void createDebugOutput() const;

		void addLocalNamespace(const String& name, const Uri& uri);
		const std::map<ConstantString, Uri>& getLocalNamespaces() const;

		virtual bool isTemplate() {
			return false;
		}

		protected:

		Dataset* mDataset;
		std::map<ConstantString, Uri> mLocalNamespaces;

		
		TripleContainer mTriple;
		ReferentNode mNameNode;
		std::map<uint32, uint32> mDatasourceReferenceCount;

		Graph(Dataset* dataset, const ReferentNode& name);
		virtual ~Graph();
	};
}
