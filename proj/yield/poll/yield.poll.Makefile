TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/poll -name "*.d")


ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/poll/bsd/fd_event_queue.o
	O_FILE_PATHS += ../../../build/yield/poll/posix/fd_event_queue.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/poll/bsd/fd_event_queue.o
	O_FILE_PATHS += ../../../build/yield/poll/posix/fd_event_queue.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/poll/linux/fd_event_queue.o
	O_FILE_PATHS += ../../../build/yield/poll/posix/fd_event_queue.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/poll/posix/fd_event_queue.o
	O_FILE_PATHS += ../../../build/yield/poll/sunos/fd_event_queue.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/poll/win32/fd_event_queue.o
endif


all: ../../../lib/yield/libyield_poll.a

clean:
	$(RM) ../../../lib/yield/libyield_poll.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield_thread.a:
	$(MAKE) -C ../thread yield.thread.Makefile


../../../lib/yield/libyield_poll.a: $(O_FILE_PATHS) ../../../lib/yield/libyield_thread.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/poll/bsd/fd_event_queue.o: ../../../src/yield/poll/bsd/fd_event_queue.cpp
	-mkdir -p ../../../build/yield/poll/bsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/bsd/fd_event_queue.o -MD $(CXXFLAGS) ../../../src/yield/poll/bsd/fd_event_queue.cpp

../../../build/yield/poll/linux/fd_event_queue.o: ../../../src/yield/poll/linux/fd_event_queue.cpp
	-mkdir -p ../../../build/yield/poll/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/linux/fd_event_queue.o -MD $(CXXFLAGS) ../../../src/yield/poll/linux/fd_event_queue.cpp

../../../build/yield/poll/posix/fd_event_queue.o: ../../../src/yield/poll/posix/fd_event_queue.cpp
	-mkdir -p ../../../build/yield/poll/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/posix/fd_event_queue.o -MD $(CXXFLAGS) ../../../src/yield/poll/posix/fd_event_queue.cpp

../../../build/yield/poll/sunos/fd_event_queue.o: ../../../src/yield/poll/sunos/fd_event_queue.cpp
	-mkdir -p ../../../build/yield/poll/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/sunos/fd_event_queue.o -MD $(CXXFLAGS) ../../../src/yield/poll/sunos/fd_event_queue.cpp

../../../build/yield/poll/win32/fd_event_queue.o: ../../../src/yield/poll/win32/fd_event_queue.cpp
	-mkdir -p ../../../build/yield/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/win32/fd_event_queue.o -MD $(CXXFLAGS) ../../../src/yield/poll/win32/fd_event_queue.cpp
