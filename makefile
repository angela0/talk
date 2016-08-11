talk: talk.c
	gcc -o talk talk.c

install: talk
	cp talk /usr/local/bin/
