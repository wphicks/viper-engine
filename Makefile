BUILDDIR=build
TESTDIR=test

CC=clang
CFLAGS=-g -Wall -std=c99 -O0 -pthread
PROFFLAGS=-pg -Wall -std=c99
INCFLAGS=-I. -Icontainers -Ithread
LDTESTFLAGS=-lgtest -lgtest_main -lpthread -lrt

CXX=clang++
CXXFLAGS=-g -Wall -std=c++14

TARGETS=test
TESTS=$(BUILDDIR)/vqueue_test $(BUILDDIR)/main_test

all : $(TARGETS)

all_test: test memtest

test: $(TESTS)
	for test_file in $(TESTS) ; do\
	    $$test_file  || exit 1; \
	done

memtest: $(TESTS)
	for test_file in $(TESTS) ; do\
	    valgrind --tool=memcheck $$test_file  || exit 1; \
	done

$(BUILDDIR)/main_test: $(BUILDDIR)/vqueue.o $(TESTDIR)/main.c
	$(CC) $(CFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o $(BUILDDIR)/vqueue.o $(TESTDIR)/main.c $(LDTESTFLAGS) -o $(BUILDDIR)/main_test

$(BUILDDIR)/vqueue_test: $(BUILDDIR)/vqueue.o $(TESTDIR)/vqueue_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o $(BUILDDIR)/vqueue.o $(TESTDIR)/vqueue_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/vqueue_test

$(BUILDDIR)/vqueue.o: containers/vqueue.h containers/vqueue.impl.c $(BUILDDIR)/vatomic.o
	$(CC) $(CFLAGS) $(INCFLAGS) $(BUILDDIR)/vatomic.o -c containers/vqueue.impl.c -o $(BUILDDIR)/vqueue.o

$(BUILDDIR)/vatomic.o: thread/vatomic.h thread/vatomic.linux.c
	$(CC) $(CFLAGS) $(INCFLAGS) -c thread/vatomic.linux.c -o $(BUILDDIR)/vatomic.o

clean:
	rm $(BUILDDIR)/*

