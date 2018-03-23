SUBDIRS = lexical \
          util

bin_PROGRAMS = kavac

kavac_SOURCES = main.c \
                kavac.c

kavac_CFLAGS = -I$(srcdir)/include -I$(top_srcdir)/src/include
kavac_LDADD = $(srcdir)/lexical/liblexical.a
