#include "stdafx.h"
#include "StructureDeserializer.h"

#include <Core/Graph.h>
#include <Core/Datatype.h>
#include "BinaryBlobDeserializer.h"

#include <boost/bind.hpp>

#include <iostream>

using namespace FastRDF;



FastRDF::StructureDeserializer::StructureDeserializer()
	: NTripleDeserializer()
{
	
}

void FastRDF::StructureDeserializer::deserializeGraph(const String & string, Graph* graph, const BinarySerializer::Blob& blob, BinaryBlobDeserializer* binaryBlobDeserializer)
{
	graph->addNamespace("binary", Uri("http://vocab.arvida.de/binary/"));
	NTripleDeserializer::deserializeGraph(string, graph);


	graph->sortTripleBySubject();


	mBinaryGuidNode = graph->findUriNode("binary", "guid").getNodeId();
	mBinaryOffsetNode = graph->findUriNode("binary", "offset").getNodeId();
	mBinaryConstNode = graph->findUriNode("binary", "constantLiteral").getNodeId();
	mRDFTypeNode= graph->findUriNode("rdf", "type").getNodeId();


	if (mBinaryGuidNode.isInvalid())
		return;

	BinaryLiteralDescriptorList blanksToReplace;
	blanksToReplace.reserve(50);



	findLiteralBlankNodes(graph,blanksToReplace);
	// sort by blank node id
	std::sort(blanksToReplace.begin(), blanksToReplace.end(), [](const BinaryLiteralDescriptor& a, const BinaryLiteralDescriptor& b) {return a.LiteralNodeId < b.LiteralNodeId; });

	
	const Graph::TripleContainer& triples = graph->getTriple();
	std::vector<std::pair<uint32,uint32> > usageIndices(blanksToReplace.size());
	
	for (uint32 literalIter = 0; literalIter < blanksToReplace.size(); ++literalIter)
	{
		for (uint32 tripleIter = 0; tripleIter < triples.size(); ++tripleIter)
		{
			if (triples[tripleIter].Object == blanksToReplace[literalIter].LiteralNodeId)
			{
				usageIndices[literalIter] = std::pair<uint32,uint32>(tripleIter,literalIter);
			}
		}
	}

	std::sort(usageIndices.begin(), usageIndices.end(), [](const std::pair<uint32, uint32>& a, const std::pair<uint32, uint32>& b) -> bool { return a.first < b.first;});

	BinaryLiteralDescriptorList finalList;
	finalList.reserve(blanksToReplace.size());

	// triples are still sorted by subject node id
	uint32 literalIndex = 0;
	uint32 usageIter = 0;
	Graph::TripleContainer newTriples;
	newTriples.reserve(triples.size() - (2 * blanksToReplace.size()));

	//replace binary literal nodes
	for (uint32 tripleIter = 0; tripleIter < triples.size(); ++tripleIter)
	{

		while (literalIndex < blanksToReplace.size() && triples[tripleIter].Subject > blanksToReplace[literalIndex].LiteralNodeId)
			literalIndex++;

		if (usageIter < usageIndices.size() && tripleIter == usageIndices[usageIter].first)
		{
			uint32 blankIter = usageIndices[usageIter].second;
			const Datatype* datatype = graph->getDatatype(blanksToReplace[blankIter].Datatype);
			if (datatype->isConstantSize())
			{
				ConstantSizeDatatype* constantSizeDatatype = (ConstantSizeDatatype*)graph->getDatatype(blanksToReplace[blankIter].Datatype);
				assert(constantSizeDatatype != NULL);
				auto getFunction = std::bind(&BinaryBlobDeserializer::getConstantSizeData, binaryBlobDeserializer, std::placeholders::_1, blanksToReplace[blankIter].Guid, constantSizeDatatype->ByteSize, blanksToReplace[blankIter].DatasourceOffset);
				auto constantSizeNode = graph->createConstantSizeLiteralNode(constantSizeDatatype->ByteSize, getFunction, graph->getUriNode(blanksToReplace[blankIter].Datatype));

				//std::cout << "replace const size" << triples[tripleIter].Subject.Id << ":" << triples[tripleIter].Subject.Type << " " << triples[tripleIter].Predicate.Id << ":" << triples[tripleIter].Predicate.Type << " " << triples[tripleIter].Object.Id << ":" << triples[tripleIter].Object.Type << " with: " << constantSizeNode.getNodeId().Id << std::endl;
				newTriples.push_back(Triple(triples[tripleIter].Subject, triples[tripleIter].Predicate, constantSizeNode.getNodeId()));

				finalList.push_back(BinaryLiteralDescriptor(constantSizeNode.getNodeId(), blanksToReplace[blankIter].Guid, blanksToReplace[blankIter].DatasourceOffset, blanksToReplace[blankIter].ConstantLiteral, blanksToReplace[blankIter].Datatype));
			}
			else
			{
				DynamicSizeDatatype* dynamicSizeDatatype = (DynamicSizeDatatype*)graph->getDatatype(blanksToReplace[blankIter].Datatype);
				assert(dynamicSizeDatatype != NULL);

				auto getFunction = std::bind(&BinaryBlobDeserializer::getDynamicSizeData, binaryBlobDeserializer, std::placeholders::_1, blanksToReplace[blankIter].Guid);
				auto getSizeFunction = std::bind(&BinaryBlobDeserializer::getDataSize, binaryBlobDeserializer, blanksToReplace[blankIter].Guid);
				auto dynamicSizeNode = graph->createDynamicSizeLiteralNode(getFunction, getSizeFunction, graph->getUriNode(blanksToReplace[blankIter].Datatype));

				//std::cout << "replace dynamic" << triples[tripleIter].Subject.Id << ":" << triples[tripleIter].Subject.Type << " " << triples[tripleIter].Predicate.Id << ":" << triples[tripleIter].Predicate.Type << " " << triples[tripleIter].Object.Id << ":" << triples[tripleIter].Object.Type << " with: " << dynamicSizeNode.getNodeId().Id << std::endl;
				newTriples.push_back(Triple(triples[tripleIter].Subject, triples[tripleIter].Predicate, dynamicSizeNode.getNodeId()));

				finalList.push_back(BinaryLiteralDescriptor(dynamicSizeNode.getNodeId(), blanksToReplace[blankIter].Guid, blanksToReplace[blankIter].DatasourceOffset, blanksToReplace[blankIter].ConstantLiteral, blanksToReplace[blankIter].Datatype));
			}
			usageIter++;
		}
		else if (literalIndex < blanksToReplace.size())
		{

			// ignore triple
			if (triples[tripleIter].Subject == blanksToReplace[literalIndex].LiteralNodeId)
				continue;
			else
				newTriples.push_back(triples[tripleIter]); // unaffected triple
		}
		else
			newTriples.push_back(triples[tripleIter]);

	}

	

	graph->setTriples(newTriples);

	binaryBlobDeserializer->setInitialBlob(blob, finalList, graph);

	mBinaryGuidNode = NodeId();
	mBinaryOffsetNode = NodeId();
	mBinaryConstNode = NodeId();
	mRDFTypeNode = NodeId();

}

