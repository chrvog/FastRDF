#include "stdafx.h"
#include "Graph.h"
#include "Dataset.h"

#include <iostream>

using namespace FastRDF;

void FastRDF::Graph::addTriple(NodeId subjectId, NodeId predicateId, NodeId objectId)
{
	assert(!subjectId.isInvalid());
	assert(!predicateId.isInvalid());
	assert(!objectId.isInvalid());
	mTriple.push_back(Triple(subjectId, predicateId, objectId));

	if (objectId.Type == NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
	{
		auto literal = getRelativeConstantSizeLiteralNode(objectId);
		std::map<uint32, uint32>::iterator iter = mDatasourceReferenceCount.find(literal.getDatasourceGUID());

		if (iter != mDatasourceReferenceCount.end())
			iter->second++;
		else
			mDatasourceReferenceCount[literal.getDatasourceGUID()] = 1;
	}


	mDataset->increaseReferenceCount(subjectId);
	mDataset->increaseReferenceCount(predicateId);
	mDataset->increaseReferenceCount(objectId);

	OnTripleCreated(Triple(subjectId, predicateId, objectId));
}

bool FastRDF::Graph::removeTriple(NodeId subjectId, NodeId predicateId, NodeId objectId)
{

	TripleContainer::iterator iter = find(mTriple.begin(), mTriple.end(), Triple(subjectId, predicateId, objectId));

	if (iter == mTriple.end())
		return false;

	OnTripleRemoving(Triple(subjectId, predicateId, objectId));

	mTriple.erase(iter);


	mDataset->decreaseReferenceCount(subjectId);
	mDataset->decreaseReferenceCount(predicateId);
	mDataset->decreaseReferenceCount(objectId);



	return true;
}

Graph::Graph(Dataset* dataset, const ReferentNode& name)
	: mDataset(dataset), mNameNode(name)
{}

void FastRDF::Graph::setTriples(const TripleContainer & triples)
{
	mTriple = triples;
}

uint64 FastRDF::Graph::getCurrentTimestamp() const
{
	return mDataset->getCurrentTimestamp();
}

void FastRDF::Graph::createDebugOutput() const
{
	
	for (Triple triple : mTriple)
	{
		std::cout << triple.Subject.Id << ":" << triple.Subject.Type << " ";
		if (triple.Subject.Type == NodeId::NODE_URI)
			std::cout << getUriNode(triple.Subject).getUri().toString() << "  ";

		std::cout << triple.Predicate.Id << ":" << triple.Predicate.Type << "  ";
		if (triple.Predicate.Type == NodeId::NODE_URI)
			std::cout << getUriNode(triple.Predicate).getUri().toString() << "  ";

		std::cout << triple.Object.Id << ":" << triple.Object.Type << "  ";
		if (triple.Object.Type == NodeId::NODE_URI)
			std::cout << getUriNode(triple.Object).getUri().toString();

		std::cout << std::endl;

	}

}

void Graph::addLocalNamespace(const String& name, const Uri& uri)
{
	mLocalNamespaces[ConstantString(name)] = uri;
}

const std::map<ConstantString, Uri>& FastRDF::Graph::getLocalNamespaces() const
{
	return mLocalNamespaces;
}

void Graph::addNamespace(const String& name, const Uri& uri)
{
	mDataset->addNamespace(name, uri);
}

FastRDF::Graph::~Graph()
{
	for (const Triple& triple : mTriple)
	{
		mDataset->decreaseReferenceCount(triple.Subject);
		mDataset->decreaseReferenceCount(triple.Predicate);
		mDataset->decreaseReferenceCount(triple.Object);
	}
}

UriNode FastRDF::Graph::createUriNode(const String & uri)
{
	return mDataset->createUriNode(uri);
}

UriNode FastRDF::Graph::createUriNode(const Uri& uri)
{
	return mDataset->createUriNode(uri);
}



UriNode FastRDF::Graph::createUriNode(const String & nameSpace, const String & relativePath)
{
	ConstantString namespaceString(nameSpace);

	const auto& namespaceIter = mLocalNamespaces.find(namespaceString);
	if (namespaceIter == mLocalNamespaces.end())
		return mDataset->createUriNode(nameSpace, relativePath);
	else
		return mDataset->createUriNode(Uri(namespaceIter->second.toString().get() + relativePath));
	

	Path path(relativePath);

	Uri uri = namespaceIter->second;
	uri.append(relativePath);

	return createUriNode(uri);
}

UriNode FastRDF::Graph::findUriNode(const String & uri)
{
	return mDataset->findUriNode(uri);
}

FastRDF::ReferentNode FastRDF::Graph::getNameNode() const
{
	return mNameNode;
}

UriNode FastRDF::Graph::findUriNode(const Uri& uri)
{
	return mDataset->findUriNode(uri);
}



UriNode FastRDF::Graph::findUriNode(const String & nameSpace, const String & relativePath)
{
	ConstantString namespaceString(nameSpace);

	const auto& namespaceIter = mLocalNamespaces.find(namespaceString);
	if (namespaceIter == mLocalNamespaces.end())
		return mDataset->findUriNode(nameSpace, relativePath);

	Path path(relativePath);

	Uri uri = namespaceIter->second;
	uri.append(relativePath);

	return findUriNode(uri);
}


BlankNode Graph::createBlankNode()
{
	return mDataset->createBlankNode();
}

FastRDF::BlankNode FastRDF::Graph::getBlankNode(NodeId blankNodeId)
{
	return mDataset->getBlankNode(blankNodeId);
}

ConstantLiteralNode FastRDF::Graph::createConstantLiteralNode(void * data, uint32 size, const UriNode & type)
{
	return mDataset->createConstantLiteralNode(data,size,type);
}

ConstantSizeLiteralNode FastRDF::Graph::createConstantSizeLiteralNode(size_t size, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction, const UriNode& type)
{
	return mDataset->createConstantSizeLiteralNode(size, getDataFunction, type);
}

DynamicLiteralNode FastRDF::Graph::createDynamicSizeLiteralNode(const DynamicLiteralNode::GetLiteralDataFunction & getDataFunction, const DynamicLiteralNode::GetLiteralByteSizeFunction & getSizeFunction, const UriNode & type)
{
	return mDataset->createDynamicSizeLiteralNode(getDataFunction, getSizeFunction,  type);
}

RelativeConstantSizeLiteralNode FastRDF::Graph::createRelativeLiteralNode(size_t size, uint32 offset, const UriNode & type, const Datasource & datasource)
{
	return mDataset->createRelativeLiteralNode(size,offset,type,datasource);
}



UriNode FastRDF::Graph::getUriNode(NodeId nodeId) const
{
	return mDataset->getUriNode(nodeId);
}


ConstantLiteralNode FastRDF::Graph::getConstantLiteralNode(NodeId nodeId) const
{
	return mDataset->getConstantLiteralNode(nodeId);
}


DynamicLiteralNode FastRDF::Graph::getDynamicLiteralNode(NodeId nodeId) const
{
	return mDataset->getDynamicSizeLiteralNode(nodeId);
}

ConstantSizeLiteralNode FastRDF::Graph::getConstantSizeLiteralNode(NodeId nodeId) const
{
	return mDataset->getConstantSizeLiteralNode(nodeId);
}

RelativeConstantSizeLiteralNode FastRDF::Graph::getRelativeConstantSizeLiteralNode(NodeId nodeId) const
{
	return mDataset->getRelativeConstantSizeLiteralNode(nodeId);
}

const Datasource & FastRDF::Graph::createDatasource(NodeId sourceNode, uint32 byteSize, const ConstantSizeLiteralNode::GetLiteralDataFunction & getDataFunction )
{
	return mDataset->createDatasource(sourceNode, byteSize, getDataFunction);
}

const FastRDF::Datatype* FastRDF::Graph::getDatatype(NodeId nodeId) const
{
	return mDataset->getDatatype(nodeId);
}

const Datasource & FastRDF::Graph::getDatasourceById(uint32 datasourceId) const
{
	return mDataset->getDatasourceById(datasourceId);
}

const Datasource & FastRDF::Graph::getDatasourceByGuid(uint32 datasourceGuid) const
{
	return mDataset->getDatasourceByGuid(datasourceGuid);
}

void FastRDF::Graph::addTriple(const ReferentNode & subject, const UriNode & predicate, const ReferentNode & object)
{
	addTriple(subject.getNodeId(), predicate.getNodeId(), object.getNodeId());
}

void FastRDF::Graph::addTriple(const ReferentNode & subject, const UriNode & predicate, const ConstantLiteralNode & object)
{
	addTriple(subject.getNodeId(), predicate.getNodeId(), object.getNodeId());
}

void FastRDF::Graph::addTriple(const ReferentNode & subject, const UriNode & predicate, const ConstantSizeLiteralNode & object)
{
	addTriple(subject.getNodeId(), predicate.getNodeId(), object.getNodeId());
}

void FastRDF::Graph::addTriple(const ReferentNode & subject, const UriNode & predicate, const RelativeConstantSizeLiteralNode & object)
{
	addTriple(subject.getNodeId(), predicate.getNodeId(), object.getNodeId());
}

void FastRDF::Graph::addTriple(const ReferentNode & subject, const UriNode & predicate, const  DynamicLiteralNode & object)
{
	addTriple(subject.getNodeId(), predicate.getNodeId(), object.getNodeId());
}




void FastRDF::Graph::sortTripleBySubject()
{
	std::sort(mTriple.begin(), mTriple.end(), [](const Triple& a, const Triple& b) { return a.Subject < b.Subject; });
}



