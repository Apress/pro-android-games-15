#include <stdio.h>

extern int main (int argc, char **argv);

void _start(int argc, char **argv)
{
	char * args[] = {"quake2"
		, "+set", "basedir", "/sdcard/quake2"
		, "+set", "nocdaudio", "1"
		, "+set", "cd_nocd", "1"
		, "+set", "s_initsound", "0"
		, "+set", "vid_ref", "glx"
		, "+set", "gl_driver", "libnanoGL.so"
	};
	int len = 19;
	int i ;

	for ( i = 0 ; i < len ; i++) {
		printf("Main[%d]=%s\n", i, args[i]);
	}

	printf("Calling quake 2\n");

	main(len, args);

	exit (0);
}
