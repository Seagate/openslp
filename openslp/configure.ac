#
## Process this file with autoconf to produce a configure script
#
AC_PREREQ(2.59)
AC_INIT([OpenSLP], [2.0.0], [openslp-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([slpd/slpd_main.c])
AC_CONFIG_MACRO_DIR([m4])

#
# This is enabled for pthread UNIX98 extensions
AC_USE_SYSTEM_EXTENSIONS

#
# Checks for tools (programs)
#
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_LEX
AC_PROG_YACC
AM_PROG_CC_C_O

#
# Checks for libraries
#
AC_CHECK_LIB([resolv], [inet_aton])
AC_CHECK_LIB([socket], [socket])
AC_CHECK_LIB([nsl], [gethostbyname])
AC_CHECK_LIB([m], [log10])
AC_CHECK_LIB([pthread], [pthread_create])

#
# Checks for header files
#
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_STAT
AC_CHECK_HEADERS([unistd.h stdio.h stdlib.h stddef.h stdarg.h stdint.h inttypes.h ctype.h string.h strings.h memory.h math.h limits.h errno.h signal.h fcntl.h pthread.h arpa/inet.h netdb.h sys/types.h sys/time.h sys/socket.h pwd.h grp.h])

#
# Checks for types
#
AC_TYPE_SIGNAL
OPENSLP_CHECK_TYPE([socklen_t], [int],
	[defined to size_t if <sys/socket.h> does not support socklen_t data type])
OPENSLP_STRUCT_SA_RESTORER

#
# Checks for structure members
#
AC_CHECK_MEMBER([struct sockaddr_storage.ss_family],
	[AC_DEFINE(HAVE_SOCKADDR_STORAGE_SS_FAMILY,1,[whether sockaddr_storage 
has ss_family])],
        [], [#include <sys/socket.h>])

AC_CHECK_MEMBER([struct sockaddr_in.sin_len],
	[AC_DEFINE(HAVE_SOCKADDR_IN_SIN_LEN,1,[whether sockaddr_in has sin_len])],
	[], [#include <netinet/in.h>])

AC_CHECK_MEMBER([struct sockaddr_storage.ss_len],
	[AC_DEFINE(HAVE_SOCKADDR_STORAGE_SS_LEN,1,[whether sockaddr_storage has ss_len])],
	[], [#include <netinet/in.h>])

#
# Checks for compiler characteristics
#
AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN

#
# Checks for library functions
#
AC_REPLACE_FNMATCH
AC_FUNC_FORK
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strchr memcpy strcasecmp strdup strtol strerror isascii alarm gethostname gettimeofday select socket poll])
AC_CHECK_FUNCS([pthread_mutexattr_settype pthread_mutexattr_setkind_np])

#
# SLPv1 support - default is ON
#   ON means ENABLE_SLPv1 is defined to 1
#
AC_ARG_ENABLE([slpv1], 
  [AS_HELP_STRING([--disable-slpv1], [Turn off SLPv1 support @<:@default is ON@:>@])],
  [case "${enableval}" in
    yes) slpv1=yes ;;
    no)  slpv1=no  ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --disable-slpv1]) ;;
  esac], [slpv1=yes])
AM_CONDITIONAL([ENABLE_SLPv1], [test "${slpv1}" = yes])
if test "${slpv1}" = yes; then
  AC_DEFINE([ENABLE_SLPv1], 1, [Defined for SLPv1 support])
fi

#
# Predicate support - default is ON
#   ON means ENABLE_PREDICATES is defined to 1
#
AC_ARG_ENABLE([predicates], 
  AS_HELP_STRING([--disable-predicates], [Turn off predicates @<:@default is ON@:>@]),
  [case "${enableval}" in
    yes) predicates=yes ;;
    no)  predicates=no  ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --disable-predicates]) ;;
  esac], [predicates=yes])
AM_CONDITIONAL([ENABLE_PREDICATES], [test "x${predicates}" = xyes])
if test "x${predicates}" = xyes; then
  AC_DEFINE([ENABLE_PREDICATES], 1, [Defined for predicate support])
fi

#
# Asyncronous API support - default is OFF
#   ON means ENABLE_ASYNC_API is defined to 1
#
AC_ARG_ENABLE([async-api],
  AS_HELP_STRING([--enable-async-api], [Turn on async API support @<:@default is OFF@:>@]),
  [case "${enableval}" in
    yes) asyncapi=yes ;;
    no)  asyncapi=no  ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-async-api]) ;;
  esac], [asyncapi=no])
AM_CONDITIONAL([ENABLE_ASYNC_API], [test "x${asyncapi}" = xyes])
if test "x${asyncapi}" = xyes; then
  AC_DEFINE([ENABLE_ASYNC_API], 1, [Defined for async API support])
fi

#
# SLPv2 Security features support - default is OFF
#   ON means ENABLE_SLPv2_SECURITY is defined
#
AC_ARG_ENABLE(slpv2-security,
  AS_HELP_STRING([--enable-slpv2-security], [Turn on security support @<:@default is OFF@:>@]),
  [case "${enableval}" in
    yes) slpv2security=yes ;;
    no)  slpv2security=no  ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-slpv2-security]) ;;
  esac], [slpv2security=no])
