#pragma once

#include "ReferentNode.h"


namespace FastRDF
{
	class Dataset;
	class Uri;

	class UriNode : public ReferentNode
	{
		friend class Dataset;
	public:
		UriNode();
		const Uri& getUri() const;


		UriNode(const UriNode& other);
		UriNode& operator=(const UriNode& other);


	private:
		UriNode(Dataset* dataset, NodeId nodeId);
	};
}