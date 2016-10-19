CC = agcc

CFLAGS 	= 
LFLAGS 	= -shared

OBJ = nanoWrap.o eglwrap.o nanogl.o 
#OBJ 	=  glwrap.o 

#-lGLES_CM
all: $(OBJ)
	ald $(LFLAGS) -o libnanoGL.so $(OBJ) 

test:
	agcc -c test.c
	ald -o test test.o -L. -lnanoGL
.c.o:
	@echo
	$(CC) -fpic -c $(CFLAGS) $<
	
.cpp.o:
	@echo
	ag++ -fpic -c $(CFLAGS) $<

deploy: all
	adb push libnanoGL.so /data

#adb push test /data

clean:
	rm *.o test
