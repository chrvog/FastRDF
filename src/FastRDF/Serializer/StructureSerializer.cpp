#include <stdafx.h>
#include "StructureSerializer.h"

#include <boost/lexical_cast.hpp>

#include <Core/Datatype.h>
#include "DefaultStringSerializer.h"

using namespace FastRDF;


FastRDF::StructureSerializer::StructureSerializer(Dataset* dataset)
{
	mLiteralCache.reserve(50);


}

String FastRDF::StructureSerializer::serializeStructure(const Graph* graph, BinaryLiteralDescriptorList& translationTable, std::vector<uint32>& datasourceGuids, TemplateBinding* binding)
{
	std::vector<Triple> dynamicTriples;
	std::vector<Triple> constantSizeTriples;
	std::vector<Triple> constantTriples;
	std::vector<Triple> templateTriples;

	const Graph::TripleContainer& triples = graph->getTriple();

	constantTriples.reserve(triples.size());


	translationTable.reserve(triples.size() / 2);

	// build translationtable and collect datasource guids
	for (const Triple& triple : triples)
	{
		if (triple.Object.Type == NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
		{
			Guid guid = graph->getRelativeConstantSizeLiteralNode(triple.Object).getDatasourceGUID();
			bool exists = false;
			for (uint32 i = 0; i < datasourceGuids.size(); ++i)
			{
				if (datasourceGuids[i] == guid)
				{
					exists = true;
					break;
				}
			}

			if (!exists)
				datasourceGuids.push_back(guid);
		}
	}


	// build caches and determine string size
	splitTriples(graph, constantTriples, constantTriples, constantTriples, templateTriples);

	uint32 size = calculateStringLength(graph, constantTriples, constantSizeTriples, dynamicTriples,templateTriples, binding);

	size += calculateDatasourceStringLength(graph, datasourceGuids);

	for (const Triple& triple : triples)
	{
		if (triple.Object.Type >= NodeId::NODE_CONSTANT_LITERAL && triple.Object.Type != NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
		{
			translationTable.push_back(findBinaryLiteralDescriptor(triple.Object));
		}
	}


	std::vector<String> dynamicValueStrings; // not used


	String rdf = buildString(size, graph, constantTriples, constantSizeTriples, dynamicTriples, dynamicValueStrings,templateTriples, binding);

	//for (Guid guid : datasourceGuids)
	//{
	//	rdf += mDatasourceCache[guid];
	//}

	return rdf;
}

const ConstantString & FastRDF::StructureSerializer::createConstantNodeString(NodeId nodeId, const Graph* graph)
{
	NodeId literalDatatype;
	uint32 size;
	ConstantString value;
	switch (nodeId.Type)
	{
	case NodeId::NODE_BLANK:
		value = String("_:b") + boost::lexical_cast<String>(nodeId.Id);
		break;
	case NodeId::NODE_URI:
	{
		UriNode uriNode = graph->getUriNode(nodeId);
		const Uri& uri = uriNode.getUri();
		const ConstantString& uriCString = uri.toString();
		value = String("<") + uriCString.get() + String(">");
		break;
	}
	case NodeId::NODE_CONSTANT_LITERAL:
	{
		auto node = graph->getConstantLiteralNode(nodeId);
		literalDatatype = node.getLiteralDatatypeNode();
		size = node.getLiteralByteSize();
		auto datatype = graph->getDatatype(literalDatatype);

		Guid guid = node.getDataGuid();
		if (guid == -1)
			guid = node.generateDataGuid();
			
		String guidString = uint32ToString(&guid, 4);

		String literalBlankNode = String("_:literal") + guidString;
		String localValue = literalBlankNode + " .\n";
		localValue += literalBlankNode + String(" <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <") + datatype->BinaryTypeUri.toString().get() + "> .\n";
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#constantLiteral/> \"true\"^^<http://www.w3.org/2001/XMLSchema#boolean> .\n");
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#guid/> \"") + guidString + "\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt>";

		value = localValue;
		BinaryLiteralDescriptor binaryDescriptor(nodeId, guid, 0, true, literalDatatype);
		mLiteralCache.push_back(binaryDescriptor);

		break;
	}
	case NodeId::NODE_CONSTANT_SIZE_LITERAL:
	{
		auto node = graph->getConstantSizeLiteralNode(nodeId);
		literalDatatype = node.getLiteralDatatypeNode();
		size = node.getByteSize();
		auto datatype = graph->getDatatype(literalDatatype);

		Guid guid = node.getDataGuid();
		if (guid == -1)
			guid = node.generateDataGuid();
		String guidString = uint32ToString(&guid,4);

		String literalBlankNode = String("_:literal") + guidString;
		String localValue = literalBlankNode + " .\n";
		localValue += literalBlankNode + String(" <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <") + datatype->BinaryTypeUri.toString().get() + "> .\n";
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#constantLiteral> \"false\"^^<http://www.w3.org/2001/XMLSchema#boolean> .\n");
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#guid/> \"") + guidString + "\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt>";

		value = localValue;
		
		BinaryLiteralDescriptor binaryDescriptor(nodeId,guid, 0, false, literalDatatype);
		mLiteralCache.push_back(binaryDescriptor);

		break;
	}
	case NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL:
	{
		auto node = graph->getRelativeConstantSizeLiteralNode(nodeId);
		literalDatatype = node.getLiteralDatatypeNode();
		size = node.getByteSize();
		const auto datatype = (ConstantSizeDatatype*)graph->getDatatype(literalDatatype);
		const auto& datasource = node.getDatasource();

		Guid guid = datasource.DatasourceGuid;
		String guidString = uint32ToString(&guid, 4);

		uint32 offset = node.getByteOffset();
		String offsetString = uint32ToString(&offset, 4);


		uint32 nodeGuid = node.getDataGuid();
		if (nodeGuid == -1)
			nodeGuid = node.generateDataGuid();
		String nodeGuidString = uint32ToString(&nodeGuid,4);

		String literalBlankNode = String("_:literal") + nodeGuidString;

		String localValue = literalBlankNode + " .\n";
		localValue += literalBlankNode + String(" <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <") + datatype->BinaryTypeUri.toString().get() + "> .\n";
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#constantLiteral> \"false\"^^<http://www.w3.org/2001/XMLSchema#boolean> .\n");
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#guid> \"") + guidString + "\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt> .\n";
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#offset> \"") + offsetString + "\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt>";
		value = localValue;

		BinaryLiteralDescriptor binaryDescriptor(nodeId, guid, offset, false, literalDatatype);
		mLiteralCache.push_back(binaryDescriptor);

		break;
	}
	case NodeId::NODE_DYNAMIC_LITERAL:
	{
		//get literal datatype and current size
		auto node = graph->getDynamicLiteralNode(nodeId);
		literalDatatype = node.getLiteralDatatypeNode();
		size = node.getByteSize(NULL);
		auto datatype = (DynamicSizeDatatype*)graph->getDatatype(literalDatatype);


		//create guid
		Guid guid = node.getDataGuid();
		if (guid == -1)
			guid = node.generateDataGuid();
		String guidString = uint32ToString(&guid, 4);

		//create cachable string
		String literalBlankNode = String("_:literal") + guidString;
		String localValue = literalBlankNode + " .\n";
		localValue += literalBlankNode + String(" <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <") + datatype->BinaryTypeUri.toString().get() + "> .\n";
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#constantLiteral> \"false\"^^<http://www.w3.org/2001/XMLSchema#boolean> .\n");
		localValue += literalBlankNode + String(" <http://vocab.arvida.de/binary#guid/> \"") + guidString + "\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt>";

		value = localValue;

		BinaryLiteralDescriptor binaryDescriptor(nodeId, guid, 0, false, literalDatatype);
		mLiteralCache.push_back(binaryDescriptor);

		break;

	}
	default:
		// unknown node type
		assert(false);
	}
	auto iter = mNodeToStringCache.emplace(nodeId, value).first;

	return iter->second;
}

const static BinaryLiteralDescriptor gInvalidBindaryLiteralDescriptor;

const BinaryLiteralDescriptor & FastRDF::StructureSerializer::findBinaryLiteralDescriptor(NodeId nodeId)
{
	for (const auto& literal : mLiteralCache)
	{
		if (literal.LiteralNodeId == nodeId)
			return literal;
	}

	return gInvalidBindaryLiteralDescriptor;
}

uint32 FastRDF::StructureSerializer::calculateDatasourceStringLength(const Graph* graph, std::vector<uint32> datasourceGuids)
{
	String binaryGuidPropertyString(" <http://vocab.arvida.de/binary#guid> \"");
	String guidEndString("\"^^<http://www.w3.org/2001/XMLSchema#unsignedInt> .\n");

	uint32 size = 0;
	for (Guid guid : datasourceGuids)
	{
		std::map<uint32, ConstantString>::const_iterator cacheIter = mDatasourceCache.find(guid);

		if (cacheIter == mDatasourceCache.end())
		{
			const Datasource& datasource = graph->getDataset()->getDatasourceByGuid(guid);
			std::map<NodeId, ConstantString>::const_iterator nodeCacheIter = mNodeToStringCache.find(datasource.DatasourceNode);
			if (nodeCacheIter == mNodeToStringCache.end())
			{
				createConstantNodeString(datasource.DatasourceNode, graph);
				nodeCacheIter = mNodeToStringCache.find(datasource.DatasourceNode);
			}
			String guidString = uint32ToString(&guid, 4);
			String newCacheEntry = nodeCacheIter->second.get() + binaryGuidPropertyString + guidString + guidEndString;

			mDatasourceCache[guid] = newCacheEntry;
			cacheIter = mDatasourceCache.find(guid);
		}

		size += cacheIter->second.get().length() + 2; // add (. or ;) and newline
	}
	return size;
}
