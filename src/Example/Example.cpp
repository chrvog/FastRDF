#include "stdafx.h"
#include "../FastRDF/DefaultIncludes.h"
#include "../FastRDF/Typedefs.h"
#include "../FastRDF/Core/Uri.h"
#include "../FastRDF/Core/Graph.h"
#include "../FastRDF/Core/TemplateBinding.h"
#include "../FastRDF/Core/NativeClassGraphTemplate.h"
#include "../FastRDF/Serializer/NTripleSerializer.h"
#include "../FastRDF/Serializer/NTripleDeserializer.h"
#include "../FastRDF/Serializer/StructureSerializer.h"
#include "../FastRDF/Serializer/StructureDeserializer.h"
#include "../FastRDF/Serializer/BinaryBlobSerializer.h"
#include "../FastRDF/Serializer/BinaryBlobDeserializer.h"
#include "../FastRDF/Serializer/RaptorSerializer.h"
#include "../FastRDF/Serializer/RaptorDeserializer.h"

#include "Pose.h"

using namespace FastRDF;
using namespace std;


// forward declaration for functions
void constructGraphForPose(Graph* graph, Pose pose, UriNode poseNode);
void registerNamespaceToGraph(Graph* graph);


int main()
{
	// create pose 
	Pose pose;
	pose.mRotation.x = 0.0f;
	pose.mRotation.y = 1.0f;
	pose.mRotation.z = 2.0f;
	pose.mRotation.w = 3.0f;
	pose.mTranslation.x = -10.0f;
	pose.mTranslation.y = -20.0f;
	pose.mTranslation.z = -30.0f;


	// create rdf dataset (collection with multiple rdf graphs) with initial timestamp
	Dataset dataset(0);
	// create a base node for a graph
	auto poseNode = dataset.createUriNode("http://localhost:123/pose");

	// create the graph
	Graph* graph = dataset.createGraph(poseNode);

	registerNamespaceToGraph(graph);

	constructGraphForPose(graph, pose, poseNode);

	//serialize graph and binary payload
	StructureSerializer* serializer = new StructureSerializer(&dataset);
	BinaryLiteralDescriptorList literalDescriptorList;
	std::vector<uint32> datasourceGuids;
	String result = serializer->serializeStructure(graph, literalDescriptorList, datasourceGuids);
	BinarySerializer::Blob blob = BinarySerializer::serializePayload(graph, literalDescriptorList, datasourceGuids, 0, NULL);

	// -> do whatever you want with the graph
	
	//// parse graph
	// create a new dataset 
	Dataset dataset2(0);
	// create graph with base uri
	auto poseNode2 = dataset2.createUriNode("http://localhost:123/pose");
	Graph* graph2 = dataset2.createGraph(poseNode2);
	// deserialize graph
	StructureDeserializer deserializer;
	BinaryBlobDeserializer binaryBlobDeserializer;
	deserializer.deserializeGraph(result, graph2, blob, &binaryBlobDeserializer);

	// -> read data 

	
    return 0;
}

void registerNamespaceToGraph(Graph* graph)
{
	graph->addNamespace(ConstantString("math"), Uri("http://vocab.arvida.de/2015/06/maths/vocab"));
	graph->addNamespace(ConstantString("spatial"), Uri("http://vocab.arvida.de/2015/06/spatial/vocab"));
	graph->addNamespace(ConstantString("tracking"), Uri("http://vocab.arvida.de/2015/06/tracking/vocab"));
	graph->addNamespace(ConstantString("vom"), Uri("http://vocab.arvida.de/2015/06/vom/vocab"));
	graph->addNamespace(ConstantString("core"), Uri("http://vocab.arvida.de/2015/06/core/vocab"));
}

