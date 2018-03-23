noinst_LIBRARIES = liblexical.a
check_PROGRAMS = check_file_chunker
TESTS = $(check_PROGRAMS)

liblexical_a_SOURCES = file-chunker.c
liblexical_a_CFLAGS = -I$(top_srcdir)/src/include \
                      -I$(top_srcdir)/src/kavac/include

check_file_chunker_SOURCES = file-chunker-test.c

check_file_chunker_CFLAGS = -I$(top_srcdir)/src/include \
		            -I$(top_srcdir)/src/kavac/include
check_file_chunker_LDADD = liblexical.a \
			   $(top_srcdir)/src/kavac/util/libutil.a
