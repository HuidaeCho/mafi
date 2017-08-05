AC_INIT(README)
#AC_PREFIX_DEFAULT(~)
#AC_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST
AC_PROG_CC

my_includes="/usr/include /usr/share/include /usr/local/include"
my_libs="/usr/lib /usr/share/lib /usr/local/lib /usr/lib64 /usr/share/lib64 /usr/local/lib64"


GD=yes
# Use GD library?
AC_ARG_WITH(gd,
	[ --with-gd			Use GD library],
	[
		case "$withval" in
		"n" | "no")
			GD=
		;;
		esac
	])

# GD include directory
AC_ARG_WITH(gd-includes,
	[ --with-gd-includes=DIR	Specify include directory for GD],
	[
		case "$withval" in
		"" | "y" | "ye" | "yes" | "n" | "no")
			AC_MSG_ERROR([*** You must supply a directory to --with-gd-includes.])
		;;
		esac
		gd_includes="$withval"
	])

# GD library directory
AC_ARG_WITH(gd-libs,
	[ --with-gd-libs=DIR		Specify library directory for GD],
	[
		case "$withval" in
		"" | "y" | "ye" | "yes" | "n" | "no")
			AC_MSG_ERROR([*** You must supply a directory to --with-gd-libs.])
		;;
		esac
		gd_libs="$withval"
	])


################################################################################
# GD library
################################################################################

if test "$GD"; then
AC_MSG_CHECKING(for GD include directory)
my_gd_includes="$my_includes /usr/local/include /usr/local/include/gd /usr/local/gd/include"
GD_INCLUDES=
if test "$gd_includes"; then
	for dir in $gd_includes; do
		if test -d "$dir"; then
			if test -f "$dir/gd.h"; then
				GD_INCLUDES="$GD_INCLUDES -I$dir"
#				break
			elif test -d "$dir/gd" && test -f "$dir/gd/gd.h"; then
				GD_INCLUDES="$GD_INCLUDES -I$dir/gd"
#				break
			fi
		else
			AC_MSG_WARN([*** GD include directory $dir does not exist.])
		fi
	done
else
#	for dir in $my_includes $my_gd_includes; do
	for dir in $my_gd_includes; do
		if test -d "$dir" && test -f "$dir/gd.h"; then
			GD_INCLUDES="$GD_INCLUDES -I$dir"
#			break
		elif test -d "$dir/gd" && test -f "$dir/gd/gd.h"; then
			GD_INCLUDES="$GD_INCLUDES -I$dir/gd"
#			break
		fi
	done
fi
AC_MSG_RESULT($GD_INCLUDES)
AC_SUBST(GD_INCLUDES)


AC_MSG_CHECKING(for GD library directory)
my_gd_libs="$my_libs /usr/local/lib /usr/local/lib/gd /usr/local/gd/lib"
GD_LIBS=
if test "$gd_libs"; then
	for dir in $gd_libs; do
		if test -d "$dir"; then
			if test -f "$dir/libgd.so"; then
				GD_LIBS="$GD_LIBS -L$dir"
#				break
			elif test -d "$dir/gd" && test -f "$dir/gd/libgd.so"; then
				GD_LIBS="$GD_LIBS -L$dir/gd"
#				break
			fi
		else
			AC_MSG_WARN([*** GD library directory $dir does not exist.])
		fi
	done
else
#	for dir in $my_libs $my_gd_libs; do
	for dir in $my_gd_libs; do
		if test -d "$dir" && test -f "$dir/libgd.so"; then
			GD_LIBS="$GD_LIBS -L$dir"
#			break
		elif test -d "$dir/gd" && test -f "$dir/gd/libgd.so"; then
			GD_LIBS="$GD_LIBS -L$dir/gd"
#			break
		fi
	done
fi
AC_MSG_RESULT($GD_LIBS)
if test "$GD_LIBS"; then
	GD_LIBS="$GD_LIBS -lgd"
fi
AC_SUBST(GD_LIBS)
fi


USE_GD="#"
if test "$GD" && test "$GD_INCLUDES" && test "$GD_LIBS" ; then
	USE_GD=
fi
AC_SUBST(USE_GD)


AC_OUTPUT(Makefile)