void FastRDF::StructureDeserializer::findLiteralBlankNodes(const Graph* graph, BinaryLiteralDescriptorList &blanksToReplace)
{
	const Graph::TripleContainer& triples = graph->getTriple();
	for (uint32 tripleIter = 0; tripleIter < triples.size(); ++tripleIter)
	{
		if (triples[tripleIter].Predicate == mBinaryGuidNode && triples[tripleIter].Subject.Type == NodeId::NODE_BLANK)
		{
			auto guidObjectNode = graph->getConstantLiteralNode(triples[tripleIter].Object);
			Guid guid = *((uint32*)guidObjectNode.getData());


			// search offset and const flag - triples are sorted by subject
			uint32 offset = 0;
			bool constFlag = false;
			NodeId binaryType;
			bool foundOffset = false;
			bool foundConst = false;
			bool foundBinaryType = false;
			// forward
			for (uint32 offsetIter = tripleIter; offsetIter < triples.size() && triples[offsetIter].Subject == triples[tripleIter].Subject && !(foundOffset && foundConst && foundBinaryType); ++offsetIter)
			{
				if (triples[offsetIter].Predicate == mBinaryOffsetNode)
				{
					foundOffset = true;
					auto offsetObjectNode = graph->getConstantLiteralNode(triples[offsetIter].Object);
					offset = *((uint32*)offsetObjectNode.getData());
				}
				else if (triples[offsetIter].Predicate == mBinaryConstNode)
				{
					foundConst = true;
					auto constObjectNode = graph->getConstantLiteralNode(triples[offsetIter].Object);
					constFlag = *((bool*)constObjectNode.getData());
				}
				else if (triples[offsetIter].Predicate == mRDFTypeNode)
				{
					foundBinaryType = true;
					binaryType = triples[offsetIter].Object;
				}
			}
			//backward
			for (uint32 offsetIter = tripleIter; offsetIter >= 0 && triples[offsetIter].Subject == triples[tripleIter].Subject &&  !(foundOffset && foundConst && foundBinaryType); --offsetIter)
			{
				if (triples[offsetIter].Predicate == mBinaryOffsetNode)
				{
					foundOffset = true;
					auto offsetObjectNode = graph->getConstantLiteralNode(triples[offsetIter].Object);
					offset = *((uint32*)offsetObjectNode.getData());
				}
				else if (triples[offsetIter].Predicate == mBinaryConstNode)
				{
					foundConst = true;
					auto constObjectNode = graph->getConstantLiteralNode(triples[offsetIter].Object);
					constFlag = *((bool*)constObjectNode.getData());
				}
				else if (triples[offsetIter].Predicate == mRDFTypeNode)
				{
					foundBinaryType = true;
					binaryType = triples[offsetIter].Object;
				}
			}
			assert(!binaryType.isInvalid());

			BinaryLiteralDescriptor literal(triples[tripleIter].Subject, guid, offset, constFlag, binaryType);
			blanksToReplace.push_back(literal);
		}
	}
}

