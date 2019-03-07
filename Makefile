CC = gcc
COMPILER_FLAGS = -g -std=gnu99
LINKER_FLAGS = -g -lpthread -lm
BINARYOSS = oss
BINARYUSER = user
OBJSOSS = oss.o common.o osstime.o
OBJSUSER = user.o common.o osstime.o
HEADERS = common.h osstime.h

all: $(BINARYOSS) $(BINARYUSER)

$(BINARYOSS): $(OBJSOSS)
	$(CC) -o $(BINARYOSS) $(OBJSOSS) $(LINKER_FLAGS)

$(BINARYUSER): $(OBJSUSER)
	$(CC) -o $(BINARYUSER) $(OBJSUSER) $(LINKER_FLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(COMPILER_FLAGS) -c $<

clean:
	/bin/rm $(OBJSOSS) $(OBJSUSER) $(BINARYOSS) $(BINARYUSER)

dist:
	zip -r oss.zip *.c *.h Makefile README
