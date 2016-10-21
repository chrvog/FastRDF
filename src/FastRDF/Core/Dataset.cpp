#include <stdafx.h>
#include <Core/Dataset.h>


#include <Core/Datatype.h>
#include <Core/NativeClassGraphTemplate.h>
#include "../Serializer/DefaultStringSerializer.h"
#include "Graph.h"
#include "Datasource.h"

#include <iostream>


using namespace std;
using namespace FastRDF;

FastRDF::Dataset::Dataset(uint32 timestamp)
	: mCurrentTimestamp(timestamp)
	, mNextBlankNodeId(0)
	, mNextDynamicLiteralNodeId(0)
	, mNextConstantLiteralNodeId(0)
	, mNextConstantSizeLiteralNodeId(0)
	, mNextRelativeConstantSizeLiteralNodeId(0)
	, mNextUriNodeId(0)
	, mNextUriPlaceholderNodeId(0)
	, mNextDatasourceId(0)
{
	addNamespace(ConstantString("xsd"), Uri("http://www.w3.org/2001/XMLSchema#"));
	addNamespace(ConstantString("rdf"), Uri("http://www.w3.org/1999/02/22-rdf-syntax-ns#"));
	addNamespace(ConstantString("binary"), Uri("http://vocab.arvida.de/binary#"));


	registerDefaultDatatypes();
}

Graph * FastRDF::Dataset::createGraph(const ReferentNode& nameNode)
{
	if (nameNode.isInvalid())
		return NULL;
	NodeId nameNodeId = nameNode.getNodeId();

	map<NodeId, Graph*>::iterator findExistingGraphIter = mNamedGraphs.find(nameNodeId);

	if (findExistingGraphIter == mNamedGraphs.end())
	{
		Graph* returnValue = new Graph(this,nameNode);
		mNamedGraphs.insert(std::pair<NodeId,Graph*>(nameNode.getNodeId(), returnValue));

		OnGraphCreated(returnValue);

		return returnValue;
	}

	return findExistingGraphIter->second;
}

void FastRDF::Dataset::deleteGraph(Graph* graph)
{
	assert(graph != NULL);

	map<NodeId, Graph*>::iterator findExistingGraphIter = mNamedGraphs.find(graph->mNameNode.getNodeId());

	assert(findExistingGraphIter != mNamedGraphs.end());

	OnGraphDeleting(graph);

	mNamedGraphs.erase(findExistingGraphIter);
	delete graph;
}

