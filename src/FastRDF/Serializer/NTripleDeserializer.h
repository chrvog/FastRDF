#pragma once

#include <Core/ConstantLiteralNode.h>

namespace FastRDF
{
	class Graph;

	class NTripleDeserializer
	{
	public:
		void deserializeGraph(const String& string, Graph* graph);

	private:
		struct TripleIndices
		{
			uint32 SubjectStart = -1;
			uint32 SubjectEnd = -1;
			uint32 PredicateStart = -1;
			uint32 PredicateEnd = -1;
			uint32 ObjectStart = -1;
			uint32 ObjectEnd = -1;
			uint32 DatatypeStart = -1;
			uint32 DatatypeEnd = -1;
		};


		ConstantLiteralNode createLiteralNode(Graph* graph, const String& text, const TripleIndices& tripleIndices);
	};
}
