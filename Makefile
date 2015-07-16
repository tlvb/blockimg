.PHONY: default
default: blockimg
	./blockimg b test.png || gdb ./blockimg -ex 'set arg b test.png'
	./blockimg g test.png || gdb ./blockimg -ex 'set arg g test.png'
	./blockimg c test.png || gdb ./blockimg -ex 'set arg c test.png'

blockimg: blockimg.c
	cc -g -o $@ $^

.PHONY: clean
clean:
	rm blockimg || true
