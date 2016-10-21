#pragma once

#include "ReferentNode.h"


namespace FastRDF
{
	class Dataset;
	class Uri;

	class UriPlaceholderNode : public ReferentNode
	{
		friend class Dataset;
	public:
		const String& getPlaceholderName() const;

		UriPlaceholderNode(const UriPlaceholderNode& other);

		UriPlaceholderNode& operator=(const UriPlaceholderNode& other);


	private:
		String mPlaceholderName;
		UriPlaceholderNode(String placeholderName, Dataset* dataset, NodeId nodeId);
	};
}