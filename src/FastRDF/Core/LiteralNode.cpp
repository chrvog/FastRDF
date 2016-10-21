#include "stdafx.h"
#include "LiteralNode.h"

#include <core/Dataset.h>

using namespace FastRDF;

LiteralNode::LiteralNode(NodeId nodeId, Dataset* dataset)
: mDataset(dataset)
, mNodeId(nodeId)
{
	if (!nodeId.isInvalid())
		mDataset->increaseReferenceCount(nodeId);
}

LiteralNode& FastRDF::LiteralNode::operator=(const LiteralNode& other)
{
	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->decreaseReferenceCount(mNodeId);

	mNodeId = other.mNodeId;
	mDataset = other.mDataset;

	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->increaseReferenceCount(mNodeId);

	return *this;
}


LiteralNode::LiteralNode(const LiteralNode& other)
: mDataset(other.mDataset)
, mNodeId(other.mNodeId)

{
	if (!mNodeId.isInvalid())
		mDataset->increaseReferenceCount(mNodeId);
}

FastRDF::LiteralNode::~LiteralNode()
{
	if (!mNodeId.isInvalid() && mDataset != NULL)
		mDataset->decreaseReferenceCount(mNodeId);
}


const Datatype* FastRDF::LiteralNode::getLiteralDatatype() const
{
	return mDataset->getDatatype(getLiteralDatatypeNode());
}

Dataset* LiteralNode::getDataset() const
{
	return mDataset;
}


NodeId LiteralNode::getNodeId() const
{
	return mNodeId;
}