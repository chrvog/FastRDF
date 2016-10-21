#include "stdafx.h"
#include "NTripleSerializer.h"


#include <boost/lexical_cast.hpp>

#include <Core/Datatype.h>
#include <Core/TemplateBinding.h>

using namespace FastRDF;

#define INPLACE_SIZE 256


std::string FastRDF::NTripleSerializer::serializeGraph(const Graph* graph, TemplateBinding* binding)
{
	std::vector<Triple> dynamicTriples;
	std::vector<Triple> constantSizeTriples;
	std::vector<Triple> constantTriples;
	std::vector<Triple> templateTriples;
	const Graph::TripleContainer& triples = graph->getTriple();

	constantSizeTriples.reserve(triples.size()/2);
	constantTriples.reserve(triples.size());
	dynamicTriples.reserve(5);

	splitTriples(graph, constantTriples, constantSizeTriples, dynamicTriples,templateTriples);
	
	uint32 size = calculateStringLength(graph, constantTriples, constantSizeTriples, dynamicTriples, templateTriples, binding);

	std::vector<String> dynamicLiteralValueStrings;
	dynamicLiteralValueStrings.reserve(dynamicTriples.size());

	createDynamicValueStrings(dynamicLiteralValueStrings,dynamicTriples, graph,binding);

	return buildString(size,graph,constantTriples,constantSizeTriples,dynamicTriples,dynamicLiteralValueStrings, templateTriples,binding);
}

void FastRDF::NTripleSerializer::splitTriples(const Graph* graph, std::vector<Triple>& constantTriples, std::vector<Triple>& constantSizeTriples, std::vector<Triple>& dynamicTriples, std::vector<Triple>& templateTriples)
{
	for (const auto& triple : graph->getTriple())
	{
		if (triple.Subject.Type == NodeId::NODE_URI_PLACEHOLDER || triple.Predicate.Type == NodeId::NODE_URI_PLACEHOLDER)
			templateTriples.push_back(triple);
		else if (triple.Object.Type == NodeId::NODE_DYNAMIC_LITERAL)
			dynamicTriples.push_back(triple);
		else if (triple.Object.Type == NodeId::NODE_CONSTANT_SIZE_LITERAL || triple.Object.Type == NodeId::NODE_RELATIVE_CONSTANT_SIZE_LITERAL)
			constantSizeTriples.push_back(triple);
		else if (triple.Object.Type == NodeId::NODE_CONSTANT_LITERAL || triple.Object.Type == NodeId::NODE_URI || triple.Object.Type == NodeId::NODE_BLANK)
			constantTriples.push_back(triple);
	}
}

uint32 FastRDF::NTripleSerializer::calculateStringLength(const Graph* graph, const std::vector<Triple>& constantTriples, const std::vector<Triple>& constantSizeTriples, const std::vector<Triple>& dynamicTriples, const std::vector<Triple>& templateTriples, TemplateBinding* binding)
{
	uint32 size = 0;
	// always add a newline
	for (const auto& triple : constantTriples)
		size += calculateConstantTripleLength(triple, graph) + 1;
	for (const auto& triple : constantSizeTriples)
		size += calculateConstantSizeTripleLength(triple, graph) + 1;
	for (const auto& triple : dynamicTriples)
		size += buildDynamicTripleCache(triple,graph,binding);
	for (const auto& triple : templateTriples)
		size += calculateTemplateTripleLength(triple, graph,binding);

	return size;
}


