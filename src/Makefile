CC:=gcc
SRC:=${wildcard *.c}
OBJ:=${SRC:.c=.o}
CFLAGS:=-Wextra -pedantic -lpthread
RTC:=RTC
INCLUDE=I../include/

${RTC} : ${OBJ}
	${CC} ${CFLAGS} $? -o $@

%.o : %.c
	${CC} ${CFLAGS} -c $<

clean:
	rm ${RTC} ${OBJ}


