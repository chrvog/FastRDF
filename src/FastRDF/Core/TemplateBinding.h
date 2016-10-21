#pragma once

#include <map>
#include <core/Uri.h>

namespace FastRDF {
	struct TemplateBinding {
		std::map<String, Uri> UriBindings;
		void* mInstanceClass;
	};
}