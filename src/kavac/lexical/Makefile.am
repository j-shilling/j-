noinst_LIBRARIES = liblexical.a
check_PROGRAMS = check_utf8_stream
TESTS = $(check_PROGRAMS)

liblexical_a_SOURCES = utf8-stream.c
liblexical_a_CFLAGS = -I$(top_srcdir)/src/include

check_utf8_stream_SOURCES = utf8-stream-test.c
check_utf8_stream_CFLAGS = -I$(top_srcdir)/src/include
check_utf8_stream_LDADD = liblexical.a @LIBUNISTRING@
