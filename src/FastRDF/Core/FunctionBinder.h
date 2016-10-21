#pragma once

namespace FastRDF {
	class Binding;

	struct GetBinderBase {
		enum CallerType {
			CALL_DIRECT,
			CALL_BINDING,
			CALL_OWNER,
		};

		virtual GetBinderBase* clone() = 0;
		virtual void callGetter(void* owner, void* value, Binding* binding) = 0;
	};
	template<typename OwnerType, GetBinderBase::CallerType>
	struct GetBinder : public BinderBase {
		virtual void callGetter(void* owner, void* value, Binding* binding) = 0;
	};

	template<typename OwnerType>
	struct GetBinder<OwnerType, GetBinderBase::CALL_DIRECT> {

		virtual GetBinderBase* clone()
		{
			auto* binder = new GetBinder<OwnerType, GetBinderBase::CALL_DIRECT>();
			binder->Getter = Getter;
		}
		virtual void callGetter(void* owner, void* value, Binding* binding)
		{
			Getter(value);
		}
		std::function<void(void*)> Getter;
	};

	template<typename OwnerType>
	struct GetBinder<OwnerType, GetBinderBase::CALL_BINDING> : public BinderBase {
		virtual GetBinderBase* clone()
		{
			auto* binder = new GetBinder<OwnerType, GetBinderBase::CALL_BINDING>();
			binder->Getter = Getter;
		}
		virtual void callGetter(void* owner, void* value, Binding* binding)
		{
			Getter((OwnerType*)binding->mOwner, value);
		}
		std::function<void(OwnerType*, void*)> Getter;
	};

	template<typename OwnerType>
	struct GetBinder<OwnerType, GetBinderBase::CALL_OWNER> : public BinderBase {
		virtual GetBinderBase* clone()
		{
			auto* binder = new GetBinder<OwnerType, GetBinderBase::CALL_OWNER>();
			binder->Getter = Getter;
		}

		virtual void callGetter(void* owner, void* value, Binding* binding)
		{
			Getter((OwnerType*)owner, value);
		}
		std::function<void(OwnerType*, void*)> Getter;
	};
}

