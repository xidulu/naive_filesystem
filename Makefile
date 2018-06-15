CC=clang
MODULE = bitmap.c inode.c directory.c block.c p5.c

build: $(MODULE)
	$(CC) $(MODULE)  testp5.c -o p5.out -fsanitize=undefined

unit: $(MODULE)
	$(CC) $(MODULE) unit.c -o unit_test.out && ./unit_test.out

test: $(TARGET)
	$(CC) $(MODULE) testp5.c -o test.out && ./test.out

debug: $(MODULE)
	$(CC) $(MODULE) testp5.c -g

clean:
	rm -f ./*.o ./*.out
