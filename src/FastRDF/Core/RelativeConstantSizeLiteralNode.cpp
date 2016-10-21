#include "stdafx.h"
#include "RelativeConstantSizeLiteralNode.h"


#include "Graph.h"

using namespace FastRDF;

NodeId FastRDF::RelativeConstantSizeLiteralNode::getLiteralDatatypeNode() const
{
	return mDataset->mRelativeConstantSizeLiterals[mNodeId].Datatype;
}

uint32 FastRDF::RelativeConstantSizeLiteralNode::getByteSize() const
{
	return mDataset->mRelativeConstantSizeLiterals[mNodeId].ByteSize;
}

void FastRDF::RelativeConstantSizeLiteralNode::getData(void * data)
{
	assert(false);
	// not implemented
}

FastRDF::RelativeConstantSizeLiteralNode::RelativeConstantSizeLiteralNode(const RelativeConstantSizeLiteralNode & other)
	: LiteralNode(other.mNodeId, other.mDataset)
{}

FastRDF::RelativeConstantSizeLiteralNode::~RelativeConstantSizeLiteralNode()
{

}

FastRDF::RelativeConstantSizeLiteralNode::RelativeConstantSizeLiteralNode(NodeId nodeId, Dataset* dataset)
	: LiteralNode(nodeId, dataset)
{}

const Datasource& FastRDF::RelativeConstantSizeLiteralNode::getDatasource() const
{
	const RelativeConstantSizeLiteral& literal = mDataset->mRelativeConstantSizeLiterals[mNodeId];
	return mDataset->getDatasourceById(literal.DatasourceId);
}

uint32 FastRDF::RelativeConstantSizeLiteralNode::getByteOffset() const
{
	return mDataset->mRelativeConstantSizeLiterals[mNodeId].ByteOffset;
}

uint32 FastRDF::RelativeConstantSizeLiteralNode::getDatasourceGUID() const
{
	return getDatasource().DatasourceGuid;
}

void FastRDF::RelativeConstantSizeLiteralNode::update()
{
	mDataset->mRelativeConstantSizeLiterals[mNodeId].TimeStamp = mDataset->mCurrentTimestamp;
}

Guid FastRDF::RelativeConstantSizeLiteralNode::getDataGuid() const
{
	RelativeConstantSizeLiteral& literal = mDataset->mRelativeConstantSizeLiterals[mNodeId];

	return literal.Guid;
}

Guid FastRDF::RelativeConstantSizeLiteralNode::generateDataGuid()
{
	Guid guid = mDataset->generateDataGuid(mNodeId);
	setDataGuid(guid);
	return guid;
}
void FastRDF::RelativeConstantSizeLiteralNode::setDataGuid(Guid dataGuid)
{
	RelativeConstantSizeLiteral& literal = mDataset->mRelativeConstantSizeLiterals[mNodeId];

	literal.Guid = dataGuid;
}



FastRDF::String FastRDF::RelativeConstantSizeLiteralNode::convertToString(TemplateBinding* binding) const
{
	//not supported
	assert(!mNodeId.isInvalid());
	return "";
}