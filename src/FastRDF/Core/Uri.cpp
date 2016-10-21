#include "stdafx.h"
#include "Uri.h"
#include <locale>

using namespace std;
using namespace FastRDF;


Uri::Query::Query(const ConstantString& key, const ConstantString& value)
: mKey(key), mValue(value)
{}


Uri::Uri()
: mRelative(true)
{}

FastRDF::Uri::Uri(const String & uriString)
{
	String temp = uriString;
	if (uriString.size() > 1 && uriString[uriString.size() - 1] == '/')
		temp.pop_back();

	setNewUriString(temp);
}



bool FastRDF::Uri::append(const String& relativeUri)
{
	if (relativeUri.size() == 0)
		return true;

	if (mPath.isInvalid())
		return false;

	// no checks
	String tmpRelative(relativeUri);

	if (tmpRelative[0] != '/')
		tmpRelative = String("/") + tmpRelative;

	if (tmpRelative[tmpRelative.size() - 1] == '/')
		tmpRelative.pop_back();


	String tmp = mStringUri.get();
	mStringUri = ConstantString(tmp.append(tmpRelative));
	return true;
}

void Uri::setNewUriString(const String& uriString)
{
	// See http://tools.ietf.org/html/rfc3986#section-3
	// URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
	//

	

	if (uriString.size() == 0)
		return; // empty uri

	if (uriString[0] == '/')
		setNewRelativeUri(uriString);
	
	// here
	//  |
	//  foo://example.com:8042/over/there?name=ferret#nose
	//	\_/ \________________/ \________/ \_________/ \__/
	//	 |          |              |            |       |
	// scheme     authority       path        query  fragment

	// S3.1: scheme ::= ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) 
	if (!isalpha(uriString[0]))
		return; //error

	mScheme.Start = 0;

	uint32 index = 1;

	// find end of scheme
	for (; index < uriString.size(); ++index)
	{
		auto character = uriString[index];

		if (isalpha(character) || isdigit(character))
			continue;

		else if (( character == '+' || character == '-' || character == '.') )
			continue;

		if (character == ':')
		{
			mScheme.Length = index;
			break;
		}
	}

	if (mScheme.isInvalid())
		return; // error

	//   here
	//     |
	//  foo://example.com:8042/over/there?name=ferret#nose
	//	\_/ \________________/ \________/ \_________/ \__/
	//	 |          |              |            |       |
	// scheme     authority       path        query  fragment

	if (uriString.size() < index +3)
		return; //error
	
	if (uriString[index + 1] != '/' || uriString[index + 2] != '/')
		return; // error


	index += 3;

	//      here
	//        |
	//  foo://example.com:8042/over/there?name=ferret#nose
	//	\_/ \________________/ \________/ \_________/ \__/
	//	 |          |              |            |       |
	// scheme     authority       path        query  fragment

	// S3.2: authority   = [ userinfo "@" ] host [ ":" port ] 
	//       userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
	//       host        = IP-literal / IPv4address / reg-name
	//       IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"
	//       IPvFuture   = "v" 1 * HEXDIG "." 1 * (unreserved / sub - delims / ":")

	//       IPv6address = 6( h16 ":" ) ls32
	//                   / "::" 5(h16 ":") ls32
	//                   / [h16] "::" 4(h16 ":") ls32
	//                   / [*1(h16 ":") h16] "::" 3(h16 ":") ls32
	//                   / [*2(h16 ":") h16] "::" 2(h16 ":") ls32
	//                   / [*3(h16 ":") h16] "::"    h16 ":"   ls32
	//                   / [*4(h16 ":") h16] "::"              ls32
	//                   / [*5(h16 ":") h16] "::"              h16
	//                   / [*6(h16 ":") h16] "::"
	//                   
	//                   ls32 = (h16 ":" h16) / IPv4address
	//                   ; least - significant 32 bits of address
	//                   
	//                   h16 = 1 * 4HEXDIG
	//                   ; 16 bits of address represented in hexadecimal
	
	//       IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
	//       
	//       dec - octet = DIGIT; 0 - 9
	//                   / %x31 - 39 DIGIT; 10 - 99
	//                   / "1" 2DIGIT; 100 - 199
	//                   / "2" %x30 - 34 DIGIT; 200 - 249
	//                   / "25" %x30 - 35; 250 - 255

	//       reg-name    = *( unreserved / pct-encoded / sub-delims )

	//       port        = *DIGIT

	//
	// summary: find next slash or end of string. ignore checks for performance
	//
	
	mAuthority.Start = index;

	for (; index < uriString.size(); ++index)
	{
		if (uriString[index] == '/')
		{
			
			break;
		}
	}
	processRelativeUriPart(index, uriString);


	mStringUri = ConstantString(uriString);
	
}


FastRDF::uint32 FastRDF::Uri::processRelativeUriPart(uint32 index, const String &uriString)
{
	mAuthority.Length = index - mAuthority.Start;

	if (index != uriString.length())
	{

		index += 1;

		//                       here
		//                         |
		//  foo://example.com:8042/over/there?name=ferret#nose
		//	\_/ \________________/ \________/ \_________/ \__/
		//	 |          |              |            |       |
		// scheme     authority       path        query  fragment

		// S3.3: The path is terminated by the first question mark ("?") or number sign ("#") character, or
		// by the end of the URI.

		// if path length is zero it's invalid anyways
		mPath.Start = index;

		bool queryFound = false;
		bool fragmentFound = false;

		for (; index < uriString.size(); ++index)
		{
			if (uriString[index] == '?')
			{
				queryFound = true;
				break;
			}
			else if (uriString[index] == '#')
			{
				fragmentFound = true;
				break;
			}
		}

		mPath.Length = index - mPath.Start;

		if (queryFound)
		{
			index += 1;
			//                                  here
			//                                    |
			//  foo://example.com:8042/over/there?name=ferret#nose
			//	\_/ \________________/ \________/ \_________/ \__/
			//	 |          |              |            |       |
			// scheme     authority       path        query  fragment

			// S3.4: The query component is indicated by the first question
			// mark ("?") character and terminated by a number sign ("#") character
			// or by the end of the URI.
			mQuery.Start = index;
			for (; index < uriString.size(); ++index)
			{
				if (uriString[index] == '#')
				{
					fragmentFound = true;
					break;
				}
			}

			mQuery.Length = index - mQuery.Start;
		}

		if (fragmentFound)
		{
			index += 1;
			//                                              here
			//                                                |
			//  foo://example.com:8042/over/there?name=ferret#nose
			//	\_/ \________________/ \________/ \_________/ \__/
			//	 |          |              |            |       |
			// scheme     authority       path        query  fragment

			// S3.5: A fragment identifier component is indicated by the presence of a
			// number sign("#") character and terminated by the end of the URI.
			mFragments.Start = index;
			mFragments.Length = (uint16)uriString.size() - index;

		}
	}	return index;
}

const ConstantString & FastRDF::Uri::toString() const
{
	return mStringUri;
}

bool FastRDF::Uri::operator<(const Uri & other) const
{
	return mStringUri.get() < other.mStringUri.get();
}

bool FastRDF::Uri::operator==(const Uri & other) const
{
	return mStringUri.get() == other.mStringUri.get();
}

void FastRDF::Uri::setNewRelativeUri(const String& uriString)
{
	processRelativeUriPart(0, uriString);
	mStringUri = ConstantString(uriString);
}