if test "x${slpv2security}" = xyes; then
  AC_CHECK_LIB([crypto], [DSA_sign], [],
    [echo "------------------------------------------------------------"
     echo " The OpenSSL crypto library is required for SLPv2 security. "
     echo " Continuing without SLPv2 security functionality.           "
     echo "------------------------------------------------------------"
     slpv2security=no], [-ldl -lz])
  AC_CHECK_LIB([z], [inflate], [],
    [echo "--------------------------------------------------------------"
     echo " The OpenSSL crypto library requires zlib for SLPv2 security. "
     echo " Continuing without SLPv2 security functionality.             "
     echo "--------------------------------------------------------------"
     slpv2security=no])
  AC_CHECK_LIB([dl], [dlopen], [],
    [echo "------------------------------------------------------------"
     echo " The OpenSSL crypto library requires dl for SLPv2 security. "
     echo " Continuing without SLPv2 security functionality.           "
     echo "------------------------------------------------------------"
     slpv2security=no])
fi
AM_CONDITIONAL([ENABLE_SLPv2_SECURITY], [test "x${slpv2security}" = xyes])
if test "x${slpv2security}" = xyes; then
  AC_DEFINE([ENABLE_SLPv2_SECURITY], 1, [Defined for SLPv2 security support])
fi

#
# Generic DEBUG support - default is OFF
#   ON means with symbols, no optimization, DEBUG is defined to 1
#   OFF means with symbols, full optimization, NDEBUG is defined to 1
#
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Turn on debugging @<:@default is OFF@:>@]),
  [case "${enableval}" in
    yes) debug=yes ;;
    no)  debug=no  ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
  esac], [debug=no])
AM_CONDITIONAL([DEBUG], [test "x${debug}" = xyes])
AM_CONDITIONAL([NDEBUG], [test "x${debug}" = xno])
if test "x${debug}" = xyes; then
  AC_DEFINE([DEBUG], 1, [Defined for debug builds])
else
  AC_DEFINE([NDEBUG], 1, [Defined for non-debug builds])
fi

#
# System type macros used in OpenSLP source code
#
case "${host_os}" in
  *linux*)   CPPFLAGS="$CPPFLAGS -DLINUX"   ;;
  *solaris*) CPPFLAGS="$CPPFLAGS -DSOLARIS" ;;
  *aix*)     CPPFLAGS="$CPPFLAGS -DAIX"     ;;
  *hpux*)    CPPFLAGS="$CPPFLAGS -DHPUX"    ;;
  *darwin*)  CPPFLAGS="$CPPFLAGS -DDARWIN"  ;;
esac

# Determine if building for sparc and if so set cpu to v9
if test "x${host_cpu}" = xsparc; then
  GCC_OS_FLAGS="-mcpu=v9"
fi

#
# Set C/C++ compiler-specific warning/optimization/option flags
#
if test "x${GCC}" = xyes; then
  # GCC: turn on all warnings, DON'T set warnings as errors
  #   disable optimization for debug builds
  #   enable high-level optimizations for non-debug builds
  CFLAGS="$CFLAGS -Wall $GCC_OS_FLAGS"
  if test "x${debug}" = xyes; then 
    OPTFLAGS="-O0"
  else
    OPTFLAGS="-O2"
  fi
elif $CC -V 2>&1 | grep "WorkShop Compilers"; then
  # Solaris Workshop: enable optimizations for debug builds
  if test "x${debug}" = xno; then
    OPTFLAGS="-fast"
    if echo $CC | grep "xarch=v9"; then
      # "-fast" sets -xarch=v8 disabling 64-bit mode, enable it again
      OPTFLAGS="$OPTFLAGS -xarch=v9"
    fi
  fi
elif echo ${host_os} | grep -i "osf" >/dev/null; then
  # OSF: enable standard C (shouldn't be necessary)
  #   enable optimizations for non-debug builds
  CFLAGS="$CFLAGS -std"
  if test "x${debug}" = xno; then
    OPTFLAGS="-O"
  fi
else
  # everything else: take a wild guess (shouldn't need this)
  if test "x${debug}" = xno; then
    OPTFLAGS="-O"
  fi
fi
CFLAGS="$CFLAGS $OPTFLAGS"

#
# Configuration and output
#
AC_CONFIG_FILES([Makefile
		 common/Makefile
		 libslpattr/Makefile
		 libslp/Makefile
		 slpd/Makefile
		 slptool/Makefile
		 etc/Makefile
		 doc/Makefile
		 doc/doxyfile
		 obs/Makefile
		 obs/debian.changelog
		 obs/debian.control
		 obs/debian.rules
		 obs/openslp.dsc
		 obs/openslp.spec])
AC_OUTPUT

echo \
"------------------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION} Configuration:

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

 Package features:
   SLPv1 Messages: ${slpv1}
   SLPv2 Security: ${slpv2security}
   SLP Async API:  ${asyncapi}
   SLP Predicates: ${predicates}
   Debugging:      ${debug}

 Now type 'make @<:@prefix=<user-prefix>@:>@ @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries (default).
     docs               - build doxygen documentation.
     install            - install all binaries and data.

   additionally, on rpm-based systems only:
     rpmcheck           - check whether rpms may be built.
     srcrpm             - build only source rpm.
     rpms               - build source and binary rpms.

------------------------------------------------------------"