void constructGraphForPose(Graph* graph, Pose pose, UriNode poseNode)
{
	// create required nodes
	auto floatTypeNode = graph->createUriNode("xsd", "float");
	auto unsignedIntegerTypeNode = graph->createUriNode("xsd", "unsignedInt");

	auto rdfTypePredicate = graph->createUriNode("rdf", "type");
	auto rotationPredicate = graph->createUriNode("spatial", "rotation");
	auto rotation3D = graph->createUriNode("spatial", "Rotation3D");
	auto quantityValuePredicate = graph->createUriNode("spatial", "quantityValue");
	auto xPredicate = graph->createUriNode("math", "x");
	auto yPredicate = graph->createUriNode("math", "y");
	auto zPredicate = graph->createUriNode("math", "z");
	auto wPredicate = graph->createUriNode("math", "w");
	auto quaternion = graph->createUriNode("math", "Quaternion");
	auto sourceCSPredicate = graph->createUriNode("spatial", "sourceCoordinateSystem");
	auto targetCSPredicate = graph->createUriNode("spatial", "targetCoordinateSystem");
	auto coordinateSystem = graph->createUriNode("math", "LeftHandedCartesianCoordinateSystem3D");
	auto translationPredicate = graph->createUriNode("spatial", "translation");
	auto vector3d = graph->createUriNode("math", "Vector3D");
	auto unitPredicate = graph->createUriNode("vom", "unit");
	auto meter = graph->createUriNode("vom", "meter");
	auto translation3d = graph->createUriNode("spatial", "Translation3D");
	auto spatialRelationship = graph->createUriNode("spatial", "SpatialRelationship");

	auto rotationBN = graph->createBlankNode();
	auto rotationValueBN = graph->createBlankNode();
	auto sourceCSBN = graph->createBlankNode();
	auto targetCSBN = graph->createBlankNode();
	auto translationBN = graph->createBlankNode();
	auto translationValueBN = graph->createBlankNode();

	// create callbacks for rotation
	std::function<float()> xRotfunc = std::bind(&Pose::Quaternion::getX, &pose.mRotation);
	std::function<float()> yRotfunc = std::bind(&Pose::Quaternion::getY, &pose.mRotation);
	std::function<float()> zRotfunc = std::bind(&Pose::Quaternion::getZ, &pose.mRotation);
	std::function<float()> wRotfunc = std::bind(&Pose::Quaternion::getW, &pose.mRotation);

	// create literalnodes that uses callbacks
	ConstantSizeLiteralNode xRotLiteralNode = graph->getDataset()->createConstantSizeLiteralNode(xRotfunc, floatTypeNode);
	ConstantSizeLiteralNode yRotLiteralNode = graph->getDataset()->createConstantSizeLiteralNode(yRotfunc, floatTypeNode);
	ConstantSizeLiteralNode zRotLiteralNode = graph->getDataset()->createConstantSizeLiteralNode(zRotfunc, floatTypeNode);
	ConstantSizeLiteralNode wRotLiteralNode = graph->getDataset()->createConstantSizeLiteralNode(wRotfunc, floatTypeNode);

	//create literals the points to a relative memory address
	uint32 byteSize = (((uint64)(&pose.mTranslation)) - ((uint64)&pose.mRotation)) + sizeof(Pose::Translation);
	const Datasource& datasource = graph->getDataset()->createDatasource(poseNode.getNodeId(), byteSize, std::bind(&Pose::getTranslationAndRotation, &pose, std::placeholders::_1));

	RelativeConstantSizeLiteralNode xPosLiteralNode = graph->getDataset()->createRelativeLiteralNode(sizeof(float), ((uint64)(&pose.mTranslation.x)) - ((uint64)(&pose.mRotation)), floatTypeNode, datasource);
	RelativeConstantSizeLiteralNode yPosLiteralNode = graph->getDataset()->createRelativeLiteralNode(sizeof(float), ((uint64)(&pose.mTranslation.y)) - ((uint64)(&pose.mRotation)), floatTypeNode, datasource);
	RelativeConstantSizeLiteralNode zPosLiteralNode = graph->getDataset()->createRelativeLiteralNode(sizeof(float), ((uint64)(&pose.mTranslation.z)) - ((uint64)(&pose.mRotation)), floatTypeNode, datasource);
	


	//add more triples to the graph
	graph->addTriple(poseNode, rotationPredicate, rotationBN);
	graph->addTriple(rotationBN, quantityValuePredicate, rotationValueBN);
	graph->addTriple(rotationBN, rdfTypePredicate, rotation3D);
	graph->addTriple(rotationValueBN, xPredicate, xRotLiteralNode);
	graph->addTriple(rotationValueBN, yPredicate, yRotLiteralNode);
	graph->addTriple(rotationValueBN, zPredicate, zRotLiteralNode);
	graph->addTriple(rotationValueBN, wPredicate, wRotLiteralNode);
	graph->addTriple(rotationValueBN, rdfTypePredicate, quaternion);

	graph->addTriple(poseNode, sourceCSPredicate, sourceCSBN);
	graph->addTriple(sourceCSBN, rdfTypePredicate, coordinateSystem);

	graph->addTriple(poseNode, targetCSPredicate, targetCSBN);
	graph->addTriple(targetCSBN, rdfTypePredicate, coordinateSystem);

	graph->addTriple(poseNode, translationPredicate, translationBN);
	graph->addTriple(translationBN, rdfTypePredicate, translation3d);
	graph->addTriple(translationBN, unitPredicate, meter);
	graph->addTriple(translationBN, quantityValuePredicate, translationValueBN);

	graph->addTriple(translationValueBN, xPredicate, xPosLiteralNode);
	graph->addTriple(translationValueBN, yPredicate, yPosLiteralNode);
	graph->addTriple(translationValueBN, zPredicate, zPosLiteralNode);
}
