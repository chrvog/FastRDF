#pragma once

namespace FastRDF
{
	class Graph;

	class RaptorDeserializer
	{
	public:
		void deserializeGraph(const String& string, const Uri& baseUri, Graph* graph);
	private:
	};
}