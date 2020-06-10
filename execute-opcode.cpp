#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <errno.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifdef __APPLE__
using sighandler_t = sig_t;
#endif

using namespace std;
typedef void (*fonction_vide)();

void sigsegv(int si)
{
  cout << "memory or other problem" << endl; //if the execution ended wrongly
  exit(1);
}

void initbt() { //handler of  SIGSEV and SIGBUS
    signal(SIGSEGV, (sighandler_t) sigsegv);
    signal(SIGBUS, (sighandler_t) sigsegv);
}


unsigned char atoh(char * nombre){
        unsigned char n = 0;
	for (int i = 0; nombre[i] != '\0'; i++){
                n = n * 16 ;
		if (nombre[i] >= 'a') n += 10 + (nombre[i] - 'a');
		else if (nombre[i] >= 'A') n += 10 + (nombre[i] - 'A');
		else n += nombre[i] - '0';
	}
  	return n;
}

int main(int argc, char *argv[]){
        //argv = b9 04 43 ... 55 == a list of binary code bytes that will be run
        initbt();
	char * code = (char *) malloc(argc);
	for(int i = 1 ; i < argc ; ++i){
		code[i-1] = atoh(argv[i]);
	}
	code[argc-1] = '\xc3'; //c3=ret : to return back to main
	unsigned long pos = (unsigned long )  code;
	int page_size = getpagesize();
	pos -= (unsigned long)pos % page_size;
	if (mprotect( (void *) pos, page_size, PROT_EXEC | PROT_READ | PROT_WRITE) != 0)
    {
		//handle_error("mprotect");
        switch(errno)
        {
            case EACCES: cout << "EACCESS" << endl; break;
            case EINVAL: cout << "EINVAL" << endl; break;
            case ENOMEM: cout << "ENOMEM" << endl; break;
        }
	}
	fonction_vide f = (fonction_vide) code;
	f(); //CALL TO THE INSTRUCTION !!
	cout << "done" << endl; //everything end correctly
	return 0;
} 
