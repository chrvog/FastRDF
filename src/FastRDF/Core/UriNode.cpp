#include "stdafx.h"
#include "UriNode.h"

#include "Graph.h"


using namespace FastRDF;

FastRDF::UriNode::UriNode()
	: ReferentNode()
{
}



const Uri & FastRDF::UriNode::getUri() const
{
	return mDataset->mUris[mNodeId];
}



FastRDF::UriNode::UriNode(Dataset* dataset, NodeId nodeId)
	: ReferentNode(dataset, nodeId)
{}

FastRDF::UriNode::UriNode(const UriNode & other)
	: ReferentNode(other)
{
}

UriNode& UriNode::operator=(const UriNode& uri)
{
	ReferentNode::operator=(uri);


	return *this;
}