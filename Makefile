main: pingserver pingclient1 pingclient2 pingclient3

pingserver:
	gcc -O -Wall pingserver.c -o pingserver

pingclient1:
	gcc -O -Wall pingclient1.c -o pingclient1

pingclient2:
	gcc -O -Wall pingclient2.c -o pingclient2

pingclient3:
	gcc -O -Wall pingclient3.c -o pingclient3