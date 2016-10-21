#pragma once

#include "BinaryLiteralDescriptor.h"

namespace FastRDF
{
	class Graph;

	class TranslationTable
	{
		
	public:
		struct Entry
		{
			uint32 Guid;
			uint32 ByteOffset;
			uint32 ByteSize;
		};


		TranslationTable();

		void reset(uint32 newEntryCount);

		const void* getTableBlob() const
		{
			return &(mTable[0]);
		}

		void setFromTableBlob(void* blob, uint32 byteSize);

		const Entry* findEntry(Guid guid) const;
		uint32 findEntryIndex(Guid guid) const;

		void generateFromDescriptorList(const BinaryLiteralDescriptorList& descriptorList, const std::vector<uint32>& datasourceGuids, uint64 lastTimestamp, const Graph* graph, TemplateBinding* binding);

		uint32 getByteSize() const
		{
			return mSize * sizeof(Entry);
		}

		uint32 computePayloadByteSize() const;

		uint32 getEntryCount() const
		{
			return mSize;
		}

		Entry& operator[](uint32 index)
		{
			assert(index < mSize);
			return mTable[index];
		}

		const Entry& operator[](uint32 index) const
		{
			assert(index < mSize);
			return mTable[index];
		}


	private:
		Entry* mTable;
		uint32 mSize;
	};
}