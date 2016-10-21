#pragma once

#include "BinaryLiteralDescriptor.h"
#include <core/TemplateBinding.h>

namespace FastRDF
{
	class Graph;

	class TranslationTable;

	class BinarySerializer
	{
	public:
		struct Blob
		{
			void* TranslationTable;
			uint32 TranslationTableByteSize;

			void* Payload;
			uint32 PayloadByteSize;
		};

		static Blob serializePayload(const Graph* graph,const BinaryLiteralDescriptorList& translationTable, const std::vector<uint32>& datasourceGuids, uint64 timeStamp, TemplateBinding* binding);
	};
}