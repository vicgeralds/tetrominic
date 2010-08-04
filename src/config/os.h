#if defined(WIN32) || defined(__WIN32__)
#undef WIN32
#define WIN32 1
#define WIN32_LEAN_AND_MEAN

#elif defined(MSDOS) || defined(__MSDOS__) || defined(__PACIFIC__)
#undef MSDOS
#define MSDOS 1

#else
#undef UNIX
#define UNIX 1
#endif
