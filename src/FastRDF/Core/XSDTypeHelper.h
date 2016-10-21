#pragma once

namespace FastRDF {
	class Dataset;

	template<typename T_Type>
	UriNode getXSDType(Dataset* dataset) {
		// not implemented
		assert(false);
	}

	template<>
	UriNode getXSDType<bool>(Dataset* dataset);
	template<>
	UriNode getXSDType<int08>(Dataset* dataset);
	template<>
	UriNode getXSDType<uint08>(Dataset* dataset);
	template<>
	UriNode getXSDType<int16>(Dataset* dataset);
	template<>
	UriNode getXSDType<uint16>(Dataset* dataset);
	template<>
	UriNode getXSDType<int32>(Dataset* dataset);
	template<>
	UriNode getXSDType<uint32>(Dataset* dataset);
	template<>
	UriNode getXSDType<int64>(Dataset* dataset);
	template<>
	UriNode getXSDType<uint64>(Dataset* dataset);
	template<>
	UriNode getXSDType<float>(Dataset* dataset);
	template<>
	UriNode getXSDType<double>(Dataset* dataset);
	template<>
	UriNode getXSDType<String>(Dataset* dataset);

}