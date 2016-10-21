#include <stdafx.h>
#include "BinaryBlobDeserializer.h"

#include "../core/Datatype.h"
#include "../core/Graph.h"

using namespace FastRDF;

FastRDF::BinaryBlobDeserializer::BinaryBlobDeserializer()
	: mDataBlob(NULL)
{}

void FastRDF::BinaryBlobDeserializer::getDynamicSizeData(void * data, Guid guid)
{
	std::map<uint32, DynamicSizeLiteral>::iterator dynamicLiteralIter = mDynamicSizeLiterals.find(guid);
	if (dynamicLiteralIter != mDynamicSizeLiterals.end())
	{
		memcpy(data, (char*)dynamicLiteralIter->second.DataBlob, dynamicLiteralIter->second.ByteSize);
	}
}

void FastRDF::BinaryBlobDeserializer::getConstantSizeData(void * data, Guid guid, uint32 literalByteSize, uint32 literalOffset)
{
	const TranslationTable::Entry* entry = mTranslationTable.findEntry(guid);
	if (entry != NULL)
	{
		uint32 offset =  entry->ByteOffset + literalOffset;
		assert(offset + literalByteSize <= mBlobByteSize);
	
		memcpy(data, (char*)mDataBlob + offset, literalByteSize);
	}
}

uint32 FastRDF::BinaryBlobDeserializer::getDataSize(Guid guid)
{
	const TranslationTable::Entry* entry = mTranslationTable.findEntry(guid);
	if (entry != NULL)
		return entry->ByteSize;
	else
	{
		std::map<uint32, DynamicSizeLiteral>::iterator dynamicLiteralIter = mDynamicSizeLiterals.find(guid);
		if (dynamicLiteralIter != mDynamicSizeLiterals.end())
			return dynamicLiteralIter->second.ByteSize;
		// else
		return -1;
	}
}

void BinaryBlobDeserializer::setInitialBlob(const BinarySerializer::Blob& blob, const BinaryLiteralDescriptorList& literalList, Graph* graph)
{
	// setup transport table
	TranslationTable blobTranslationTable;
	blobTranslationTable.setFromTableBlob(blob.TranslationTable, blob.TranslationTableByteSize);


	//init new translationTable

	// setup datatype cache
	std::vector<const Datatype*> literalDatatypes;
	literalDatatypes.resize(blobTranslationTable.getEntryCount(), NULL);

	std::vector<uint32> descriptorIndices;
	descriptorIndices.resize(blobTranslationTable.getEntryCount());

	// find entry count
	// and build dynamic literals
	uint32 constEntryCount = 0;
	
	for (uint32 literalIter = 0; literalIter < blobTranslationTable.getEntryCount(); ++literalIter)
	{
		const TranslationTable::Entry& translationEntry = blobTranslationTable[literalIter];
		uint32 descriptorIndex = findInLiteralList(translationEntry.Guid, literalList);
		descriptorIndices[literalIter] = descriptorIndex;
		if (descriptorIndex == -1)
			continue;

		const BinaryLiteralDescriptor& descriptor = literalList[descriptorIndex];

		const Datatype* datatype = graph->getDatatype(descriptor.Datatype);
		literalDatatypes[literalIter] = datatype;
		
		if (descriptor.ConstantLiteral || datatype->isConstantSize())
			constEntryCount++;
		else
		{
			
			auto literalEntry = mDynamicSizeLiterals.emplace(std::make_pair(blobTranslationTable[literalIter].Guid,DynamicSizeLiteral()));

			literalEntry.first->second.ByteSize = blobTranslationTable[literalIter].ByteSize;
			literalEntry.first->second.DataBlob = new char[blobTranslationTable[literalIter].ByteSize];
		}
	}

	mTranslationTable.reset(constEntryCount);
	mOnChangedSignals.resize(constEntryCount);

	uint32 blobSize = 0;
	uint32 constSizeIter = 0;
	for (uint32 i = 0; i < blobTranslationTable.getEntryCount(); ++i)
	{
		const Datatype* datatype = literalDatatypes[i];

		// if not dynamic size literal (not constant, not datasource (no descriptor) and not constant size) 
		if (descriptorIndices[i] != -1 && !literalList[descriptorIndices[i]].ConstantLiteral)
			if (datatype != NULL && !datatype->isConstantSize())
				continue;

		uint32 size = blobTranslationTable[i].ByteSize;

		mTranslationTable[constSizeIter].Guid = blobTranslationTable[i].Guid;
		mTranslationTable[constSizeIter].ByteSize = size;
		mTranslationTable[constSizeIter].ByteOffset = blobSize;

		blobSize += size;
		constSizeIter++;
	}
	mDataBlob = new char[blobSize];
	mBlobByteSize = blobSize;
	updateBlob(blob);
}