void FastRDF::Dataset::registerDefaultDatatypes()
{
	String xsd("xsd");
	// todo replace by xsdType Converter template
	UriNode boolNode = createUriNode(xsd, "boolean");
	UriNode boolBinaryNode = createUriNode("binary", "boolean");

	UriNode int08Node = createUriNode(xsd, "byte");
	UriNode int08BinaryNode = createUriNode("binary", "byte");
	UriNode uint08Node = createUriNode(xsd, "unsignedByte");
	UriNode uint08BinaryNode = createUriNode("binary", "unsignedByte");
	UriNode int16Node = createUriNode(xsd, "short");
	UriNode int16BinaryNode = createUriNode("binary", "short");
	UriNode uint16Node = createUriNode(xsd, "unsignedShort");
	UriNode uint16BinaryNode = createUriNode("binary", "unsignedShort");
	UriNode int32Node = createUriNode(xsd, "int");
	UriNode int32BinaryNode = createUriNode("binary", "int");
	UriNode uint32Node = createUriNode(xsd, "unsignedInt");
	UriNode uint32BinaryNode = createUriNode("binary", "unsignedInt");
	UriNode int64Node = createUriNode(xsd, "long");
	UriNode int64BinaryNode = createUriNode("binary", "long");
	UriNode uint64Node = createUriNode(xsd, "unsignedLong");
	UriNode uint64BinaryNode = createUriNode("binary", "unsignedLong");
	UriNode floatNode = createUriNode(xsd, "float");
	UriNode floatBinaryNode = createUriNode("binary", "float");
	UriNode doubleNode = createUriNode(xsd, "double");
	UriNode doubleBinaryNode = createUriNode("binary", "double");

	UriNode stringNode = createUriNode(xsd, "string");
	UriNode stringBinaryNode = createUriNode("binary", "string");

	ConstantSizeDatatype* boolType = new ConstantSizeDatatype();
	boolType->DatatypeUri = boolNode.getUri();
	boolType->BinaryTypeUri = boolBinaryNode.getUri();
	boolType->ByteSize = sizeof(bool);
	boolType->MaxStringSize = 5;
	boolType->ToString = &booleanToString;
	boolType->FromString = &stringToBoolean;

	//decimal count -> http://de.wikipedia.org/wiki/Integer_%28Datentyp%29

	ConstantSizeDatatype* int08Type = new ConstantSizeDatatype();
	int08Type->DatatypeUri = int08Node.getUri();
	int08Type->BinaryTypeUri = int08BinaryNode.getUri();
	int08Type->ByteSize = sizeof(int08);
	int08Type->MaxStringSize = 4;
	int08Type->ToString = &int08ToString;
	int08Type->FromString = &stringToInt08;

	ConstantSizeDatatype* uint08Type = new ConstantSizeDatatype();
	uint08Type->DatatypeUri = uint08Node.getUri();
	uint08Type->BinaryTypeUri = uint08BinaryNode.getUri();
	uint08Type->ByteSize = sizeof(uint08);
	uint08Type->MaxStringSize = 3;
	uint08Type->ToString = &uint08ToString;
	uint08Type->FromString = &stringToUInt08;

	ConstantSizeDatatype* int16Type = new ConstantSizeDatatype();
	int16Type->DatatypeUri = int16Node.getUri();
	int16Type->BinaryTypeUri = int16BinaryNode.getUri();
	int16Type->ByteSize = sizeof(int16);
	int16Type->MaxStringSize = 6;
	int16Type->ToString = &int16ToString;
	int16Type->FromString = &stringToInt16;

	ConstantSizeDatatype* uint16Type = new ConstantSizeDatatype();
	uint16Type->DatatypeUri = uint16Node.getUri();
	uint16Type->BinaryTypeUri = uint16Node.getUri();
	uint16Type->ByteSize = sizeof(uint16);
	uint16Type->MaxStringSize = 5;
	uint16Type->ToString = &uint16ToString;
	uint16Type->FromString = &stringToUInt16;

	ConstantSizeDatatype* int32Type = new ConstantSizeDatatype();
	int32Type->DatatypeUri = int32Node.getUri();
	int32Type->BinaryTypeUri = int32BinaryNode.getUri();
	int32Type->ByteSize = sizeof(int32);
	int32Type->MaxStringSize = 11;
	int32Type->ToString = &int32ToString;
	int32Type->FromString = &stringToInt32;

	ConstantSizeDatatype* uint32Type = new ConstantSizeDatatype();
	uint32Type->DatatypeUri = uint32Node.getUri();
	uint32Type->BinaryTypeUri = uint32BinaryNode.getUri();
	uint32Type->ByteSize = sizeof(uint32);
	uint32Type->MaxStringSize = 10;
	uint32Type->ToString = &uint32ToString;
	uint32Type->FromString = &stringToUInt32;

	ConstantSizeDatatype* int64Type = new ConstantSizeDatatype();
	int64Type->DatatypeUri = int64Node.getUri();
	int64Type->BinaryTypeUri = int64BinaryNode.getUri();
	int64Type->ByteSize = sizeof(int64);
	int64Type->MaxStringSize = 20;
	int64Type->ToString = &int64ToString;
	int64Type->FromString = &stringToInt64;

	ConstantSizeDatatype* uint64Type = new ConstantSizeDatatype();
	uint64Type->DatatypeUri = uint64Node.getUri();
	uint64Type->BinaryTypeUri = uint64BinaryNode.getUri();
	uint64Type->ByteSize = sizeof(uint64);
	uint64Type->MaxStringSize = 20;
	uint64Type->ToString = &uint64ToString;
	uint64Type->FromString = &stringToUInt64;

	ConstantSizeDatatype* floatType = new ConstantSizeDatatype();
	floatType->DatatypeUri = floatNode.getUri();
	floatType->BinaryTypeUri = floatBinaryNode.getUri();
	floatType->ByteSize = sizeof(float);
	floatType->MaxStringSize = 23;
	floatType->ToString = &floatToString;
	floatType->FromString = &stringToFloat;

	ConstantSizeDatatype* doubleType = new ConstantSizeDatatype();
	doubleType->DatatypeUri = doubleNode.getUri();
	doubleType->BinaryTypeUri = doubleBinaryNode.getUri();
	doubleType->ByteSize = sizeof(double);
	doubleType->MaxStringSize = 23;
	doubleType->ToString = &doubleToString;
	doubleType->FromString = &stringToDouble;

	DynamicSizeDatatype* stringType = new DynamicSizeDatatype();
	stringType->DatatypeUri = stringNode.getUri();
	stringType->BinaryTypeUri = stringBinaryNode.getUri();
	stringType->ToString = &stringDataToString;
	stringType->FromString = &stringToNullTerminatedStringData;

	addDatatype(boolNode,	boolBinaryNode,		boolType);
	addDatatype(int08Node,	int08BinaryNode,	int08Type);
	addDatatype(uint08Node, uint08BinaryNode,	uint08Type);
	addDatatype(int16Node,	int16BinaryNode,	int16Type);
	addDatatype(uint16Node,	uint16BinaryNode,	uint16Type);
	addDatatype(int32Node,	int32BinaryNode,	int32Type);
	addDatatype(uint32Node,	uint32BinaryNode,	uint32Type);
	addDatatype(int64Node,	int64BinaryNode,	int64Type);
	addDatatype(uint64Node,	uint64BinaryNode,	uint64Type);
	addDatatype(floatNode,	floatBinaryNode,	floatType);
	addDatatype(doubleNode,	doubleBinaryNode,	doubleType);
	addDatatype(stringNode,	stringBinaryNode,	stringType);
}

