# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstmysrc.la libgstmysink.la libgstmytransform.la
##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
libgstmysrc_la_SOURCES = gstmysrc.c gstmysrc.h
libgstmysink_la_SOURCES = gstmysink.c gstmysink.h
libgstmytransform_la_SOURCES = gstmytransform.c gstmytransform.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstmysrc_la_CFLAGS = $(GST_CFLAGS)
libgstmysrc_la_LIBADD = $(GST_LIBS)
libgstmysrc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmysrc_la_LIBTOOLFLAGS = --tag=disable-static

libgstmysink_la_CFLAGS = $(GST_CFLAGS)
libgstmysink_la_LIBADD = $(GST_LIBS)
libgstmysink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmysink_la_LIBTOOLFLAGS = --tag=disable-static

libgstmytransform_la_CFLAGS = $(GST_CFLAGS)
libgstmytransform_la_LIBADD = $(GST_LIBS)
libgstmytransform_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmytransform_la_LIBTOOLFLAGS = --tag=disable-static


# headers we need but don't want installed
noinst_HEADERS = gstmysrc.h gstmysink.h gstmytransform.h
