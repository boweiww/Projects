all:
	make gameclient379 gameserver379 --always-make

gameclient379:
	gcc -std=c99  -pthread gameclient379.c -o gameclient379 #-D_POSIX_C_SOURCE -lncurses

gameserver379:
	gcc -std=c99 -D_POSIX_C_SOURCE -pthread gameserver379.c -o gameserver379 

clean: 
	rm gameclient379 gameserver379
