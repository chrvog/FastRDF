#pragma once


#include <string>
#include <map>

#include <raptor2.h>

namespace FastRDF
{
	class Graph;

	class RaptorSerializer 
	{
	public:
		const static String RAPTOR_TURTLE;
		const static String RAPTOR_NTRIPLE;
		const static String RAPTOR_XML;

		std::string serializeGraph(const Graph* graph, const std::string& serializerFormat, TemplateBinding* binding);

	protected:
		std::map<NodeId, raptor_term*> mNodeMapping;
		raptor_world* mWorld;
		
		raptor_term* getTerm(NodeId nodeId, const Graph* graph, TemplateBinding* binding);

	};
}