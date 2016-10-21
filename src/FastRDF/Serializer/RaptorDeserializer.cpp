#include "stdafx.h"
#include "RaptorDeserializer.h"

#include <core/Graph.h>
#include <raptor2.h>


#include <map>

using namespace FastRDF;
using namespace std;

struct RaptorParseData {
	Graph* mGraph;
	map<raptor_term*, NodeId> mNodeMapping;
};

NodeId createNode(RaptorParseData* data, raptor_term* raptorTerm)
{
	map<raptor_term*, NodeId>::const_iterator termIter = data->mNodeMapping.find(raptorTerm);
	NodeId termNodeId;
	if (termIter == data->mNodeMapping.end())
	{
		switch (raptorTerm->type)
		{
		case RAPTOR_TERM_TYPE_BLANK:
			termNodeId = data->mGraph->createBlankNode().getNodeId();
			break;
		case RAPTOR_TERM_TYPE_URI:
		{
			String uriString = String((const char*)raptor_uri_as_string(raptorTerm->value.uri));
			termNodeId = data->mGraph->createUriNode(uriString).getNodeId();
			break;
		}
		case RAPTOR_TERM_TYPE_LITERAL:
		{
			String valueString = String((const char*)raptorTerm->value.literal.string);
			String datatypeString = String((const char*)raptor_uri_as_string(raptorTerm->value.literal.datatype));

			const Datatype* datatype = data->mGraph->getDataset()->getDatatype(datatypeString);
			assert(datatype != NULL);

			void * value = NULL;
			uint32 byteSize = 0;
			if (datatype->isConstantSize())
			{
				const ConstantSizeDatatype* csDatatype = (const ConstantSizeDatatype*)datatype;
				byteSize = csDatatype->ByteSize;
				value = new char[byteSize];
				assert(csDatatype->FromString(valueString, value));
			}
			else // dynamic size
			{
				const DynamicSizeDatatype* csDatatype = (const DynamicSizeDatatype*)datatype;
				assert(csDatatype->FromString(valueString, value, byteSize));
			}
			UriNode datatypeUriNode = data->mGraph->createUriNode(datatype->DatatypeUri);
			ConstantLiteralNode node = data->mGraph->createConstantLiteralNode(value, byteSize, datatypeUriNode);
			termNodeId = node.getNodeId();

			break;
		}
		default:
			assert(false);
		}
	}
	else
	{
		termNodeId = termIter->second;
	}
	return termNodeId;
}

void parseRaptorTriple(void* userData, raptor_statement* statement)
{
	RaptorParseData* data = (RaptorParseData*)userData;

	NodeId subjectNodeId = createNode(data, statement->subject);
	NodeId predicateNodeId = createNode(data, statement->predicate);
	NodeId objectNodeId = createNode(data, statement->object);

	data->mGraph->addTriple(subjectNodeId, predicateNodeId, objectNodeId);

}

void FastRDF::RaptorDeserializer::deserializeGraph(const String& rdf, const Uri& baseUri, Graph* graph)
{
	raptor_world *world = raptor_new_world();
	raptor_uri *base_uri = raptor_new_uri(world,(const unsigned char*) baseUri.toString().get().c_str());
	raptor_parser* rdf_parser = raptor_new_parser_for_content(world,NULL,NULL,(const unsigned char*)rdf.c_str(),rdf.length(),NULL);

	RaptorParseData userdata;
	userdata.mGraph = graph;
	raptor_parser_set_statement_handler(rdf_parser, &userdata, &parseRaptorTriple);

	// parse entire buffer
	raptor_parser_parse_start(rdf_parser, base_uri);
	raptor_parser_parse_chunk(rdf_parser, (const unsigned char*)rdf.c_str(), rdf.length(), 1);

	// free everything
	raptor_free_parser(rdf_parser);
	raptor_free_uri(base_uri);
	raptor_free_world(world);
}
