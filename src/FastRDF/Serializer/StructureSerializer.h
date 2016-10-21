#pragma once

#include "NTripleSerializer.h"
#include "TranslationTable.h"


namespace FastRDF
{
	struct TemplateBinding;

	class StructureSerializer : protected NTripleSerializer
	{
	public:
		StructureSerializer(Dataset* dataset);
		String serializeStructure(const Graph* graph, BinaryLiteralDescriptorList& translationTable, std::vector<uint32>& datasourceGuids, TemplateBinding* binding = NULL);

	private:
		std::vector<BinaryLiteralDescriptor> mLiteralCache;
		std::map<uint32, ConstantString> mDatasourceCache;

		ConstantString mBinaryGuidNodeString;


		virtual const ConstantString& createConstantNodeString(NodeId nodeId, const Graph* graph);
		const BinaryLiteralDescriptor& findBinaryLiteralDescriptor(NodeId nodeId);
		uint32 calculateDatasourceStringLength(const Graph* graph, std::vector<uint32> datasourceGuids);


	};
}