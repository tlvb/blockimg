tput=`which tput`
gm=`which gm`

blockimg: blockimg.c
	cc -Wall -std=c99 -DTPUT=\"$(tput)\" -DGRAPHICSMAGICK=\"$(gm)\" -o $@ $^

.PHONY: clean
clean:
	rm blockimg || true
