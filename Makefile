
main: pingserver pingclient1 pingclient2 pingclient3

pingserver:
	gcc -Wall pingserver.c -o pingserver

pingclient1:
	gcc -Wall pingclient1.c -o pingclient1

pingclient2:
	gcc -Wall pingclient2.c -o pingclient2

pingclient3:
	gcc -Wall pingclient3.c -o pingclient3