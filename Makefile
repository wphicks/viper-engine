# Global options
BASEBUILDDIR:=build
TESTDIR=test

CC=clang
CFLAGS=-Wall -std=c99 -pthread -g
INCFLAGS=-I. -Icontainers -Ithread
LDTESTFLAGS=-lgtest -lgtest_main -lpthread -lrt

CXX=clang++
CXXFLAGS=-Wall -std=c++14

# Configurable options
PROFFLAGS=-p
DEBUGFLAGS=-DDEBUG

GCC ?= 0
ifeq ($(GCC), 1)
    CC=gcc
    CXX=g++
    BASEBUILDDIR:=build/gcc
endif

OPT ?= 0
CFLAGS+=-O$(OPT)
BUILDDIR:=$(BASEBUILDDIR)/opt$(OPT)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS+=$(DEBUGFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/debug
endif

PROFILE ?= 0
ifeq ($(PROFILE), 1)
    CFLAGS+=$(PROFFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/profile
endif


# Targets
TARGETS=$(TESTS)
TESTS=$(BUILDDIR)/vqueue_test $(BUILDDIR)/main_test

.PHONY: all test memtest clean objdump init

all : $(TARGETS)

test: $(TESTS)
	for test_file in $(TESTS) ; do\
	    $$test_file  || exit 1; \
	done

memtest: $(TESTS)
	for test_file in $(TESTS) ; do\
	    valgrind --tool=memcheck $$test_file  || exit 1; \
	done

objdump: $(TARGETS)
	for obj_file in $(BUILDDIR)/*.o; do\
	    objdump -S $${obj_file} > $(BUILDDIR)/asm/$$(basename $$obj_file).asm; \
	done

init:
	for entry in debug opt0 opt1 opt2 opt3 profile; do\
	    mkdir -p $(BASEBUILDDIR)/$${entry}/asm; \
	    mkdir -p $(BASEBUILDDIR)/gcc/$${entry}/asm; \
	done

$(BUILDDIR)/main_test: $(BUILDDIR)/vqueue.o $(TESTDIR)/main.c
	$(CC) $(CFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o $(BUILDDIR)/vqueue.o $(TESTDIR)/main.c $(LDTESTFLAGS) -o $@

$(BUILDDIR)/vqueue_test: $(BUILDDIR)/vqueue.o $(TESTDIR)/vqueue_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o $(BUILDDIR)/vqueue.o $(TESTDIR)/vqueue_test.cpp $(LDTESTFLAGS) -o $@

$(BUILDDIR)/vqueue.o: containers/vqueue.h containers/vqueue.impl.c $(BUILDDIR)/vatomic.o
	$(CC) $(CFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o -c containers/vqueue.impl.c -o $@

$(BUILDDIR)/vatomic.o: thread/vatomic.h thread/vatomic.linux.c
	$(CC) $(CFLAGS) $(INCFLAGS) -c thread/vatomic.linux.c -o $@

clean:
	rm $(BUILDDIR)/*