Guid FastRDF::Dataset::generateDataGuid(NodeId nodeId)
{
	assert(nodeId.Type >= NodeId::NODE_CONSTANT_LITERAL);
	return (uint32)nodeId;
}




void FastRDF::Dataset::increaseReferenceCount(NodeId nodeId)
{
	map<NodeId, uint32>::iterator iter = mReferenceCount.find(nodeId);

	if (iter == mReferenceCount.end())
		mReferenceCount[nodeId] = 1;
	else
		(iter->second)++;
}

void FastRDF::Dataset::decreaseReferenceCount(NodeId nodeId)
{
	map<NodeId, uint32>::iterator iter = mReferenceCount.find(nodeId);

	assert(iter != mReferenceCount.end());// reference count for unknown  node decreased

	if (iter->second == 1)
	{
		mReferenceCount.erase(iter);
	
		switch (nodeId.Type) {
		case NodeId::NODE_URI: 
		{
			std::cout << "delete: " << nodeId.Id << " " << mUris[nodeId].toString() << std::endl;
			map<NodeId, Uri>::const_iterator uriIter = mUris.find(nodeId);
			map<Uri, NodeId>::const_iterator nodeIter = mUriToNodeMapping.find(uriIter->second);
			mUriToNodeMapping.erase(nodeIter);
			mUris.erase(uriIter);
		}
		break;
		case NodeId::NODE_BLANK:
			mBlankNodes.erase(nodeId);
			break;
		case NodeId::NODE_CONSTANT_LITERAL:
			mConstantLiterals.erase(nodeId);
			break;
		case NodeId::NODE_CONSTANT_SIZE_LITERAL:
			mConstantSizeLiterals.erase(nodeId);
			break;
		case NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL:
			mRelativeConstantSizeLiterals.erase(nodeId);
			break;
		case NodeId::NODE_DYNAMIC_LITERAL:
			mDynamicLiterals.erase(nodeId);
			break;
		default:
			assert(false);
		}
	}
	else
		(iter->second)--;
}

void Dataset::addNamespace(const String& name, const Uri& uri)
{
	mNamespaces[ConstantString(name)] = uri;
}

const std::map<ConstantString, Uri>& FastRDF::Dataset::getNamespaces() const
{
	return mNamespaces;
}

UriNode FastRDF::Dataset::createUriNode(const String & uri)
{
	return createUriNode(Uri(uri));
}

