#ifndef GITEXPORT_H
#define GITEXPORT_H

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined( GITLIB_LIBRARY_STATIC )
#    define GITLIB_EXPORT
#  elif defined( GITLIB_LIBRARY )
#    define GITLIB_EXPORT   __declspec(dllexport)
#  else
#    define GITLIB_EXPORT   __declspec(dllimport)
#  endif
#else
#  define GITLIB_EXPORT
#endif

#endif // GITEXPORT_H
