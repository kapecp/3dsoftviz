#ifndef LEAPEXPORT_H
#define LEAPEXPORT_H

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined( LEAPLIB_LIBRARY_STATIC )
#    define LEAPLIB_EXPORT
#  elif defined( LEAPLIB_LIBRARY )
#    define LEAPLIB_EXPORT   __declspec(dllexport)
#  else
#    define LEAPLIB_EXPORT   __declspec(dllimport)
#  endif
#else
#  define LEAPLIB_EXPORT
#endif

#endif // LEAPEXPORT_H
