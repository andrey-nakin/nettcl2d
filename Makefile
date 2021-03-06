# dependendences
INCLUDE_DIR = -I/usr/include/tcl8.5 -I../phlib/src
LIB_DIR = -L../phlib/dist
PTHREAD = -pthread -lpthread

# compiler settings
CC=g++
CFLAGS += -static -O3 -Wall $(INCLUDE_DIR) $(LIB_DIR) -Wl,--unresolved-symbols=ignore-all

NETTCL2D_SRC_DIR = src/nettcl2d
NETTCL2D_SRCS = $(addprefix $(NETTCL2D_SRC_DIR)/, main.cpp calc/tagable.cpp)
NETTCL2D_EXECUTABLE = nettcl2d

all:	$(NETTCL2D_EXECUTABLE)

$(NETTCL2D_EXECUTABLE):	$(NETTCL2D_SRCS)
	$(CC) $(CFLAGS) $(NETTCL2D_SRCS) $(PTHREAD) -ltcl8.5 -ldl -lgslcblas -lgsl -lphlib -o $(NETTCL2D_EXECUTABLE)

