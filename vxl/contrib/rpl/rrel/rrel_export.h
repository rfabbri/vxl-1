
#ifndef RREL_EXPORT_H
#define RREL_EXPORT_H

#ifdef RREL_STATIC_DEFINE
#  define RREL_EXPORT
#  define RREL_NO_EXPORT
#else
#  ifndef RREL_EXPORT
#    ifdef rrel_EXPORTS
        /* We are building this library */
#      define RREL_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define RREL_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef RREL_NO_EXPORT
#    define RREL_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef RREL_DEPRECATED
#  define RREL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef RREL_DEPRECATED_EXPORT
#  define RREL_DEPRECATED_EXPORT RREL_EXPORT RREL_DEPRECATED
#endif

#ifndef RREL_DEPRECATED_NO_EXPORT
#  define RREL_DEPRECATED_NO_EXPORT RREL_NO_EXPORT RREL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RREL_NO_DEPRECATED
#    define RREL_NO_DEPRECATED
#  endif
#endif

#endif /* RREL_EXPORT_H */
