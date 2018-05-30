Q:=@ # quiet build
CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -I.. -I../munit
VPATH := $(VPATH):..:../munit

test: test.o munit.o

run : test
	$(Q)./test

clean:
	$(Q)rm -f *.o
	$(Q)rm -f test
