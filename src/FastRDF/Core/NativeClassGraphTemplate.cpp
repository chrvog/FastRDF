#include <stdafx.h>
#include "NativeClassGraphTemplate.h"
#include <core/UriPlaceHolderNode.h>

using namespace FastRDF;

NativeClassGraphTemplate::NativeClassGraphTemplate(Dataset* dataset, const ReferentNode& nameNode)
	: Graph(dataset, nameNode)
{

}

FastRDF::NativeClassGraphTemplate::~NativeClassGraphTemplate()
{

}

