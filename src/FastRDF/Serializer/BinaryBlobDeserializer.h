#pragma once

#include "BinaryLiteralDescriptor.h"
#include "BinaryBlobSerializer.h"
#include "TranslationTable.h"

#include <boost/signals2.hpp>

namespace FastRDF
{
	class LiteralNode;

	class BinaryBlobDeserializer
	{
	public:

		BinaryBlobDeserializer();

		void getDynamicSizeData(void* data, Guid guid);
		void getConstantSizeData(void* data, Guid guid, uint32 byteSize, uint32 literalOffset);
		uint32 getDataSize(Guid guid);

		void parseTranslationTable(void* translationTable, uint32 byteSize);

		void setInitialBlob(const BinarySerializer::Blob& blob, const BinaryLiteralDescriptorList& literalList, Graph* graph);

		void updateBlob(const BinarySerializer::Blob& updateBlob);


	private:
		struct DynamicSizeLiteral
		{
			DynamicSizeLiteral()
				: DataBlob(NULL), ByteSize(0)
			{}
			~DynamicSizeLiteral()
			{
				delete[] DataBlob;
			}

			DynamicSizeLiteral(const DynamicSizeLiteral& other)
				: DataBlob(NULL)
				, ByteSize(other.ByteSize)
			{
				if (ByteSize > 0)
					DataBlob = new char[ByteSize];

				memcpy(DataBlob, other.DataBlob, ByteSize);
				// boost signal is stupid, so we cannot copy the event receiver...
			}



			void* DataBlob;
			uint32 ByteSize;
			boost::signals2::signal<void()> OnChanged;
		};

		TranslationTable mTranslationTable;
		std::vector<LiteralChangedSignalType> mOnChangedSignals;
		void* mDataBlob;
		std::map<uint32, DynamicSizeLiteral> mDynamicSizeLiterals;
		uint32 mBlobByteSize;
		
		uint32 findInLiteralList(Guid guid, const BinaryLiteralDescriptorList& literalList);
		LiteralChangedSignalType& getOnChangedSignal(Guid guid);

	};
}
