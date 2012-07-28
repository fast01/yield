TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++ -lpthread
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lkstat
endif
LIBS += -lyield -lyield_thread


D_FILE_PATHS := $(shell find ../../../build/yield/stage -name "*.d")


O_FILE_PATHS += ../../../build/yield/stage/polling_stage_scheduler.o ../../../build/yield/stage/seda_stage_scheduler.o ../../../build/yield/stage/stage.o ../../../build/yield/stage/stage_scheduler.o ../../../build/yield/stage/wavefront_stage_scheduler.o


all: ../../../lib/yield/libyield_stage.a

clean:
	$(RM) ../../../lib/yield/libyield_stage.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield_thread.a:
	$(MAKE) -C ../thread yield.thread.Makefile


../../../lib/yield/libyield_stage.a: $(O_FILE_PATHS) ../../../lib/yield/libyield_thread.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/stage/polling_stage_scheduler.o: ../../../src/yield/stage/polling_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/polling_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/polling_stage_scheduler.cpp

../../../build/yield/stage/seda_stage_scheduler.o: ../../../src/yield/stage/seda_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/seda_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/seda_stage_scheduler.cpp

../../../build/yield/stage/stage.o: ../../../src/yield/stage/stage.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/stage.o -MD $(CXXFLAGS) ../../../src/yield/stage/stage.cpp

../../../build/yield/stage/stage_scheduler.o: ../../../src/yield/stage/stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/stage_scheduler.cpp

../../../build/yield/stage/wavefront_stage_scheduler.o: ../../../src/yield/stage/wavefront_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/wavefront_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/wavefront_stage_scheduler.cpp
