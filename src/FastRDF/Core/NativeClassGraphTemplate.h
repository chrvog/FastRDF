#pragma once

#include <core/Graph.h>
#include <core/Dataset.h>

namespace FastRDF {
	class UriPlaceholderNode;

	class NativeClassGraphTemplate : public Graph
	{
	public:
		friend class Dataset;
		
		typedef std::function<size_t()> GetLiteralByteSizeFunction;
		typedef std::function<void(void*)> GetLiteralDataFunction;

		virtual bool isTemplate() {
			return true;
		}


		template <typename OwnerType,typename LiteralType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(std::function<LiteralType(OwnerType*)>& getDataFunction, const UriNode& type)
		{
			std::function<void(void*, TemplateBinding*)>  getterBinding = [getDataFunction](void* value, TemplateBinding* binding) {
				LiteralType returnValue = getDataFunction((OwnerType*)(binding->mInstanceClass));
				*((LiteralType*)value) = returnValue;
			};
			return mDataset->createConstantSizeLiteralNode(sizeof(LiteralType), getterBinding, type);
		}

		template <typename OwnerType>
		ConstantSizeLiteralNode createConstantSizeLiteralNode(size_t size, std::function<void (OwnerType*,void*)>& getDataFunction, const UriNode& type)
		{
			std::function<void (void*,TemplateBinding*)>  getterBinding = [getDataFunction](void* value, TemplateBinding* binding) {
				getDataFunction((OwnerType*)(binding->mInstanceClass), value);
			};
			return mDataset->createConstantSizeLiteralNode(size, getterBinding, type);
		}

		

		//DynamicLiteralNode createDynamicSizeLiteralTemplate(const DynamicLiteralNode::GetLiteralDataFunction& getDataFunction, const DynamicLiteralNode::GetLiteralByteSizeFunction& getSizeFunction, const UriNode& type);



	protected:
		NativeClassGraphTemplate(Dataset* dataset, const ReferentNode& nameNode);

		virtual ~NativeClassGraphTemplate();
		
		std::map<String, uint32> mVariableUriNameMapping;
		std::vector<UriPlaceholderNode*> mPlaceHolderNodes;
	} ;

	

}