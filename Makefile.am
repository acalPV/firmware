SUBDIRS = \
	common \
	hal \
	parser

noinst_PROGRAMS =

bin_PROGRAMS = \
	server \
	mcu \
	mem

server_SOURCES = \
	server.c
server_LDADD = \
	common/libcommon.la \
	hal/libhal.la \
	hal/drivers/mmap/libmmap.la \
	hal/drivers/udp/libudp.la \
	hal/drivers/uart/libuart.la \
	parser/libparser.la

mcu_SOURCES = \
	mcu.c
mcu_LDADD = \
	common/libcommon.la \
	hal/libhal.la \
	hal/drivers/mmap/libmmap.la \
	hal/drivers/uart/libuart.la \
	hal/drivers/udp/libudp.la

mem_SOURCES = \
	mem.c
mem_LDADD = \
	common/libcommon.la \
	hal/libhal.la \
	hal/drivers/mmap/libmmap.la

if HAVE_GTEST
	
noinst_PROGRAMS += \
	test_acal

test_acal_SOURCES = \
	test_acal.cpp

test_acal_CPPFLAGS = \
	@GTEST_CPPFLAGS@ \
	-O0 -g
test_acal_LDFLAGS = \
	@GTEST_LDFLAGS@
test_acal_LDADD = \
	@GTEST_LIBS@ \
	hal/libhal.la \
	hal/drivers/uart/libuart.la \
	hal/drivers/mmap/libmmap.la \
	hal/drivers/udp/libudp.la

check-local:
	for i in $(noinst_PROGRAMS); do \
		LD_LIBRARY_PATH=/home/acal/Downloads/googletest-install/usr/local/lib \
			./$$i; \
	done

endif

