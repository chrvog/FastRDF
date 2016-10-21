#include <stdafx.h>
#include "XSDTypeHelper.h"

#include "Dataset.h"

using namespace FastRDF;

template<>
UriNode FastRDF::getXSDType<bool>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "boolean");
}

template<>
UriNode FastRDF::getXSDType<String>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "string");
}

template<>
UriNode FastRDF::getXSDType<double>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "double");
}

template<>
UriNode FastRDF::getXSDType<float>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "float");
}

template<>
UriNode FastRDF::getXSDType<uint64>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "unsignedLong");
}

template<>
UriNode FastRDF::getXSDType<int64>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "long");
}

template<>
UriNode FastRDF::getXSDType<uint32>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "unsignedInt");
}

template<>
UriNode FastRDF::getXSDType<int32>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "int");
}

template<>
UriNode FastRDF::getXSDType<uint16>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "unsignedShort");
}

template<>
UriNode FastRDF::getXSDType<int16>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "short");
}

template<>
UriNode FastRDF::getXSDType<uint08>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "binary");
}

template<>
UriNode FastRDF::getXSDType<int08>(Dataset* dataset)
{
	return dataset->createUriNode("xsd", "byte");
}
