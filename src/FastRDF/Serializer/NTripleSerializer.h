#pragma once

#include <Core/Graph.h>

namespace FastRDF
{
	struct ConstantSizeDatatype;
	struct TemplateBinding;

	class NTripleSerializer
	{
		struct NumericalTripleCacheEntry
		{
			ConstantString Text;
			uint32 LiteralStartIndex;
			const ConstantSizeDatatype* Datatype;
		};		

		struct DynamicTripleCacheEntry
		{
			uint32 ConstantSize;
			ConstantString ConstantPart;
			DynamicLiteralNode::GetLiteralByteSizeFunction SizeFunction;
			DynamicLiteralNode::GetLiteralDataFunction GetDataFunction;
			const DynamicSizeDatatype* Datatype;
		};

	public:
		std::string serializeGraph(const Graph* graph,TemplateBinding* binding = NULL);


	protected:

		std::map<Triple, ConstantString> mConstantTripleCache;
		std::map<Triple, DynamicTripleCacheEntry> mDynamicTripleCache;
		std::map<NodeId, ConstantString> mNodeToStringCache;
		std::map<Triple, NumericalTripleCacheEntry> mNumericalTripleCache;
		std::map<NodeId, uint32> mConstantSizeCache;
		std::map<NodeId, uint32> mConstantSizeSizeCache;

		void splitTriples(const Graph* graph, std::vector<Triple>& constantTriples, std::vector<Triple>& constantSizeTriples, std::vector<Triple>& dynamicTriples, std::vector<Triple>& templateTriples);

		uint32 calculateStringLength(const Graph* graph, const std::vector<Triple>& constantTriples, const std::vector<Triple>& constantSizeTriples, const std::vector<Triple>& dynamicTriples, const std::vector<Triple>& templateTriples, TemplateBinding* binding);
		
		uint32 buildDynamicTripleCache(const Triple& triple, const Graph* graph, TemplateBinding* binding);
		uint32 calculateConstantTripleLength(const Triple& triple, const Graph* graph);
		uint32 calculateConstantSizeTripleLength(const Triple& triple, const Graph* graph);
		uint32 calculateTemplateTripleLength(const Triple & triple, const Graph* graph, TemplateBinding* binding);

		ConstantString createConstantTripleCacheEntry(const Triple& triple, const Graph* graph);
		ConstantString createConstantSizeTripleCacheEntry(const Triple& triple, const Graph* graph);
		virtual const ConstantString& createConstantNodeString(NodeId nodeId, const Graph* graph);
		std::map<Triple, NTripleSerializer::DynamicTripleCacheEntry>::iterator createDynamicTripleCacheEntry(const Triple& triple, const Graph* graph);

		String buildString(uint32 stringSize, const Graph* graph, const std::vector<Triple>& constantTriples, const std::vector<Triple>& constantSizeTriples, const std::vector<Triple>& dynamicTriples, const std::vector<String>& dynamicLiteralValueStrings, const std::vector<Triple>& templateTriples, TemplateBinding* binding);
	
		void createDynamicValueStrings(std::vector<String>& valueStrings, const std::vector<Triple>& dynamicTriples, const Graph* graph, TemplateBinding* binding);
	};
}