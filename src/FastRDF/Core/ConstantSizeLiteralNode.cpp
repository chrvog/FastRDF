#include "stdafx.h"
#include "ConstantSizeLiteralNode.h"


#include "Graph.h"

using namespace FastRDF;

 NodeId FastRDF::ConstantSizeLiteralNode::getLiteralDatatypeNode() const
{
	return mDataset->mConstantSizeLiterals[mNodeId].Datatype;
}

 uint32 FastRDF::ConstantSizeLiteralNode::getByteSize() const
 {
	 return mDataset->mConstantSizeLiterals[mNodeId].ByteSize;
 }

 void FastRDF::ConstantSizeLiteralNode::getData(void * data, TemplateBinding* binding)
 {
	 ConstantSizeLiteral& literal = mDataset->mConstantSizeLiterals[mNodeId];
	 literal.GetDataCallback(data, binding);
 }

FastRDF::ConstantSizeLiteralNode::ConstantSizeLiteralNode(const ConstantSizeLiteralNode & other)
 : LiteralNode(other.mNodeId, other.mDataset)
{}

FastRDF::ConstantSizeLiteralNode::~ConstantSizeLiteralNode()
{

}

void FastRDF::ConstantSizeLiteralNode::update()
{
  mDataset->mConstantSizeLiterals[mNodeId].TimeStamp = mDataset->mCurrentTimestamp;
}

FastRDF::ConstantSizeLiteralNode::ConstantSizeLiteralNode(NodeId nodeId, Dataset* dataset)
  : LiteralNode(nodeId, dataset)
{}


Guid FastRDF::ConstantSizeLiteralNode::getDataGuid() const
{
  ConstantSizeLiteral& literal = mDataset->mConstantSizeLiterals[mNodeId];

  return literal.Guid;
}

Guid FastRDF::ConstantSizeLiteralNode::generateDataGuid()
{
  Guid guid = mDataset->generateDataGuid(mNodeId);
  setDataGuid(guid);
  return guid;
}

void FastRDF::ConstantSizeLiteralNode::setDataGuid(Guid dataGuid)
{
  ConstantSizeLiteral& literal = mDataset->mConstantSizeLiterals[mNodeId];

  literal.Guid = dataGuid;
}

FastRDF::String FastRDF::ConstantSizeLiteralNode::convertToString(TemplateBinding* binding) const
{
  assert(!mNodeId.isInvalid());
  const auto& iter = mDataset->mConstantSizeLiterals.find(mNodeId);
  assert(iter != mDataset->mConstantSizeLiterals.end());

  ConstantSizeLiteral& literal = iter->second;

  const Datatype* datatype = mDataset->getDatatype(literal.Datatype);

  if (datatype->isConstantSize())
  {
	  const ConstantSizeDatatype* specializedDatatype = (const ConstantSizeDatatype*)datatype;

	  char* data = new char[literal.ByteSize];
	  literal.GetDataCallback(data, binding);
	  
	  return specializedDatatype->ToString(data, literal.ByteSize);
	  delete[] data;
  }
  else
  {
	  const DynamicSizeDatatype* specializedDatatype = (const DynamicSizeDatatype*)datatype;
	  
	  char* data = new char[literal.ByteSize];
	  literal.GetDataCallback(data, binding);

	  return specializedDatatype->ToString(data, literal.ByteSize);
	  delete[] data;
  }
}
