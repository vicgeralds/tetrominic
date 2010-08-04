#if __DJGPP__ || __TURBOC__
#undef HAVE_CONIO_H
#undef HAVE_GETTEXTINFO
#define HAVE_CONIO_H 1
#define HAVE_GETTEXTINFO 1
#endif
