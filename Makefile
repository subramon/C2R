FLAGS :=  -std=gnu99 
FLAGS +=  -fPIC

WARN :=  -Wall -W -Waggregate-return -Wcast-align -Wmissing-prototypes 
WARN += -Wnested-externs -Wshadow -Wwrite-strings 

DBGFLAGS= 
DBGFLAGS= -g -O0

OPTFLAGS= -O4
OPTFLAGS= 

CFLAGS= $(DBGFLAGS) $(OPTFLAGS)  $(WARN) $(FLAGS)
CC= gcc

INCS := -I./inc/
INCS += -I./rincs/
.c.o : 
	$(CC) -c -o $@ $< $(CFLAGS)  $(INCS)

UNIT_TESTS := unit_test_1

all : libc2r.so rshutdown ${UNIT_TESTS}

unit_test_1 : ./test/unit_test_1.o libc2r.so
	$(CC) $(CFLAGS) ./test/unit_test_1.o  \
	libc2r.so \
	-o unit_test_1

rshutdown : ./src/rshutdown.o 
	$(CC) $(CFLAGS) ./src/rshutdown.o  \
		libc2r.so -o rshutdown 

LIB_SRCS := src/aux_rcli.c 
LIB_SRCS += src/get_named_vec.c 
LIB_SRCS += src/set_vec.c 
LIB_SRCS += src/get_vec.c 
LIB_SRCS += src/get_bool.c 
LIB_SRCS += src/exists.c 
LIB_SRCS += src/chk_R_class.c 
LIB_SRCS += src/n_df.c 
LIB_SRCS += src/get_F8.c 
LIB_SRCS += src/get_I4.c 
LIB_SRCS += src/str_array_R_to_C.c 
LIB_OBJS = $(patsubst %.c, %.o, $(LIB_SRCS)) 

./src/aux_rcli.o : CFLAGS += -Wno-cast-align
./src/set_vec.o : CFLAGS += -Wno-cast-align
./src/get_named_vec.o : CFLAGS += -Wno-cast-align
./src/get_bool.o : CFLAGS += -Wno-cast-align
./src/get_vec.o : CFLAGS += -Wno-cast-align
./src/get_F8.o : CFLAGS += -Wno-cast-align
./src/get_vec_len.o : CFLAGS += -Wno-cast-align
./src/exists.o : CFLAGS += -Wno-cast-align

./test/unit_test_1.o : CFLAGS += -Wno-cast-align

libc2r.so : $(LIB_OBJS)
	$(CC) -fPIC -shared $(CFLAGS) $(LIB_OBJS) -o libc2r.so  

clean:
	rm -r -f *.o rshutdown *.so ${UNIT_TESTS} ./src/*.o ./test/*.o

