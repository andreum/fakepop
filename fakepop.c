#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>

const char *loadavgfile = "/proc/loadavg";

const char *const cmd = "/usr/local/teapop/libexec/teapop";
char *const args[] = {"teapop", "-n", "-N", "-L", "-t600", NULL};

#define MAXLOAD 20.0

void banner() 
{
	printf("+OK Fakepop - <%d.%d%04x@tatui>\n", time(NULL), getpid(), rand());
}

double getload()
{
	FILE *f = fopen(loadavgfile, "r");
	char line[1024];
	double load;
	char *ptr;
	if (!f) {
		fprintf(stderr, "CANNOT LOAD LOADAVG FILE: %s", loadavgfile);
		return 100.0;
	}
	fgets(line, 1024, f);
	load = strtold(line, &ptr);
	fclose(f);
	fprintf(stderr, "Load: %.2f\n", load);
	return load;
	
}

void ok(const char *msg)
{
	printf("+OK %s\n", msg);
}

void bye()
{
	ok("Ate mais tarde");
}

void err(const char *msg)
{
	printf("-ERR %s\n", msg);
}

int status = 0;

int do_capa()
{
	ok("These are my limits, Sir");
	printf("TOP\n");
	printf("USER\n");
	printf("LOGIN-DELAY\n");
	printf("EXPIRE NEVER\n");
	printf("IMPLEMENTATION OverLoadPop-1\n");
	printf(".\n");
}


int do_command(FILE *f)
{
	char buf[1024];
	const char *ptr = fgets(buf, 1024, f);
	if (!strncasecmp(ptr, "QUIT", 4)) {
		return do_quit();
	}
	if (!strncasecmp(ptr, "CAPA", 4)) {
		do_capa();
		return 1;
	}
	switch(status) {
		case 0:
			if (!strncasecmp(ptr, "USER", 4)) {
				ok("Entre sua senha");
				status = 1;
				break;
			}
			err("Comando desconhecido");
			break;
		case 1:
			if (!strncasecmp(ptr, "PASS", 4)) {
				ok("Senha aceita");
				status = 2;
				break;
			}
			err("Comando desconhecido");
			break;
		case 2:
			if (!strncasecmp(ptr, "LIST", 4)) {
				ok("0 messages");
				break;
			}
			if (!strncasecmp(ptr, "TOP", 3)) {
				err("Mensagem desconhecida");
				break;
			}
			if (!strncasecmp(ptr, "RETR", 4)) {
				err("Mensagem desconhecida");
				break;
			}
		default:
			err("Comando desconhecido");
	}
	return 1;
}

int do_quit()
{
	return 0;
}

void execpop()
{
	int err;
	if (err = execvp(cmd, args)) {
		char *erro;
		char buf[1024];
		snprintf(buf, 1024, "erro em exec: %d", errno);
		perror("Nao rolou: ");
		syslog(LOG_ERR, buf);
		exit(err);
	};
}


int main(int argc, char **argv) 
{
	double load = getload();

	openlog("fakepop", LOG_PID, LOG_LOCAL0);
	if (load < MAXLOAD) {
		execpop();
	}
	srand(getpid() ^ time(NULL));
	banner();
	while(do_command(stdin));
	bye();
}

