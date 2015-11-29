#ifndef MOUSE3DEXPORT_H
#define MOUSE3DEXPORT_H

//macro for creating cpp libraries

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined( MOUSE3DLIB_LIBRARY_STATIC )
#    define MOUSE3DLIBEXPORT_H
#  elif defined( MOUSE3DLIB_LIBRARY )
#    define MOUSE3DLIBEXPORT_H   __declspec(dllexport)
#  else
#    define MOUSE3DLIBEXPORT_H   __declspec(dllimport)
#  endif
#else
#  define MOUSE3DLIBEXPORT_H
#endif

#endif // MOUSE3DEXPORT_H
