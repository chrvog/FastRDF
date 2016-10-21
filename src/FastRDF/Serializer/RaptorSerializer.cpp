#include <stdafx.h>
#include "RaptorSerializer.h"

#include <Core/Graph.h>

#include <DefaultIncludes.h>
#include <Core/Graph.h>

#include <raptor2.h>


using namespace FastRDF;
using namespace std;


const String FastRDF::RaptorSerializer::RAPTOR_TURTLE = "turtle";
const String FastRDF::RaptorSerializer::RAPTOR_NTRIPLE = "ntriples";
const String FastRDF::RaptorSerializer::RAPTOR_XML = "rdfxml";

std::string FastRDF::RaptorSerializer::serializeGraph(const Graph* graph, const std::string& serializerFormat, TemplateBinding* binding)
{
	const Graph::TripleContainer& fastRDFTriples = graph->getTriple();
	
	if (fastRDFTriples.size() == 0)
		return "";

	mWorld = raptor_new_world();
	raptor_serializer* serializer = raptor_new_serializer(mWorld, serializerFormat.c_str());

	assert(raptor_serializer_set_option(serializer, RAPTOR_OPTION_WRITE_BASE_URI,(const char*)"true", 0) == 0);
	//assert(raptor_serializer_set_option(serializer, RAPTOR_OPTION_PREFIX_ELEMENTS,(const unsigned char*)"true", 0) == 0);

	auto namespaces = graph->getDataset()->getNamespaces();
	for (auto& nspace : namespaces) {
		raptor_uri* namespaceUri = raptor_new_uri(mWorld, (const unsigned char*)nspace.second.toString().get().c_str());
		raptor_serializer_set_namespace(serializer, namespaceUri, (const unsigned char*)nspace.first.get().c_str());
	}

	std::vector<raptor_statement*> raptorTriples;
	raptorTriples.reserve(fastRDFTriples.size());
	
	uint32 datasourcseByteSize = 0;
	std::map<uint32, uint32> datatsourceGuidLocalIdMapping;
	std::vector<uint32> datasourceOffsets;
	std::vector < std::function<void(void*,TemplateBinding*)> > datasourceGetter;
	datasourceOffsets.reserve(graph->getDatasourceReferences().size());
	for (const auto& datasourceReferences : graph->getDatasourceReferences())
	{
		if (datasourceReferences.second == 0)
			continue;

		const Datasource& datasource = graph->getDatasourceByGuid(datasourceReferences.first);
		datasourceOffsets.push_back(datasourcseByteSize);
		datasourcseByteSize += datasource.ByteSize;

		datatsourceGuidLocalIdMapping[datasource.DatasourceGuid] = datasourceGetter.size();
		datasourceGetter.push_back(datasource.GetDataCallback);
	}


	uint08* datasourceMemory = new uint08[datasourcseByteSize];

	for (uint32 i = 0; i < datasourceGetter.size(); ++i)
	{
		datasourceGetter[i]((void*)(datasourceMemory + datasourceOffsets[i]),binding);
	}





	for (uint32 i = 0; i < fastRDFTriples.size(); ++i)
	{
		const Triple& triple = fastRDFTriples[i];

		raptor_term* subjectTerm = getTerm(triple.Subject,graph,binding);
		raptor_term* predicateTerm = getTerm(triple.Predicate, graph, binding);
		raptor_term* objectTerm = NULL;
		if (triple.Object.Type != NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
			objectTerm = getTerm(triple.Object, graph, binding);
		else
		{
			auto literalNode = graph->getRelativeConstantSizeLiteralNode(triple.Object);
			uint32 datasourceGuid = literalNode.getDatasourceGUID();
			std::map<uint32, uint32>::const_iterator iter = datatsourceGuidLocalIdMapping.find(datasourceGuid);
			assert(iter != datatsourceGuidLocalIdMapping.end());
			
			NodeId datatypeId = literalNode.getLiteralDatatypeNode();
			const Datatype* datatype = graph->getDatatype(datatypeId);
			assert(datatype->isConstantSize());
			
			const ConstantSizeDatatype* csDatatype = (const ConstantSizeDatatype*)datatype;
			uint32 datasourceIndex = iter->second;
			String value = csDatatype->ToString((void*) (datasourceMemory + datasourceOffsets[datasourceIndex] + literalNode.getByteOffset()),csDatatype->ByteSize);
			
			String datatypeUriString = datatype->DatatypeUri.toString();
			
			raptor_uri* datatypeUri = raptor_new_uri(mWorld, (const unsigned char*)datatypeUriString.c_str());
			objectTerm = raptor_new_term_from_literal(mWorld, (const unsigned char*)value.c_str(), datatypeUri, NULL);
		}

		raptorTriples.push_back(raptor_new_statement_from_nodes(mWorld,subjectTerm,predicateTerm, objectTerm,NULL));
	}
	







	const char* result = NULL;
	raptor_serializer_start_to_string(serializer, NULL, (void**)&result, NULL);
	for (uint32 i = 0; i < raptorTriples.size(); ++i)
	{
		raptor_serializer_serialize_statement(serializer, raptorTriples[i]);
	}
	raptor_serializer_serialize_end(serializer);

	raptor_free_serializer(serializer);

	raptor_free_world(mWorld);

	delete[] datasourceMemory;
	return result;
}

raptor_term* FastRDF::RaptorSerializer::getTerm(NodeId nodeId, const Graph* graph, TemplateBinding* binding)
{
	assert(!nodeId.isInvalid());

	map<NodeId, raptor_term*>::const_iterator existingTermIter = mNodeMapping.find(nodeId);

	if (existingTermIter != mNodeMapping.end())
		return existingTermIter->second;

	NodeId::NodeType nodeType = (NodeId::NodeType)nodeId.Type;

	if (nodeType == NodeId::NODE_URI)
	{
		UriNode node = graph->getUriNode(nodeId);
		String uri = node.getUri().toString().get();
		
		raptor_term* uriTerm = raptor_new_term_from_uri_string(mWorld,(const unsigned char*) uri.c_str());
		mNodeMapping[nodeId] = uriTerm;
		
		return uriTerm;
	}
	else if (nodeType == NodeId::NODE_BLANK)
	{ 
		raptor_term* blankTerm = raptor_new_term_from_blank(mWorld, NULL);

		mNodeMapping[nodeId] = blankTerm;

		return blankTerm;
	}
	else
	{
		String value;
		const Datatype* datatype = NULL;

		// replace by proper inheritance...
		if (nodeType == NodeId::NODE_CONSTANT_LITERAL)
		{
			auto literalNode = graph->getConstantLiteralNode(nodeId);
			value = literalNode.convertToString();
			datatype = graph->getDatatype(literalNode.getLiteralDatatypeNode());
		}
		else if (nodeType == NodeId::NODE_CONSTANT_SIZE_LITERAL)
		{
			auto literalNode = graph->getConstantSizeLiteralNode(nodeId);
			value = literalNode.convertToString(binding);
			datatype = graph->getDatatype(literalNode.getLiteralDatatypeNode());
		}
		else if (nodeType == NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
		{
			auto literalNode = graph->getRelativeConstantSizeLiteralNode(nodeId);
			value = literalNode.convertToString(binding);

		}
		else if (nodeType == NodeId::NODE_DYNAMIC_LITERAL)
		{
			auto literalNode = graph->getDynamicLiteralNode(nodeId);
			value = literalNode.convertToString(binding);
			datatype = graph->getDatatype(literalNode.getLiteralDatatypeNode());
		}
		else
			assert(false);

		assert(datatype != NULL);
		String datatypeUriString = datatype->DatatypeUri.toString();
		raptor_uri* datatypeUri = raptor_new_uri(mWorld, (const unsigned char*)datatypeUriString.c_str());
		raptor_term* literalTerm = raptor_new_term_from_literal(mWorld, (const unsigned char*)value.c_str(),datatypeUri,NULL);

		mNodeMapping[nodeId] = literalTerm;

		return literalTerm;
	}
	

}
