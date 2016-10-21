#include <stdafx.h>
#include "NTripleDeserializer.h"


#include <Core/Graph.h>
#include <Core/Datatype.h>

using namespace FastRDF;



// TODO REFACTORING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void FastRDF::NTripleDeserializer::deserializeGraph(const String & ntripleText, Graph* graph)
{
	// todo find better heuristic
	uint32 predictedSize = ntripleText.length() / 80;

	std::vector<uint32> linestartIndices;
	linestartIndices.reserve(predictedSize);
	linestartIndices.push_back(0);

	for (uint32 pos = 0; pos < ntripleText.length() - 1; ++pos)
	{
		if (ntripleText[pos] == '\n')
			linestartIndices.push_back(pos + 1);
	}
	linestartIndices.push_back(ntripleText.length() - 1);

	if (linestartIndices.size() == 0)
		return;

	linestartIndices.reserve(linestartIndices.size());

	std::map<std::string, BlankNode> blankNodeMapping;

	for (uint32 linestartIndex = 0; linestartIndex < linestartIndices.size() - 1; ++linestartIndex)
	{
		TripleIndices tripleIndices;

		// subject
		uint32 end = linestartIndices[linestartIndex + 1];

		bool subjectBlank = false;
		bool objectUri = false;
		bool objectBlank = false;
		bool objectLiteral = false;


		for (uint32 subjectIter = linestartIndices[linestartIndex]; subjectIter < end; ++subjectIter)
		{
			char character = ntripleText[subjectIter];
			if (character == ' ' || character == '\t')
				continue;
			else if (character == '<')
				tripleIndices.SubjectStart = subjectIter;
			else if (character == '_')
			{
				assert(ntripleText[subjectIter + 1] == ':');
				tripleIndices.SubjectStart = subjectIter;
				subjectBlank = true;
			}
			else
				assert(false);// "invalid character sequence";

			break;
		}

		if (tripleIndices.SubjectStart == -1)
			continue;

		for (uint32 subjectIter = tripleIndices.SubjectStart + 1; subjectIter < end; ++subjectIter)
		{
			char character = ntripleText[subjectIter];
			if (!subjectBlank && character == '>')
				tripleIndices.SubjectEnd = subjectIter;
			else if (subjectBlank && (character == ' ' || character == '\t'))
				tripleIndices.SubjectEnd = subjectIter - 1;
			else
				continue;

			break;
		}

		assert(tripleIndices.SubjectEnd != -1);


		// predicate
		for (uint32 predicateIter = tripleIndices.SubjectEnd + 1; predicateIter < end; ++predicateIter)
		{
			char character = ntripleText[predicateIter];
			if (character == ' ' || character == '\t')
				continue;
			else if (character == '<')
				tripleIndices.PredicateStart = predicateIter;
			else
				assert(false);// , "invalid character sequence");

			break;
		}

		assert(tripleIndices.PredicateStart != -1);

		for (uint32 predicateIter = tripleIndices.PredicateStart + 1; predicateIter < end; ++predicateIter)
		{
			char character = ntripleText[predicateIter];
			if (character == '>')
				tripleIndices.PredicateEnd = predicateIter;
			else if (character == ' ' || character == '\t')
				assert(false); // invalid whitespace in uri
			else continue;

			break;
		}

		assert(tripleIndices.PredicateStart != -1);


		// object
		for (uint32 objectIter = tripleIndices.PredicateEnd + 1; objectIter < end; ++objectIter)
		{
			char character = ntripleText[objectIter];
			if (character == ' ' || character == '\t')
				continue;
			else if (character == '<')
			{
				tripleIndices.ObjectStart = objectIter;
				objectUri = true;
			}
			else if (character == '_')
			{
				assert(ntripleText[objectIter + 1] == ':');
				tripleIndices.ObjectStart = objectIter;
				objectBlank = true;
			}
			else if (character == '\"')
			{
				tripleIndices.ObjectStart = objectIter;
				objectLiteral = true;
			}
			else
				assert(false);// , "invalid character sequence");

			break;
		}
		assert(tripleIndices.ObjectStart != -1);
		assert(objectUri || objectBlank || objectLiteral);

		if (objectUri)
		{
			for (uint32 objectIter = tripleIndices.ObjectStart + 1; objectIter < end; ++objectIter)
			{
				char character = ntripleText[objectIter];
				if (character == ' ' || character == '\t')
					assert(false);// , "invalid character sequence");
				else if (character == '>')
				{
					tripleIndices.ObjectEnd = objectIter;
				}
				else
					continue;

				break;
			}
		}
		else if (objectBlank)
		{
			for (uint32 objectIter = tripleIndices.ObjectStart + 1; objectIter < end; ++objectIter)
			{
				char character = ntripleText[objectIter];
				if (character == ' ' || character == '\t' || character == '.')
				{
					tripleIndices.ObjectEnd = objectIter - 1;
				}
				else
					continue;

				break;
			}
		}
		else if (objectLiteral)
		{
			for (uint32 objectIter = tripleIndices.ObjectStart + 1; objectIter < end; ++objectIter)
			{
				char character = ntripleText[objectIter];
				if (character == '\"')
				{
					tripleIndices.ObjectEnd = objectIter;
				}
				else
					continue;

				break;
			}
		}
		assert(tripleIndices.ObjectEnd != -1);

		// find datatype
		if (objectLiteral)
		{
			for (uint32 datatypeIter = tripleIndices.ObjectEnd + 1; datatypeIter < end - 2; ++datatypeIter)
			{
				char character = ntripleText[datatypeIter];
				if (character == ' ' || character == '\t')
					continue;

				if (character == '^' && ntripleText[datatypeIter + 1] == '^')
				{
					if (ntripleText[datatypeIter + 2] == '<')
						tripleIndices.DatatypeStart = datatypeIter + 2;
					else
						assert(ntripleText[datatypeIter + 2] == '\"');

					break;
				}
			}

			if (tripleIndices.DatatypeStart)
			{

				for (uint32 datatypeIter = tripleIndices.DatatypeStart + 2; datatypeIter < end; ++datatypeIter)
				{
					char character = ntripleText[datatypeIter];
					if ( character == '>')
					{
						tripleIndices.DatatypeEnd = datatypeIter;
						break;
					}
					assert(character != ' ' && character != '\t');
				}
				assert(tripleIndices.DatatypeEnd != -1);
			}
		}

		const auto& predicate = graph->createUriNode(ntripleText.substr(tripleIndices.PredicateStart + 1, tripleIndices.PredicateEnd - tripleIndices.PredicateStart - 1));

		if (subjectBlank)
		{
			String subjectBlankNodeText = ntripleText.substr(tripleIndices.SubjectStart + 2, tripleIndices.SubjectEnd - tripleIndices.SubjectStart - 1);
			const auto& subjectBlankNodeMappingIter = blankNodeMapping.find(subjectBlankNodeText);
			BlankNode subject;
			if (subjectBlankNodeMappingIter == blankNodeMapping.end())
			{
				subject = graph->createBlankNode();
				blankNodeMapping[subjectBlankNodeText] = subject;
			}
			else
				subject = subjectBlankNodeMappingIter->second;




			if (objectUri)
			{
				auto object = graph->createUriNode(ntripleText.substr(tripleIndices.ObjectStart + 1, tripleIndices.ObjectEnd - tripleIndices.ObjectStart - 1));
				graph->addTriple(subject, predicate, object);
			}
			else if (objectBlank)
			{
				String objectBlankNodeText = ntripleText.substr(tripleIndices.ObjectStart + 2, tripleIndices.ObjectEnd - tripleIndices.ObjectStart - 1);
				const auto& objectBlankNodeMappingIter = blankNodeMapping.find(objectBlankNodeText);
				BlankNode object;
				if (objectBlankNodeMappingIter == blankNodeMapping.end())
				{
					object = graph->createBlankNode();
					blankNodeMapping[objectBlankNodeText] = object;
				}
				else
					object = objectBlankNodeMappingIter->second;

				graph->addTriple(subject, predicate, object);
			}
			else if (objectLiteral)
			{
				ConstantLiteralNode object = createLiteralNode(graph, ntripleText, tripleIndices);

				graph->addTriple(subject, predicate, object);
			}
		}
		else
		{
			String subjectString = ntripleText.substr(tripleIndices.SubjectStart + 1, tripleIndices.SubjectEnd - tripleIndices.SubjectStart - 1);
			auto subject = graph->createUriNode(subjectString);

			if (objectUri)
			{
				String objectString = ntripleText.substr(tripleIndices.ObjectStart + 1, tripleIndices.ObjectEnd - tripleIndices.ObjectStart - 1);
				auto object = graph->createUriNode(objectString);
				graph->addTriple(subject, predicate, object);
			}
			else if (objectBlank)
			{
				String blankNodeText = ntripleText.substr(tripleIndices.ObjectStart + 2, tripleIndices.ObjectEnd - tripleIndices.ObjectStart - 1);
				const auto& blankNodeMappingIter = blankNodeMapping.find(blankNodeText);
				BlankNode object;
				if (blankNodeMappingIter == blankNodeMapping.end())
				{
					object = graph->createBlankNode();
					blankNodeMapping[blankNodeText] = object;
				}
				else
					object = blankNodeMappingIter->second;
				graph->addTriple(subject, predicate, object);
			}
			else if (objectLiteral)
			{
				ConstantLiteralNode object = createLiteralNode(graph, ntripleText, tripleIndices);

				graph->addTriple(subject, predicate, object);
			}
		}
	}
	
}

ConstantLiteralNode NTripleDeserializer::createLiteralNode(Graph* graph, const String& text, const TripleIndices& tripleIndices)
{
	UriNode datatypeNode = graph->findUriNode(text.substr(tripleIndices.DatatypeStart + 1, tripleIndices.DatatypeEnd - tripleIndices.DatatypeStart - 1));
	assert(!datatypeNode.getNodeId().isInvalid());

	const Datatype* datatype = graph->getDatatype(datatypeNode);
	assert(datatype->isConstantSize());

	uint32 dataByteSize = ((ConstantSizeDatatype*)datatype)->ByteSize;
	void* data = new uint08[dataByteSize];

	auto deserializerFunction = ((ConstantSizeDatatype*)datatype)->FromString;

	uint32 objectStartIndex = tripleIndices.ObjectStart + 1;
	uint32 objectLength = tripleIndices.ObjectEnd - tripleIndices.ObjectStart - 1;
	std::string objectString = text.substr(objectStartIndex, objectLength);
	bool success = deserializerFunction(objectString, data);
	assert(success);

	ConstantLiteralNode object = graph->createConstantLiteralNode(data, dataByteSize, datatypeNode);

	return object;
}
