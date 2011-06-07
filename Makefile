
all:	comipiler

comipiler:
	gcc -o sxass sxass.c

clean:
	rm -rf sxass

