#include "stdafx.h"
#include "DynamicLiteralNode.h"

#include "Graph.h"

using namespace FastRDF;


const DynamicLiteralNode::GetLiteralByteSizeFunction & FastRDF::DynamicLiteralNode::getGetLiteralSizeFunction() const
{
	return mDataset->mDynamicLiterals[mNodeId].GetByteSizeCallback;
}

void FastRDF::DynamicLiteralNode::setGetLiteralSizeFunction(const GetLiteralByteSizeFunction & function)
{
	mDataset->mDynamicLiterals[mNodeId].GetByteSizeCallback = function;
}

const DynamicLiteralNode::GetLiteralDataFunction & FastRDF::DynamicLiteralNode::getGetLiteralDataFunction() const
{
	return mDataset->mDynamicLiterals[mNodeId].GetDataCallback;
}

void FastRDF::DynamicLiteralNode::setGetLiteralDataFunction(const GetLiteralDataFunction & function)
{
	mDataset->mDynamicLiterals[mNodeId].GetDataCallback = function;

}

FastRDF::DynamicLiteralNode::DynamicLiteralNode(const DynamicLiteralNode & other)
: LiteralNode((const LiteralNode&)other)
{
}

FastRDF::DynamicLiteralNode::~DynamicLiteralNode()
{

}

const DynamicSizeDatatype * FastRDF::DynamicLiteralNode::getLiteralDatatype() const
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];
	const Datatype* datatype = mDataset->getDatatype(literal.Datatype);
	assert(!datatype->isConstantSize());
	return (DynamicSizeDatatype*)datatype;
}

NodeId FastRDF::DynamicLiteralNode::getLiteralDatatypeNode() const
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];
	return literal.Datatype;
}


FastRDF::DynamicLiteralNode::DynamicLiteralNode(Dataset* dataset, NodeId nodeId)
	: LiteralNode(nodeId, dataset)
{
}

void FastRDF::DynamicLiteralNode::getData(void * data, TemplateBinding* binding)
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];
	literal.GetDataCallback(data, binding);
}

uint32 FastRDF::DynamicLiteralNode::getByteSize(TemplateBinding* binding) const
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];
	return literal.GetByteSizeCallback(binding);
}

void FastRDF::DynamicLiteralNode::update()
{
	mDataset->mDynamicLiterals[mNodeId].TimeStamp = mDataset->mCurrentTimestamp;
}

Guid FastRDF::DynamicLiteralNode::getDataGuid() const
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];
	
	return literal.Guid;
}

Guid FastRDF::DynamicLiteralNode::generateDataGuid()
{
	Guid guid = mDataset->generateDataGuid(mNodeId);
	setDataGuid(guid);
	return guid;
}

void FastRDF::DynamicLiteralNode::setDataGuid(Guid dataGuid)
{
	DynamicLiteral& literal = mDataset->mDynamicLiterals[mNodeId];

	literal.Guid = dataGuid;
}


FastRDF::String FastRDF::DynamicLiteralNode::convertToString(TemplateBinding* binding) const
{
	assert(!mNodeId.isInvalid());
	const auto& iter = mDataset->mDynamicLiterals.find(mNodeId);
	assert(iter != mDataset->mDynamicLiterals.end());

	DynamicLiteral& literal = iter->second;

	const Datatype* datatype = mDataset->getDatatype(literal.Datatype);

	if (datatype->isConstantSize())
	{
		const ConstantSizeDatatype* specializedDatatype = (const ConstantSizeDatatype*)datatype;

		uint32 size = literal.GetByteSizeCallback(binding);
		char* data = new char[size];
		literal.GetDataCallback(data, binding);

		return specializedDatatype->ToString(data, literal.ByteSize);
		delete[] data;
	}
	else
	{
		const DynamicSizeDatatype* specializedDatatype = (const DynamicSizeDatatype*)datatype;

		uint32 size = literal.GetByteSizeCallback(binding);
		char* data = new char[size];
		literal.GetDataCallback(data, binding);

		return specializedDatatype->ToString(data, size);
		delete[] data;
	}
}