String FastRDF::NTripleSerializer::buildString(uint32 stringSize, const Graph* graph, const std::vector<Triple>& constantTriples, const std::vector<Triple>& constantSizeTriples, const std::vector<Triple>& dynamicTriples, const std::vector<String>& dynamicLiteralStrings, const std::vector<Triple>& templateTriples, TemplateBinding* binding)
{
	String returnValue;
	returnValue.reserve(stringSize);

	// always add a newline
	for (const auto& triple : constantTriples)
	{
		returnValue += mConstantTripleCache[triple];
	}

	char defaultMemory[INPLACE_SIZE];
	// always add a newline
	for (const auto& triple : constantSizeTriples)
	{
		NumericalTripleCacheEntry&  cacheEntry = mNumericalTripleCache[triple];

		uint32 literalStartIndex = returnValue.length() + cacheEntry.LiteralStartIndex;
		returnValue += cacheEntry.Text;

		ConstantSizeLiteralNode literalNode = graph->getConstantSizeLiteralNode(triple.Object);

		uint32 valueSize = literalNode.getByteSize();
		// use inplace for small values to reduce memory allocations
		void* value = &defaultMemory;
		if (valueSize  > INPLACE_SIZE)
			value = new char[valueSize];

		literalNode.getData(value,binding);

		String literalValue = cacheEntry.Datatype->ToString(value, cacheEntry.Datatype->ByteSize);
		returnValue.replace(literalStartIndex, literalValue.length(), literalValue.c_str());

		if (valueSize  > INPLACE_SIZE)
			delete[] static_cast<char*>(value);
	}

	for (const auto& triple : dynamicTriples)
	{
		DynamicTripleCacheEntry& cacheEntry = mDynamicTripleCache[triple];

		returnValue += cacheEntry.ConstantPart;
		uint32 valueByteSize = cacheEntry.SizeFunction(binding);

		// use inplace for small values to reduce memory allocations
		void* value = &defaultMemory;
		if (valueByteSize  > INPLACE_SIZE)
			value = new char[valueByteSize];

		cacheEntry.GetDataFunction(value, binding);
		returnValue += "\"";
		returnValue += cacheEntry.Datatype->ToString(value, valueByteSize);
		returnValue += "^^<";
		returnValue += cacheEntry.Datatype->DatatypeUri.toString();
		returnValue += "> .\n";

		if (valueByteSize  > INPLACE_SIZE)
			delete[] value;

	}

	for (const auto& triple : templateTriples)
	{
		switch (triple.Subject.Type)
		{
			case NodeId::NODE_URI:
			case NodeId::NODE_BLANK:
			{
				const auto& subjectIter = mNodeToStringCache.find(triple.Subject);
				assert(subjectIter != mNodeToStringCache.end());
				returnValue += subjectIter->second;
			}
			break;
			case NodeId::NODE_URI_PLACEHOLDER:
			{
				String placeHolderName = graph->getDataset()->getUriPlaceholderName(triple.Subject);

				const auto& uri = binding->UriBindings.find(placeHolderName);
				assert(uri != binding->UriBindings.end());
				returnValue += "<";
				returnValue += uri->second.toString();
				returnValue += ">";
			}
			break;
			default:
				assert(false);

		}
		returnValue += " ";

		switch (triple.Predicate.Type)
		{
		case NodeId::NODE_URI:
		{
			const auto& predicatetIter = mNodeToStringCache.find(triple.Predicate);
			assert(predicatetIter != mNodeToStringCache.end());
			returnValue += predicatetIter->second;
		}
		break;
		case NodeId::NODE_URI_PLACEHOLDER:
		{
			String placeHolderName = graph->getDataset()->getUriPlaceholderName(triple.Predicate);

			const auto& uri = binding->UriBindings.find(placeHolderName);
			assert(uri != binding->UriBindings.end());
			returnValue += "<";
			returnValue += uri->second.toString();
			returnValue += ">";
		}
		break;
		default:
			assert(false);
		}

		returnValue += " ";

		switch (triple.Object.Type)
		{
		case NodeId::NODE_URI:
		case NodeId::NODE_BLANK:
		case NodeId::NODE_CONSTANT_LITERAL:
		{
			const auto& objectIter = mNodeToStringCache.find(triple.Object);
			assert(objectIter != mNodeToStringCache.end());
			returnValue += objectIter->second;
		}
		break;
		case NodeId::NODE_URI_PLACEHOLDER:
		{
			String placeHolderName = graph->getDataset()->getUriPlaceholderName(triple.Object);

			const auto& uri = binding->UriBindings.find(placeHolderName);
			assert(uri != binding->UriBindings.end());
			returnValue += "<";
			returnValue += uri->second.toString();
			returnValue += ">";
		}
		break;
		case NodeId::NODE_CONSTANT_SIZE_LITERAL:
		{
			ConstantSizeLiteralNode literalNode = graph->getConstantSizeLiteralNode(triple.Object);

			uint32 valueByteSize = literalNode.getByteSize();
			// use inplace for small values to reduce memory allocations
			void* value = &defaultMemory;
			if (valueByteSize  > INPLACE_SIZE)
				value = new char[valueByteSize];

			literalNode.getData(value,binding);

			const ConstantSizeDatatype* datatype = (const ConstantSizeDatatype*)literalNode.getLiteralDatatype();
			String literalValue = datatype->ToString(value, datatype->ByteSize);
			if (valueByteSize > INPLACE_SIZE)
				delete[] value;

			returnValue += literalValue;
		}
		break;
		case NodeId::NODE_DYNAMIC_LITERAL:
		{
			DynamicLiteralNode literalNode = graph->getDynamicLiteralNode(triple.Object);
			const DynamicSizeDatatype* datatype = (const DynamicSizeDatatype*)literalNode.getLiteralDatatype();

			uint32 valueByteSize = literalNode.getByteSize(binding);
			// use inplace for small values to reduce memory allocations
			void* value = &defaultMemory;
			if (valueByteSize > INPLACE_SIZE)
				value = new char[valueByteSize];

			literalNode.getData(value, binding);
			returnValue += "\"";
			returnValue += datatype->ToString(value, valueByteSize);
			returnValue += "^^<";
			returnValue += datatype->DatatypeUri.toString();
			returnValue += "> .\n";

			if (valueByteSize > INPLACE_SIZE)
				delete[] value;
		}
			break;
		default:
			assert(false);
		}

		returnValue += ";\n";

	}

	return returnValue;
}

