#define	_MAFI_C_
#include "mafi.h"

int
main(argc,argv)
    	int	argc;
	char	**argv;
{
#ifdef	DEBUG
	FILE	*fp;
#endif
	tty=isatty(0);
	inithash();
#ifdef	DEBUG
	fp = fopen("/var/tmp/mafi.log", "a");
#endif
	for(;!err&&scanline()>=0;dolineno++){
#ifdef	DEBUG
		fprintf(fp, "%s\n", buffer[dolineno]);
#endif
		doline(buffer[dolineno], 0);
	}
#ifdef	DEBUG
	fclose(fp);
#endif
	exit(err);
}
