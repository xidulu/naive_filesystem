CC=gcc
MODULE = bitmap.c inode.c directory.c

unit: $(MODULE)
	$(CC) $(MODULE) unit.c -o unit_test.out && ./unit_test.out

clean:
	rm -f ./*.o ./*.out