#include "stdafx.h"
#include "ConstantLiteralNode.h"

#include "Dataset.h"

using namespace FastRDF;

FastRDF::ConstantLiteralNode::ConstantLiteralNode(const ConstantLiteralNode & other)
	: LiteralNode(other.mNodeId, other.mDataset)
{
}

FastRDF::ConstantLiteralNode::~ConstantLiteralNode()
{

}

uint32 FastRDF::ConstantLiteralNode::getLiteralByteSize() const
{
	const auto& iter = mDataset->mConstantLiterals.find(mNodeId);
	assert(iter != mDataset->mConstantLiterals.end());

	return iter->second.ByteSize;
}

NodeId FastRDF::ConstantLiteralNode::getLiteralDatatypeNode() const
{
	const auto& iter = mDataset->mConstantLiterals.find(mNodeId);
	assert(iter != mDataset->mConstantLiterals.end());

	return iter->second.Datatype;
}

FastRDF::ConstantLiteralNode::ConstantLiteralNode(Dataset * dataset, NodeId nodeId)
	: LiteralNode(nodeId, dataset)
{}



const void* ConstantLiteralNode::getData() const
{
	const auto& iter = mDataset->mConstantLiterals.find(mNodeId);
	assert(iter != mDataset->mConstantLiterals.end());
	
	return iter->second.Data;
}

void FastRDF::ConstantLiteralNode::update()
{
	mDataset->mConstantLiterals[mNodeId].TimeStamp = mDataset->mCurrentTimestamp;
}

Guid FastRDF::ConstantLiteralNode::getDataGuid() const
{
	ConstantLiteral& literal = mDataset->mConstantLiterals[mNodeId];

	return literal.Guid;
}

Guid FastRDF::ConstantLiteralNode::generateDataGuid()
{
	Guid guid = mDataset->generateDataGuid(mNodeId);
	setDataGuid(guid);
	return guid;
}

void FastRDF::ConstantLiteralNode::setDataGuid(Guid dataGuid)
{
	ConstantLiteral& literal = mDataset->mConstantLiterals[mNodeId];

	literal.Guid = dataGuid;
}

String FastRDF::ConstantLiteralNode::convertToString(TemplateBinding* binding) const
{
	assert(!mNodeId.isInvalid());
	const auto& iter = mDataset->mConstantLiterals.find(mNodeId);
	assert(iter != mDataset->mConstantLiterals.end());

	ConstantLiteral& literal = iter->second;

	const Datatype* datatype = mDataset->getDatatype(literal.Datatype);

	if (datatype->isConstantSize())
	{
		const ConstantSizeDatatype* specializedDatatype = (const ConstantSizeDatatype*)datatype;
		return specializedDatatype->ToString(literal.Data, literal.ByteSize);
	}
	else
	{
		const DynamicSizeDatatype* specializedDatatype = (const DynamicSizeDatatype*)datatype;
		return specializedDatatype->ToString(literal.Data, literal.ByteSize);
	}
}