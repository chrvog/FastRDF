#pragma once
#include <iostream>
#include <string>

#include <Typedefs.h>

struct Pose
{
	Pose()
		: mName("HappyPose")
	{}

	struct Translation
	{
		float x, y, z;

		void getX(void* value)
		{
			*((float*)value) = x;
		}

		
		void getY(void* value)
		{
			*((float*)value) = y;
		}

		void getZ(void* value)
		{
			*((float*)value) = z;
		}

	};

	struct Quaternion
	{
		float x, y, z, w;

		float getX()
		{
			return x;
		}

		float getY()
		{
			return y;
		}

		float getZ()
		{
			return z;
		}

		float getW()
		{
			return w;
		}
	};

	void getName(void* data) const
	{
		memcpy(data, &(mName[0]), mName.length());
	}

	size_t getNameByteSize() const
	{
		return mName.length();
	}

	void setName(const std::string& name)
	{
		mName = name;
	}

	void getTranslationAndRotation(void* data) const
	{
		memcpy(data, &mRotation, (((uint64_t)(&mTranslation)) - ((uint64_t)&mRotation)) + sizeof(Translation));
	}

	std::string mName;
	Quaternion mRotation;
	Translation mTranslation;
};