UriNode FastRDF::Dataset::createUriNode(const Uri& uri)
{
	// check if uri already exists
	const auto& iter = mUriToNodeMapping.find(uri);
	if (iter != mUriToNodeMapping.end())
		return UriNode(this, iter->second);

	NodeId newNodeId(NodeId::NODE_URI, mNextUriNodeId++);
	mUris[newNodeId] = Uri(uri);
	mUriToNodeMapping[uri] = newNodeId;

	std::cout << "create: " << newNodeId.Id << " " << uri.toString() <<std::endl;

	return UriNode(this, newNodeId);
}

UriNode FastRDF::Dataset::createUriNode(const String & nameSpace, const String & relativePath)
{
	ConstantString namespaceString(nameSpace);

	const auto& namespaceIter = mNamespaces.find(namespaceString);
	assert(namespaceIter != mNamespaces.end());

	

	Uri& uri = namespaceIter->second;
	

	return createUriNode(uri.toString().get() + relativePath);
}

UriNode FastRDF::Dataset::findUriNode(const String & uri)
{
	return findUriNode(Uri(uri));
}

FastRDF::ReferentNode FastRDF::Dataset::getReferentNode(NodeId nodeId)
{
	if (nodeId.Type == NodeId::NODE_BLANK)
		return getBlankNode(nodeId);
	else if (nodeId.Type == NodeId::NODE_URI)
		return getUriNode(nodeId);

	return ReferentNode();
}

UriNode FastRDF::Dataset::findUriNode(const Uri& uri)
{
	// check if uri already exists
	const auto& iter = mUriToNodeMapping.find(uri);
	if (iter != mUriToNodeMapping.end())
		return UriNode(this, iter->second);

	return UriNode();
}

UriNode FastRDF::Dataset::getUriNode(NodeId nodeId) const
{
	assert(nodeId.Type == NodeId::NODE_URI);

	const auto& iter = mUris.find(nodeId);
	assert(iter != mUris.end());

	return UriNode((Dataset*)this, nodeId);
}


UriNode FastRDF::Dataset::findUriNode(const String & nameSpace, const String & relativePath)
{
	ConstantString namespaceString(nameSpace);

	const auto& namespaceIter = mNamespaces.find(namespaceString);
	assert(namespaceIter != mNamespaces.end());

	Path path(relativePath);

	Uri uri = namespaceIter->second;
	uri.append(relativePath);

	return findUriNode(uri);
}


BlankNode Dataset::createBlankNode()
{
	NodeId newNodeId(NodeId::NODE_BLANK, mNextBlankNodeId++);
	mBlankNodes.emplace(newNodeId);

	return BlankNode(this, newNodeId);
}

FastRDF::BlankNode FastRDF::Dataset::getBlankNode(NodeId blankNodeId)
{
	assert(blankNodeId.Type == NodeId::NODE_BLANK);
	set<NodeId>::iterator iter = mBlankNodes.find(blankNodeId);

	if (iter == mBlankNodes.end())
		return BlankNode();

	//else
	return BlankNode(this,blankNodeId);
}

ConstantLiteralNode FastRDF::Dataset::createConstantLiteralNode(void * data, uint32 size, const UriNode & type)
{
	ConstantLiteral literal;
	literal.ByteSize = size;
	literal.Data = new char[size];
	literal.Datatype = type.getNodeId();
	literal.TimeStamp = mCurrentTimestamp;

	memcpy(literal.Data, data, size);

	NodeId newId(NodeId::NODE_CONSTANT_LITERAL, mNextConstantLiteralNodeId++);
	mConstantLiterals[newId] = literal;

	return ConstantLiteralNode(this, newId);
}

ConstantSizeLiteralNode FastRDF::Dataset::createConstantSizeLiteralNode(size_t size, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction, const UriNode& type)
{
	ConstantSizeLiteral literal;
	literal.Datatype = type.getNodeId();
	literal.GetDataCallback = getDataFunction;
	literal.ByteSize = size;
	literal.TimeStamp = mCurrentTimestamp;

	NodeId newId(NodeId::NODE_CONSTANT_SIZE_LITERAL, mNextConstantSizeLiteralNodeId++);
	mConstantSizeLiterals[newId] = literal;

	return ConstantSizeLiteralNode(newId, this);
}


