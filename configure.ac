AC_INIT([ansvif], [1.6], [marshall@i.code.pwnz.org])
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AC_TYPE_SIZE_T
AC_PROG_RANLIB
#AC_CONFIG_HEADERS([include/gzstream/gzstream.h])
AC_HEADER_STDC
#AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AX_CXX_COMPILE_STDCXX_11
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([ipstream])
AC_CHECK_HEADERS([lz])
AC_CHECK_HEADERS([crypto])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE([subdir-objects])
# This adds the option of compiling without using the ctemplate library,
# which has proved troublesome for compilation on some platforms
AC_ARG_ENABLE(syscalls,
[  --enable-syscalls       Enable Syscalls compiliation (experimental)],
[case "${enableval}" in
  yes) WITH_SYSCALLS=true ;;
  no)  WITH_SYSCALLS=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-syscalls) ;;
esac],[WITH_SYSCALLS=false])
AM_CONDITIONAL([WITH_SYSCALLS], [test "$WITH_SYSCALLS" = "true"])
# try to compile syscalls if they're enabled
if test "$WITH_SYSCALLS" = "true"; then
    AC_DEFINE([WITH_SYSCALLS], [], ["build syscalls module"])
    AC_MSG_NOTICE([syscalls enabled, we will try to compile them])
else
    AC_MSG_NOTICE([syscalls module is disabled])
fi
AM_CONDITIONAL(OS_IS_REDHAT, [ test `cat /etc/os-release | grep REDHAT | head -n 1 | wc -l` -eq "1"])
if test "$OS_IS_REDHAT" = "1"; then
    AC_DEFINE([__REDHAT], [], ["build for redhat"])
    AC_MSG_NOTICE([trying to build for a redhat based system])
fi
AC_OUTPUT