void BinaryBlobDeserializer::parseTranslationTable(void* translationTable, uint32 byteSize)
{
	mTranslationTable.setFromTableBlob(translationTable, byteSize);
}

void FastRDF::BinaryBlobDeserializer::updateBlob(const BinarySerializer::Blob& updateBlob)
{
	TranslationTable newTable;
	newTable.setFromTableBlob(updateBlob.TranslationTable, updateBlob.TranslationTableByteSize);
	std::vector<LiteralChangedSignalType*> changedSignals;
	changedSignals.reserve(newTable.getEntryCount());

	for (uint32 transportTableIter = 0; transportTableIter < newTable.getEntryCount(); ++transportTableIter)
	{
		Guid guid = newTable[transportTableIter].Guid;
		uint32 translationTableIndex= mTranslationTable.findEntryIndex(guid);
		
		void * sourcePtr = (void*)((std::uintptr_t)updateBlob.Payload + (std::uintptr_t)newTable[transportTableIter].ByteOffset);

		if (translationTableIndex == -1)
		{
			std::map<uint32, DynamicSizeLiteral>::iterator dynamicLiteralIter = mDynamicSizeLiterals.find(guid);
			if (dynamicLiteralIter == mDynamicSizeLiterals.end())
				continue;
		
			// if size changed update blob + table
			if (newTable[transportTableIter].ByteSize != dynamicLiteralIter->second.ByteSize)
			{
				delete[] dynamicLiteralIter->second.DataBlob;
				dynamicLiteralIter->second.DataBlob = new char[newTable[transportTableIter].ByteSize];
				dynamicLiteralIter->second.ByteSize = newTable[transportTableIter].ByteSize;
				mTranslationTable[translationTableIndex].ByteSize = newTable[transportTableIter].ByteSize;
			}
			
			void* targetPtr = dynamicLiteralIter->second.DataBlob;
			memcpy(targetPtr , sourcePtr, newTable[transportTableIter].ByteSize);
			
			changedSignals.push_back(&(dynamicLiteralIter->second.OnChanged));
		}
		else
		{
			const TranslationTable::Entry&  entry = mTranslationTable[translationTableIndex];

			void* targetPtr = (void*)((std::uintptr_t)mDataBlob + (std::uintptr_t)entry.ByteOffset);
			memcpy(targetPtr, sourcePtr, newTable[transportTableIter].ByteSize);

			changedSignals.push_back(&mOnChangedSignals[translationTableIndex]);
		}

	}

	for (uint32 i = 0; i < changedSignals.size(); ++i)
	{
		(*changedSignals[i])();
	}
	
}

static LiteralChangedSignalType gUnusedSignal;



uint32 FastRDF::BinaryBlobDeserializer::findInLiteralList(Guid guid, const BinaryLiteralDescriptorList & literalList)
{
	for (uint32 i = 0; i < literalList.size(); ++i)
	{
		if (literalList[i].Guid == guid)
			return i;
	}
	return -1;
}


LiteralChangedSignalType& FastRDF::BinaryBlobDeserializer::getOnChangedSignal(Guid guid)
{
	for (uint32 i = 0; i < mTranslationTable.getEntryCount(); ++i)
	{
		if (mTranslationTable[i].Guid == guid)
			return mOnChangedSignals[i];
		else
		{
			std::map<uint32, DynamicSizeLiteral>::iterator dynamicLiteralIter = mDynamicSizeLiterals.find(guid);
			assert(dynamicLiteralIter != mDynamicSizeLiterals.end());
		
			return dynamicLiteralIter->second.OnChanged;
		}
	}
	assert(false);
	return *((LiteralChangedSignalType*)NULL);

}
