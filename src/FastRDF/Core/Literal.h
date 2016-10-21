#pragma once

namespace FastRDF
{
	struct Literal
	{
		Literal()
			: TimeStamp(-1), Guid(-1)
		{}
		uint64 TimeStamp;
		Guid Guid;
	};
}