uint32 FastRDF::NTripleSerializer::calculateConstantTripleLength(const Triple & triple, const Graph* graph)
{
	const auto& iter = mConstantTripleCache.find(triple);

	if (iter == mConstantTripleCache.end())
		return createConstantTripleCacheEntry(triple, graph).get().length();
	
	return iter->second.get().length();
}

uint32 FastRDF::NTripleSerializer::calculateConstantSizeTripleLength(const Triple & triple, const Graph* graph)
{
	const auto& iter = mNumericalTripleCache.find(triple);

	if (iter == mNumericalTripleCache.end())
		return createConstantSizeTripleCacheEntry(triple, graph).get().length();

	return  iter->second.Text.get().length();
}


uint32 FastRDF::NTripleSerializer::calculateTemplateTripleLength(const Triple & triple, const Graph* graph, TemplateBinding* binding)
{
	uint32 size = 0;
	if (triple.Subject.Type == NodeId::NODE_URI_PLACEHOLDER)
	{
		String name = graph->getDataset()->getUriPlaceholderName(triple.Subject);
		std::map<String, Uri>::iterator iter = binding->UriBindings.find(name);
		assert(iter != binding->UriBindings.end());
		size += iter->second.toString().get().length() + 3; // <uri>whitespace
	}
	else
	{
		ConstantString subject;

		const auto& subjectIter = mNodeToStringCache.find(triple.Subject);
		if (subjectIter == mNodeToStringCache.end())
			subject = createConstantNodeString(triple.Subject, graph);
		else
			subject = subjectIter->second;
		size += subject.get().length();
	}
	if (triple.Predicate.Type == NodeId::NODE_URI_PLACEHOLDER)
	{
		String name = graph->getDataset()->getUriPlaceholderName(triple.Predicate);
		std::map<String, Uri>::iterator iter = binding->UriBindings.find(name);
		assert(iter != binding->UriBindings.end());
		size += iter->second.toString().get().length() + 3; // <uri>whitespace

	}
	else
	{
		ConstantString predicate;

		const auto& predicateIter = mNodeToStringCache.find(triple.Predicate);
		if (predicateIter == mNodeToStringCache.end())
			predicate = createConstantNodeString(triple.Predicate, graph);
		else
			predicate = predicateIter->second;
		size += predicate.get().length();
	}

	if (triple.Object.Type == NodeId::NODE_BLANK ||
		triple.Object.Type == NodeId::NODE_URI ||
		triple.Object.Type == NodeId::NODE_CONSTANT_LITERAL)
	{
		ConstantString object;

		const auto& objectIter = mNodeToStringCache.find(triple.Object);
		if (objectIter == mNodeToStringCache.end())
			object = createConstantNodeString(triple.Object, graph);
		else
			object = objectIter->second;
		size += object.get().length();
	}
	else if (triple.Object.Type == NodeId::NODE_CONSTANT_SIZE_LITERAL)
	{
		auto node = graph->getConstantSizeLiteralNode(triple.Object);
		NodeId literalTypeId = node.getLiteralDatatypeNode();
		const auto& literalTypeIter = mNodeToStringCache.find(literalTypeId);
		ConstantString literalTypeString;

		if (literalTypeIter == mNodeToStringCache.end())
			literalTypeString = createConstantNodeString(literalTypeId, graph).get();
		else
			literalTypeString = literalTypeIter->second.get();
		return literalTypeString.get().length() + 23; // 20 literal + semicolon. longlong: 19 digits with optional minus. see constant size cache creation
	}
	else if (triple.Object.Type == NodeId::NODE_DYNAMIC_LITERAL)
	{
		
	}

	return  size;
}




