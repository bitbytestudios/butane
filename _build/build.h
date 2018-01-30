// This is automatically included by unity build files.

#if defined(_WIN32) || defined(_WIN64)
  // We target Windows 7 and later.
  #define WINVER 0x0601
#endif

#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN

  // There's so much crap included by default. This should prune a lot.
  #define NOGDICAPMASKS
  #define NORASTEROPS
  #define NOCOLOR
  #define NODRAWTEXT
  #define NOMB
  #define NOMEMMGR
  #define NOMETAFILE
  #define NOMINMAX
  #define NOOPENFILE
  #define NOSCROLL
  #define NOSOUND
  #define NOCOMM
  #define NOKANJI
  #define NOHELP
  #define NODEFERWINDOWPOS
  #define NOMCX

  // Preemptively include `windows.h` and `windowsx.h` and unfuck afterwords,
  // because Microsoft litters the global namespace with so much crap that is
  // a nightmare to clean up as we go along.
  #include <windows.h>
  #include <windowsx.h>

  #undef ABSOLUTE
  #undef RELATIVE

  #undef NEAR
  #undef FAR

  #undef absolute
  #undef relative

  #undef near
  #undef far

  #undef ERROR

  #undef DELETE
#endif