RelativeConstantSizeLiteralNode FastRDF::Dataset::createRelativeLiteralNode(size_t size, uint32 offset, const UriNode & type, const Datasource& datasource)
{
	RelativeConstantSizeLiteral literal;
	literal.Datatype = type.getNodeId();
	literal.ByteOffset = offset;
	literal.ByteSize = size;
	literal.DatasourceId = datasource.DatasourceId;
	literal.TimeStamp = mCurrentTimestamp;

	NodeId newId(NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL, mNextRelativeConstantSizeLiteralNodeId++);
	mRelativeConstantSizeLiterals[newId] = literal;

	return RelativeConstantSizeLiteralNode(newId, this);
}

DynamicLiteralNode FastRDF::Dataset::createDynamicSizeLiteralNode(const DynamicLiteralNode::GetLiteralDataFunction & getDataFunction, const DynamicLiteralNode::GetLiteralByteSizeFunction & getSizeFunction, const UriNode & type)
{
	DynamicLiteral literal;
	literal.Datatype = type.getNodeId();
	literal.GetDataCallback = getDataFunction;
	literal.GetByteSizeCallback = getSizeFunction;
	literal.TimeStamp = mCurrentTimestamp;

	NodeId newId(NodeId::NODE_DYNAMIC_LITERAL, mNextConstantSizeLiteralNodeId++);
	mDynamicLiterals[newId] = literal;

	return DynamicLiteralNode(this, newId);
}


FastRDF::UriPlaceholderNode FastRDF::Dataset::createUriPlaceHolderNode(const String& placeholderName)
{
	NodeId newID(NodeId::NODE_URI_PLACEHOLDER, mNextUriPlaceholderNodeId++);
	mUriPlaceHolderNodes[newID] = placeholderName;

	return UriPlaceholderNode(placeholderName, this, newID);
}

ConstantLiteralNode FastRDF::Dataset::getConstantLiteralNode(NodeId nodeId) const
{
	assert(nodeId.Type == NodeId::NODE_CONSTANT_LITERAL);

	const auto& iter = mConstantLiterals.find(nodeId);
	assert(iter != mConstantLiterals.end());

	return ConstantLiteralNode((Dataset*)this, nodeId);
}


DynamicLiteralNode FastRDF::Dataset::getDynamicSizeLiteralNode(NodeId nodeId) const
{
	assert(nodeId.Type == NodeId::NODE_DYNAMIC_LITERAL);

	const auto& iter = mDynamicLiterals.find(nodeId);
	assert(iter != mDynamicLiterals.end());

	return DynamicLiteralNode((Dataset*)this, nodeId);
}



const Datasource& FastRDF::Dataset::createDatasource(NodeId sourceNode, uint32 byteSize, const ConstantSizeLiteralNode::GetLiteralDataFunction& getDataFunction)
{
	uint32 datasourceId = mNextDatasourceId++;

	Datasource datasource;
	datasource.DatasourceNode = sourceNode;
	datasource.GetDataCallback = getDataFunction;
	datasource.ByteSize = byteSize;
	datasource.DatasourceId = datasourceId;
	datasource.Timestamp = mCurrentTimestamp;
	datasource.DatasourceGuid = datasourceId + 2000000000;
	datasource.Dataset = this;

	mDatasourcesById[datasourceId] = datasource;
	mDatasourcesByGuid[datasource.DatasourceGuid] = datasource;

	return mDatasourcesById[datasourceId];
}

ConstantSizeLiteralNode FastRDF::Dataset::getConstantSizeLiteralNode(NodeId nodeId) const
{
	assert(nodeId.Type == NodeId::NODE_CONSTANT_SIZE_LITERAL);

	const auto& iter = mConstantSizeLiterals.find(nodeId);
	assert(iter != mConstantSizeLiterals.end());

	return ConstantSizeLiteralNode(nodeId, (Dataset*)this);
}


RelativeConstantSizeLiteralNode FastRDF::Dataset::getRelativeConstantSizeLiteralNode(NodeId nodeId) const
{
	assert(nodeId.Type == NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL);

	const auto& iter = mRelativeConstantSizeLiterals.find(nodeId);
	assert(iter != mRelativeConstantSizeLiterals.end());

	return RelativeConstantSizeLiteralNode(nodeId, (Dataset*)this);
}

FastRDF::ConstantLiteralNode FastRDF::Dataset::createConstantLiteralNode(const String& data, const UriNode& type)
{
	return createConstantLiteralNode((void*)&data[0], data.length() + 1, type);
}

