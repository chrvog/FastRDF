#pragma once


#if defined (FASTRDF_DLL) || defined (FASTRDF_DLL_EXPORT)
#ifdef FASTRDF_DLL_EXPORT
#define FASTRDF_API   __declspec(dllexport)
#else // outside DLL
#define FASTRDF_API   __declspec(dllimport)
#endif  // XYZLIBRARY_EXPORT
#else
#define FASTRDF_API
#endif