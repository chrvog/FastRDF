#include <stdafx.h>
#include "UriPlaceHolderNode.h"

using namespace FastRDF;

const FastRDF::String& FastRDF::UriPlaceholderNode::getPlaceholderName() const
{
	return mPlaceholderName;
}

FastRDF::UriPlaceholderNode::UriPlaceholderNode(const UriPlaceholderNode& other) : mPlaceholderName(other.mPlaceholderName)
{

}

FastRDF::UriPlaceholderNode& FastRDF::UriPlaceholderNode::operator=(const UriPlaceholderNode& other)
{
	mPlaceholderName = other.mPlaceholderName;
	return *this;
}

FastRDF::UriPlaceholderNode::UriPlaceholderNode(String placeholderName, Dataset* dataset, NodeId nodeId) 
	: ReferentNode(dataset,nodeId)
	, mPlaceholderName(placeholderName)
{

}