ConstantString FastRDF::NTripleSerializer::createConstantTripleCacheEntry(const Triple & triple, const Graph* graph)
{
	const auto& subjectIter = mNodeToStringCache.find(triple.Subject);
	const auto& predicateIter = mNodeToStringCache.find(triple.Predicate);
	const auto& objectIter = mNodeToStringCache.find(triple.Object);
	
	ConstantString subject;
	ConstantString predicate;
	ConstantString object;

	if (subjectIter == mNodeToStringCache.end())
		subject = createConstantNodeString(triple.Subject, graph);
	else
		subject = subjectIter->second;

	if (predicateIter == mNodeToStringCache.end())
		predicate = createConstantNodeString(triple.Predicate, graph);
	else
		predicate = predicateIter->second;

	if (objectIter == mNodeToStringCache.end())
		object = createConstantNodeString(triple.Object, graph);
	else
		object = objectIter->second;

	String tmp;

	tmp.reserve(subject.get().length() + predicate.get().length() + object.get().length() + 3);
	tmp = subject.get() + String(" ") + predicate.get() + String(" ") + object.get() + String(" .\n");
	
	const auto& returnValue = mConstantTripleCache.emplace(triple,ConstantString(tmp));
		
	return returnValue.first->second;
}

ConstantString FastRDF::NTripleSerializer::createConstantSizeTripleCacheEntry(const Triple & triple, const Graph* graph)
{
	auto node = graph->getConstantSizeLiteralNode(triple.Object);
	NodeId literalTypeId = node.getLiteralDatatypeNode();
	const auto& subjectIter = mNodeToStringCache.find(triple.Subject);
	const auto& predicateIter = mNodeToStringCache.find(triple.Predicate);
	const auto& objectIter = mNodeToStringCache.find(triple.Object);
	const auto& literalTypeIter = mNodeToStringCache.find(literalTypeId);

	ConstantString subjectString;
	ConstantString predicateString;
	ConstantString literalTypeString;

	if (subjectIter == mNodeToStringCache.end())
		subjectString = createConstantNodeString(triple.Subject, graph);
	else
		subjectString = subjectIter->second;

	if (predicateIter == mNodeToStringCache.end())
		predicateString = createConstantNodeString(triple.Predicate, graph);
	else
		predicateString = predicateIter->second;

	if (literalTypeIter == mNodeToStringCache.end())
		literalTypeString = createConstantNodeString(literalTypeId, graph).get();
	else
		literalTypeString = literalTypeIter->second.get();

	String tmp;

	uint32 objectStartIndex = subjectString.get().length() + predicateString.get().length() + 3;
	tmp.reserve(objectStartIndex + 23); // 20 literal + semicolon. longlong: 19 digits with optional minus
	//															  0 1234567890123456789012 3 	(including pre whitespace)		
	tmp = subjectString.get() + String(" ") + predicateString.get() + String(" \"                    \"") + String("^^") + literalTypeString.get() + String(" .\n");

	// create cache entry
	NumericalTripleCacheEntry cacheEntry;
	cacheEntry.Text	= ConstantString(tmp);
	cacheEntry.LiteralStartIndex = objectStartIndex;
	const Datatype* datatype = graph->getDatatype(literalTypeId);
	assert(datatype->isConstantSize());
	cacheEntry.Datatype = (ConstantSizeDatatype*)datatype;
	const auto& returnValue = mNumericalTripleCache.emplace(triple, cacheEntry);

	return returnValue.first->second.Text;
}