const FastRDF::Datatype* FastRDF::Dataset::getDatatype(NodeId nodeId) const
{
	auto iter = mDatatypes.find(nodeId);
	if (iter == mDatatypes.end())
		return NULL;

	return iter->second;
}

set<const Datatype*> FastRDF::Dataset::getAllDatatypes() const 
{
	set<const Datatype*> datatypes ;
	for_each(mDatatypes.begin(), mDatatypes.end(), [&datatypes](const std::map<NodeId, Datatype*>::value_type& value)
	{
		datatypes.insert(value.second); 
	});

	return datatypes;
}

const Datatype* FastRDF::Dataset::getDatatype(const String& uri) const
{
	std::map<Uri, NodeId>::const_iterator uriIter = mUriToNodeMapping.find(Uri(uri));

	if (uriIter == mUriToNodeMapping.end())
		return NULL;

	std::map<NodeId, Datatype*>::const_iterator datatypeIter = mDatatypes.find(uriIter->second);

	if (datatypeIter == mDatatypes.end())
		return NULL;

	return datatypeIter->second;
}

const Datasource & FastRDF::Dataset::getDatasourceById(uint32 datasourceId) const
{
	const auto& datasourceIter = mDatasourcesById.find(datasourceId);

	assert(datasourceIter != mDatasourcesById.end());

	return datasourceIter->second;
}

const Datasource & FastRDF::Dataset::getDatasourceByGuid(Guid datasourceGuid) const
{
	const auto& datasourceIter = mDatasourcesByGuid.find(datasourceGuid);

	assert(datasourceIter != mDatasourcesByGuid.end());

	return datasourceIter->second;
}

void FastRDF::Dataset::addDatatype(const UriNode& stringTypeNode, const UriNode& binaryTypeNode, Datatype* datatype)
{
	mDatatypes[stringTypeNode.getNodeId()] = datatype;
	
	if (!binaryTypeNode.getNodeId().isInvalid())
		mDatatypes[binaryTypeNode.getNodeId()] = datatype;

	if (!stringTypeNode.isInvalid())
		increaseReferenceCount(stringTypeNode.getNodeId());
	if (!binaryTypeNode.isInvalid())
		increaseReferenceCount(binaryTypeNode.getNodeId());

}

const Literal* FastRDF::Dataset::getLiteral(NodeId nodeId) const
{
	switch (nodeId.Type)
	{
	case NodeId::NODE_CONSTANT_LITERAL:
	{
		const auto& iter = mConstantLiterals.find(nodeId);
		if (iter != mConstantLiterals.end())
			return &iter->second;
		break;
	}
	case NodeId::NODE_CONSTANT_SIZE_LITERAL:
	{	const auto& iter = mConstantSizeLiterals.find(nodeId);
	if (iter != mConstantSizeLiterals.end())
		return &iter->second;
	break;
	}
	case NodeId::NODE_DYNAMIC_LITERAL:
	{
		const auto& iter = mDynamicLiterals.find(nodeId);
		if (iter != mDynamicLiterals.end())
			return &iter->second;
		break;
	}
	case NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL:
	{
		const auto& iter = mRelativeConstantSizeLiterals.find(nodeId);
		if (iter != mRelativeConstantSizeLiterals.end())
			return &iter->second;
		break;
	}
	default:
		assert(false);
	}

	return NULL;
}

void FastRDF::Dataset::updateDatasource(uint32 datasourceId)
{
	mDatasourcesById[datasourceId].Timestamp = mCurrentTimestamp;
}

FastRDF::NativeClassGraphTemplate* FastRDF::Dataset::createGraphTemplate(const String& templateName,ReferentNode& nameNode)
{
	auto existingTemplate = mGraphTemplates.find(templateName);
	if (existingTemplate != mGraphTemplates.end())
		return NULL;



	
	NativeClassGraphTemplate* returnValue = new NativeClassGraphTemplate(this, nameNode);
	mGraphTemplates[templateName] = returnValue;

	return returnValue;


}

FastRDF::String FastRDF::Dataset::getUriPlaceholderName(NodeId nodeId) const
{
	auto iter = mUriPlaceHolderNodes.find(nodeId);

	assert(iter != mUriPlaceHolderNodes.end());
	return iter->second;
}
