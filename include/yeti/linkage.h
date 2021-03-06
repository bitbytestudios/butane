//===-- yeti/linkage.h ----------------------------------*- mode: C++11 -*-===//
//
//                             __ __     _   _
//                            |  |  |___| |_|_|
//                            |_   _| -_|  _| |
//                              |_| |___|_| |_|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines pre-processor macros that specify symbol visibility.
///
//===----------------------------------------------------------------------===//

#ifndef _YETI_LINKAGE_H_
#define _YETI_LINKAGE_H_

/// \def YETI_LINKAGE_INTERNAL
/// \brief Exports symbols for embedding.
#define YETI_LINKAGE_INTERNAL 0

/// \def YETI_LINKAGE_STATIC
/// \brief Exports symbols for static linking.
#define YETI_LINKAGE_STATIC 1

/// \def YETI_LINKAGE_DYNAMIC
/// \brief Exports symbols for dynamic linking.
#define YETI_LINKAGE_DYNAMIC 2

/// \def YETI_LINKAGE
/// \brief Specifies how you intend to link to Yeti.
#if !defined(YETI_LINKAGE)
  #error ("Please specify how you intend on linking to Yeti by defining `YETI_LINKAGE'.")
#endif

/// \def YETI_PUBLIC
/// \brief Marks a symbol for public usage.
#if defined(DOXYGEN)
  #define YETI_PUBLIC
#else
  #if YETI_LINKAGE == YETI_LINKAGE_STATIC
    #define YETI_PUBLIC
  #elif YETI_LINKAGE == YETI_LINKAGE_DYNAMIC
    #if defined(__YETI_IS_BEING_COMPILED__)
      #if defined(__GNUC__)
        #if __GNUC__ >= 4
          #define YETI_PUBLIC __attribute__ ((visibility ("default")))
        #else
          #define YETI_PUBLIC
        #endif
      #elif defined(__clang__)
        #define YETI_PUBLIC __attribute__ ((visibility ("default")))
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define YETI_PUBLIC __declspec(dllexport)
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #else
      #if (defined(__GNUC__) && (__GNUC__ >= 4))
        #define YETI_PUBLIC
      #elif defined(__clang__)
        #define YETI_PUBLIC
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define YETI_PUBLIC __declspec(dllimport)
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #endif
  #endif
#endif

/// \def YETI_PRIVATE
/// \brief Marks a symbol for internal usage.
#if defined(DOXYGEN)
  #define YETI_PRIVATE
#else
  #if YETI_LINKAGE == YETI_LINKAGE_STATIC
    #define YETI_PRIVATE
  #elif YETI_LINKAGE == YETI_LINKAGE_DYNAMIC
    #if defined(__YETI_IS_BEING_COMPILED__)
      #if defined(__GNUC__)
        #if __GNUC__ >= 4
          #define YETI_PRIVATE __attribute__ ((visibility ("hidden")))
        #else
          #define YETI_PRIVATE
        #endif
      #elif defined(__clang__)
        #define YETI_PRIVATE __attribute__ ((visibility ("hidden")))
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define YETI_PRIVATE
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #else
      #define YETI_PRIVATE
    #endif
  #endif
#endif

#if defined(DOXYGEN)
  #define YETI_BEGIN_EXTERN_C
  #define YETI_END_EXTERN_C
#else
  #if defined(__cplusplus)
    #define YETI_BEGIN_EXTERN_C extern "C" {
    #define YETI_END_EXTERN_C }
  #else
    #define YETI_BEGIN_EXTERN_C
    #define YETI_END_EXTERN_C
  #endif
#endif

#endif // _YETI_LINKAGE_H_