const ConstantString & FastRDF::NTripleSerializer::createConstantNodeString(NodeId nodeId, const Graph* graph)
{
	ConstantString value;
	switch (nodeId.Type)
	{
	case NodeId::NODE_BLANK:
		value = String("_:b") + boost::lexical_cast<String>(nodeId.Id);
		break;
	case NodeId::NODE_URI:
	{
		UriNode uriNode = graph->getUriNode(nodeId);
		const Uri& uri = uriNode.getUri();
		const ConstantString& uriCString = uri.toString();
		value = String("<") + uriCString.get() + String(">");
		break;
	}
	case NodeId::NODE_CONSTANT_LITERAL:
	{
		auto node = graph->getConstantLiteralNode(nodeId);
		NodeId literalDatatype = node .getLiteralDatatypeNode();
		uint32 size = node.getLiteralByteSize();
	
		const void* data = node.getData();
		// todo non constant size datatypes
		auto datatype = (ConstantSizeDatatype*)graph->getDatatype(literalDatatype);
		
		String localValue = String("\"");
		localValue += datatype->ToString(data,datatype->ByteSize);
		localValue += String("\"^^");

		const auto& literalTypeIter = mNodeToStringCache.find(literalDatatype);

		if (literalTypeIter == mNodeToStringCache.end())
			localValue += createConstantNodeString(literalDatatype, graph).get();
		else
			localValue += literalTypeIter->second.get();

		value = localValue;
			
		break;
	}
	default:
		// unknown node type
		assert(false);
	}
	const auto& iter = mNodeToStringCache.emplace(nodeId,value).first;

	return iter->second;
}


uint32 FastRDF::NTripleSerializer::buildDynamicTripleCache(const Triple& triple, const Graph* graph, TemplateBinding* binding)
{
	uint32 size = 0;
	std::map<Triple, NTripleSerializer::DynamicTripleCacheEntry>::iterator& iter = mDynamicTripleCache.find(triple);

	if (iter == mDynamicTripleCache.end())
		iter = createDynamicTripleCacheEntry(triple, graph);
	size = iter->second.ConstantSize;
	size += iter->second.SizeFunction(binding);

	return size;
}

std::map<Triple, NTripleSerializer::DynamicTripleCacheEntry>::iterator NTripleSerializer::createDynamicTripleCacheEntry(const Triple& triple, const Graph* graph)
{
	assert(triple.Object.Type == NodeId::NODE_DYNAMIC_LITERAL);

	DynamicTripleCacheEntry entry;


	const auto& subjectIter = mNodeToStringCache.find(triple.Subject);
	const auto& predicateIter = mNodeToStringCache.find(triple.Predicate);

	auto dynamicNode = graph->getDynamicLiteralNode(triple.Object);

	entry.GetDataFunction = dynamicNode.getGetLiteralDataFunction();
	entry.SizeFunction = dynamicNode.getGetLiteralSizeFunction();
	entry.Datatype = dynamicNode.getLiteralDatatype();

	String subject;
	ConstantString predicate;

	if (subjectIter == mNodeToStringCache.end())
		subject = createConstantNodeString(triple.Subject, graph).get();
	else
		subject = subjectIter->second;

	if (predicateIter == mNodeToStringCache.end())
		predicate = createConstantNodeString(triple.Predicate, graph);
	else
		predicate = predicateIter->second;

	if (triple.Subject.Type == NodeId::NODE_URI && triple.Predicate.Type == NodeId::NODE_URI) {
		entry.ConstantPart = subject + String(" ") + predicate.get() + String(" \"");
		// "constantpart "dynamic"^^<datatypeUri> ./n"
		//                1      2345           6789
		entry.ConstantSize = entry.ConstantPart.get().length() + entry.Datatype->DatatypeUri.toString().get().length() + 9;
	}
	else
		entry.ConstantSize = 0;

	return mDynamicTripleCache.emplace(triple, entry).first;
}

void NTripleSerializer::createDynamicValueStrings(std::vector<String>& valueStrings, const std::vector<Triple>& dynamicTriples, const Graph* graph, TemplateBinding* binding)
{
	valueStrings.resize(dynamicTriples.size());
	char defaultMemory[INPLACE_SIZE];
	
	for (uint32 tripleIter = 0; tripleIter < dynamicTriples.size(); ++tripleIter)
	{
		const Triple& triple = dynamicTriples[tripleIter];
		DynamicTripleCacheEntry&  cacheEntry = mDynamicTripleCache[triple];



		const DynamicSizeDatatype*  datatype = cacheEntry.Datatype;

		uint32 valueSize = cacheEntry.SizeFunction(binding);
		void* value = &defaultMemory;
		if (valueSize > INPLACE_SIZE)
			value = new char[valueSize];

		cacheEntry.GetDataFunction(value, binding);

		valueStrings[tripleIter] = cacheEntry.Datatype->ToString(value, valueSize);

		if (valueSize  > INPLACE_SIZE)
			delete[] value;
	}
}

