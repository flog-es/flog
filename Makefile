CFLAGS ?= -O2
CFLAGS += $(COMMON_FLAGS)
FLOG_FLAGS = -pthread -fPIE -fno-unwind-tables -fno-asynchronous-unwind-tables

SRCS=$(wildcard source/*.cc source/**/*.cc)

all: flog

flog: $(SRCS)
	$(CXX) -o $@ -lv8 -lv8_libplatform -ldl -lgit2 \
$(SRCS) \
-m64 -O3 \
-DV8_COMPRESS_POINTERS -DV8_31BIT_SMIS_ON_64BIT_ARCH \
-Wall -Wextra -Wno-unused-parameter -Wshadow \
-fno-omit-frame-pointer \
-pedantic -std=c++20
