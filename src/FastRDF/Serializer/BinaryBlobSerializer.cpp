#include <stdafx.h>
#include "BinaryBlobSerializer.h"

#include <Core/Graph.h>
#include "TranslationTable.h"

using namespace FastRDF;

BinarySerializer::Blob FastRDF::BinarySerializer::serializePayload(const Graph* graph, const BinaryLiteralDescriptorList& list, const std::vector<uint32>& datasourceGuids, uint64 timestamp, TemplateBinding* binding)
{
	Blob returnValue = { NULL, 0, NULL, 0 };

	TranslationTable translationTable;
	
	translationTable.generateFromDescriptorList(list, datasourceGuids, timestamp, graph, binding);

	uint32 payloadSize = translationTable.computePayloadByteSize();
	returnValue.Payload = new char[payloadSize];


	for (uint32 literalIter = 0; literalIter < list.size(); ++literalIter)
	{
		const TranslationTable::Entry* entry = translationTable.findEntry(list[literalIter].Guid);
		if (entry == NULL)
			continue;

		if (list[literalIter].LiteralNodeId.Type == NodeId::NODE_CONSTANT_SIZE_LITERAL)
		{
			auto node = graph->getConstantSizeLiteralNode(list[literalIter].LiteralNodeId);


			node.getData((char*)returnValue.Payload + entry->ByteOffset,binding);
		}
		else if (list[literalIter].LiteralNodeId.Type == NodeId::NODE_CONSTANT_LITERAL)
		{
			auto node = graph->getConstantLiteralNode(list[literalIter].LiteralNodeId);

			const void* data = node.getData();
			memcpy((char*)returnValue.Payload + entry->ByteOffset, data, entry->ByteSize);
		}
		else if (list[literalIter].LiteralNodeId.Type == NodeId::NODE_DYNAMIC_LITERAL)
		{
			auto node = graph->getDynamicLiteralNode(list[literalIter].LiteralNodeId);

			void* data = NULL;
			node.getData((char*)returnValue.Payload + entry->ByteOffset, binding);
		}
		else
			assert(false);
	}

	for (uint32 i = 0; i < datasourceGuids.size(); ++i)
	{
		const Datasource& datasource = graph->getDataset()->getDatasourceByGuid(datasourceGuids[i]);

		const TranslationTable::Entry* entry = translationTable.findEntry(datasourceGuids[i]);
		if (entry == NULL)
			continue;

		datasource.GetDataCallback((char*)returnValue.Payload + entry->ByteOffset, binding);
	}
	returnValue.TranslationTable = new char[translationTable.getByteSize()];

	memcpy(returnValue.TranslationTable, translationTable.getTableBlob(), translationTable.getByteSize());
	returnValue.TranslationTableByteSize = translationTable.getByteSize();
	returnValue.PayloadByteSize = translationTable.computePayloadByteSize();

	return returnValue;
}