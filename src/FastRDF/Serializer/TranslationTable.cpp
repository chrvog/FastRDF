#include <stdafx.h>
#include "TranslationTable.h"

#include <Core/Graph.h>
#include <Core/Datatype.h>

using namespace FastRDF;

FastRDF::TranslationTable::TranslationTable()
	: mTable(NULL)
	, mSize(0)
{}

void FastRDF::TranslationTable::reset(uint32 newEntryCount)
{
	if (newEntryCount != mSize)
	{
		delete[] mTable;

		mTable = new Entry[newEntryCount];
		mSize = newEntryCount;
	}
}

void FastRDF::TranslationTable::setFromTableBlob(void * blob, uint32 byteSize)
{
	reset(byteSize / sizeof(Entry));

	memcpy(mTable, blob, byteSize);
}

const TranslationTable::Entry* FastRDF::TranslationTable::findEntry(Guid guid) const
{
	uint32 index = findEntryIndex(guid);
	if (index != -1)
		return &(mTable[index]);

	return NULL;
}

uint32 FastRDF::TranslationTable::findEntryIndex(Guid guid) const
{
	for (uint32 i = 0; i < mSize; ++i)
	{
		if (mTable[i].Guid == guid)
		{
			return i;
		}
	}
	return -1;
}

void FastRDF::TranslationTable::generateFromDescriptorList(const BinaryLiteralDescriptorList & descriptorList, const std::vector<uint32>& datasourceGuids, uint64 lastTimestamp, const Graph* graph, TemplateBinding* binding)
{
	uint32 currentOffset = 0;

	std::vector<uint32> filteredLiteralList;
	std::vector<uint32> filteredDatasourceList;
	filteredLiteralList.reserve(descriptorList.size());
	filteredDatasourceList.reserve(datasourceGuids.size());

	for (uint32 i = 0; i < descriptorList.size(); ++i)
	{
		if (descriptorList[i].LiteralNodeId.Type >= NodeId::NODE_CONSTANT_LITERAL)
		{
			uint64 literalTimestamp = graph->getDataset()->getLiteral(descriptorList[i].LiteralNodeId)->TimeStamp;
			if (lastTimestamp <= literalTimestamp)
				filteredLiteralList.push_back(i);
		}
	}

	for (uint32 i = 0; i < datasourceGuids.size(); ++i)
	{
		uint64 literalTimestamp = graph->getDataset()->getDatasourceByGuid(datasourceGuids[i]).Timestamp;
		if (lastTimestamp <= literalTimestamp)
			filteredDatasourceList.push_back(i);
	}

	reset(filteredLiteralList.size() + filteredDatasourceList.size());

	for (uint32 filteredLiteralIter = 0; filteredLiteralIter < filteredLiteralList.size(); ++filteredLiteralIter)
	{
		uint32 descriptorListIndex = filteredLiteralList[filteredLiteralIter];

		mTable[filteredLiteralIter].Guid = descriptorList[descriptorListIndex].Guid;
		const Datatype* datatype = graph->getDatatype(descriptorList[descriptorListIndex].Datatype);
		assert(datatype != nullptr);
		if (datatype->isConstantSize())
			mTable[filteredLiteralIter].ByteSize = ((ConstantSizeDatatype*)datatype)->ByteSize;
		else
			mTable[filteredLiteralIter].ByteSize = graph->getDynamicLiteralNode(descriptorList[descriptorListIndex].LiteralNodeId).getByteSize(binding);
		mTable[filteredLiteralIter].ByteOffset = currentOffset;

		currentOffset += mTable[filteredLiteralIter].ByteSize;
	}

	for (uint32 filteredDatasourceIter = 0; filteredDatasourceIter < filteredDatasourceList.size();++filteredDatasourceIter)
	{
		uint32 datasourceIndex = filteredDatasourceList[filteredDatasourceIter];

		uint32 iter = filteredDatasourceIter + filteredLiteralList.size();
		mTable[iter].Guid = datasourceGuids[datasourceIndex];
		const Datasource& datasource = graph->getDataset()->getDatasourceByGuid((mTable[iter].Guid));
		mTable[iter].ByteSize = datasource.ByteSize;
		mTable[iter].ByteOffset = currentOffset;
		currentOffset += mTable[iter].ByteSize;
	}
}



uint32 FastRDF::TranslationTable::computePayloadByteSize() const
{
	uint32 size = 0;

	for (uint32 i = 0; i < mSize; ++i)
		size += mTable[i].ByteSize;

	return size;
}

