CS = himenoBMT_t.c barrier.c
OBJS = himenoBMT_t.o barrier.o

CC = cc
RM = /bin/rm
TARGET = bmt

CFLAGS = -D_REENTRANT -O3 -DSMALL
BARFLAGS = -O1
LDFLAGS = ${CFLAGS} -lpthread
# SUNOS5
# CFLAGS = -D_REENTRANT -xO3 -DSMALL -DSUNOS5 -mt
# BARFLAGS = -xO1
# LDFLAGS = ${CFLAGS} -lpthread -lthread

# SGI
# CFLAGS = -D_REENTRANT -O3 -DSMALL -DSGI
# BARFLAGS = -O1
# LDFLAGS = ${CFLAGS} -lpthread

all : ${TARGET}

${TARGET} : ${OBJS}
		${CC} -o $@ ${OBJS} ${LDFLAGS}
himenoBMT_t.o :  himenoBMT_t.c barrier.h
		${CC} -c ${CFLAGS} himenoBMT_t.c
barrier.o : barrier.c barrier.h
		${CC} -c ${BARFLAGS} barrier.c

.c.o :
		${CC} -c ${CFLAGS} $<

clean :
		${RM} -f ${TARGET} ${OBJS} core
