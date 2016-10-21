#pragma once

#include "Path.h"

namespace FastRDF
{
	class Uri
	{
	public:
		struct Query
		{
			Query(const ConstantString& key, const ConstantString& value);

			ConstantString mKey;
			ConstantString mValue;
		};

		Uri();

		Uri(const String& uriString);

		bool append(const String& relativeUri);

		bool isRelative() const
		{
			return mRelative;
		}

		const ConstantString& toString() const;

		bool operator<(const Uri& other) const;

		bool operator==(const Uri& other) const;

		String getPath() const
		{
			return mStringUri.get().substr(mPath.Start, mPath.Length);
		}

	private:
		struct StringReference
		{
			uint16 Start;
			uint16 Length;

			StringReference(uint16 start, uint16 length)
				: Start(start), Length(length)
			{}

			StringReference()
				: Start(MAX_UINT16), Length(0)
			{}

			bool isInvalid() const
			{
				return Start == MAX_UINT16 || Length == 0;
			}
		};
		
		bool mRelative;

		StringReference	mScheme;
		StringReference mAuthority;
		
		StringReference mPath;

		StringReference mQuery;
		StringReference mFragments;

		ConstantString mStringUri;

		void setNewUriString(const String& uriString);

		uint32 processRelativeUriPart(uint32 index, const String &uriString);

		void setNewRelativeUri(const String& uriString);
	};
}
