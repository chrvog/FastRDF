#include "stdafx.h"
#include "ReferentNode.h"


#include <Core/Dataset.h>

using namespace std;
using namespace FastRDF;



FastRDF::ReferentNode::~ReferentNode()
{
	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->decreaseReferenceCount(mNodeId);
}

FastRDF::ReferentNode::ReferentNode(const ReferentNode& other) : mNodeId(other.mNodeId), mDataset(other.mDataset)
{
	if (!mNodeId.isInvalid())
		mDataset->increaseReferenceCount(mNodeId);
}

ReferentNode& FastRDF::ReferentNode::operator=(const ReferentNode& other)
{
	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->decreaseReferenceCount(mNodeId);

	mNodeId = other.mNodeId;
	mDataset = other.mDataset;

	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->increaseReferenceCount(mNodeId);

	return *this;
}

bool FastRDF::ReferentNode::isBlankNode()
{
	return mNodeId.Type == NodeId::NODE_BLANK;
}

bool FastRDF::ReferentNode::isUriNode()
{
	return mNodeId.Type == NodeId::NODE_BLANK;
}

NodeId::NodeType FastRDF::ReferentNode::getNodeType()
{
	return (NodeId::NodeType) mNodeId.Type;
}

bool FastRDF::ReferentNode::isInvalid() const
{
	return mDataset == NULL || mNodeId.isInvalid();
}

FastRDF::ReferentNode::ReferentNode(Dataset* dataset, NodeId nodeId) : mNodeId(nodeId), mDataset(dataset)
{
	if (!nodeId.isInvalid())
		mDataset->increaseReferenceCount(nodeId);
}

bool FastRDF::ReferentNode::operator==(const ReferentNode& other)
{
	return mNodeId == other.mNodeId && mDataset == other.mDataset;
}

FastRDF::ReferentNode::ReferentNode() : mNodeId(), mDataset(NULL)
{

}
