SUBDIRS = lexical
bin_PROGRAMS = kavac

kavac_SOURCES = main.c \
                kavac.c

kavac_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/include
kavac_LDADD = $(top_srcdir)/src/shared/libshared.a \
	      $(srcdir)/lexical/liblexical.a
