#pragma once

#include <Core/NodeId.h>

namespace FastRDF
{
	struct Triple
	{
		NodeId Subject;
		NodeId Predicate;
		NodeId Object;

		Triple(NodeId subject, NodeId predicate, NodeId object)
			: Subject(subject), Predicate(predicate), Object(object)
		{}

		bool operator<(const Triple& other) const
		{
			if (Subject < other.Subject)
				return true;
			else if (Subject == other.Subject)
			{
				if (Predicate < other.Predicate)
					return true;
				else if (Predicate == other.Predicate)
					return Object < other.Object;
				return false;
			}
			return false;
		}

		bool operator==(const Triple& other) const {
			return (Subject == other.Subject) && (Predicate == other.Predicate) && (Object == other.Object);
		}
	};
}