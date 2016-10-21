#pragma once

#include <Core/Graph.h>
#include "NTripleDeserializer.h"
#include "BinaryBlobDeserializer.h"

namespace FastRDF
{
	class GraphTranslationTable;

	class StructureDeserializer : protected NTripleDeserializer
	{
	public:

		StructureDeserializer();
		void deserializeGraph(const String& string, Graph* graph, const BinarySerializer::Blob& blob, BinaryBlobDeserializer* translationTable);


	private:
		NodeId mBinaryGuidNode;
		NodeId mBinaryOffsetNode;
		NodeId mBinaryConstNode;
		NodeId mRDFTypeNode;

		NodeId findBinaryType(NodeId blankNodeId, const Graph::TripleContainer& triple, uint32 currentIndex) const;
		void findLiteralBlankNodes(const Graph* graph, std::vector<BinaryLiteralDescriptor> &blanksToReplace);


	};
}
