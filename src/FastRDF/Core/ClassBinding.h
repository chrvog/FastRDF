#pragma once

#include <boost/function.hpp>

namespace FastRDF {

	struct ValueGetterWrapperBase
	{
		virtual void callValueGetter(void* owner, void* value) = NULL;
	};


	struct SizeGetterWrapperBase
	{
		virtual uint32 callSizeGetter(void* owner) = NULL;
	};


	template<typename OwnerType>
	struct ValueGetter : public ValueGetterBase {
		std::function<void (OwnerType*, void*)> mGetterFunction;


		virtual void callValueGetter(void* owner, void* value)
		{
			mGetterFunction((OwnerType*)owner, value);
		}
	};

	template<typename OwnerType>
	struct SizeGetter : public SizeGetterWrapperBase {
		std::function<uint32 (OwnerType*)> mGetterFunction;


		virtual uint32 callValueGetter(void* owner, void* value)
		{
			return mGetterFunction((OwnerType*)owner);
		}
	};

}