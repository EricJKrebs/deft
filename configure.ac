dnl Process this file with autoconf to produce a configure script.
AC_INIT(deft, 0, roundyd@physics.oregonstate.edu, deft)
AC_CONFIG_SRCDIR(src/deft.cpp)
AM_INIT_AUTOMAKE([subdir-objects parallel-tests color-tests])
AM_SILENT_RULES([yes]) # disable this with make V=1
# use c++ as a compiler in the tests
AC_LANG(C++)
AM_CONFIG_HEADER(src/config.h)

dnl Set default CXX flags
if test "x$CXXFLAGS" = "x"; then
  CXXFLAGS='-ansi -pipe -W -Wall -O2'
fi

# check whether to enable profiling or coverage testing.
AC_ARG_ENABLE(profiling, [AC_HELP_STRING([--enable-profiling],
                         [use profiling])],
                  use_profiling=$enableval, use_profiling=no)
if test "x$use_profiling" = "xyes"; then
   CXXFLAGS="$CXXFLAGS -pg"
fi

AC_CHECK_LIB(profiler, clock,LIBS="$LIBS -lprofiler",
             [AC_MSG_WARN([could not find google perftools library, no profiling for you!])])

AC_CHECK_LIB(tcmalloc, clock,LIBS="$LIBS -ltcmalloc",
             [AC_MSG_WARN([could not find google tcmalloc library, no heap profiling for you!])])

AC_ARG_ENABLE(coverage, [AC_HELP_STRING([--enable-coverage],
                         [use coverage testing])],
                  use_coverage=$enableval, use_coverage=no)
if test "x$use_coverage" = "xyes"; then
   CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
fi
CPPFLAGS="$CPPFLAGS -Iinclude"

dnl Checks for programs.
AC_PROG_CXX

# and support large files...
AC_SYS_LARGEFILE

dnl Checks for libraries.

AC_CHECK_LIB(fftw3, fftw_plan_r2r_1d,LIBS="$LIBS -lfftw3",
             [AC_MSG_ERROR([could not find fftw3 library!])])

# Do we want threads?

AC_ARG_WITH(threads,
            [AC_HELP_STRING([--without-threads],
                            [disable pthreads parallelization])],
            with_threads=$withval, with_threads=maybe)


if test "x$with_threads" = "xno"; then
  AC_MSG_WARN([Not using pthreads...])
else
  AC_CHECK_LIB(pthread, pthread_join,
    [with_threads=yes
     LIBS="$LIBS -lpthread"
     AC_DEFINE(DFT_THREAD, 1,
               [Define to 1 if you want to use threads])],
    [if test "x$with_threads" = "xyes"; then
       AC_MSG_ERROR([Cannot find pthreads library!])
     fi]
    )
fi

dnl Checks for header files.
AC_CHECK_HEADER(Eigen/Core,,AC_MSG_ERROR([Cannot find eigen2 header!]))

AC_ARG_ENABLE(debug,
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
