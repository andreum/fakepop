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

void flusha()
{
	fflush(stdout);
	fflush(stdin);
}

void banner() 
{
	printf("+OK Fakepop - <%d.%d%04x@tatui>\r\n", time(NULL), getpid(), rand());
	flusha();
}

double getload()
{
	FILE *f = fopen(loadavgfile, "r");
	char line[1024];
	double load;
	char *ptr;
	if (!f) {
		// fprintf(stderr, "CANNOT LOAD LOADAVG FILE: %s", loadavgfile);
		return 100.0;
	}
	fgets(line, 1024, f);
	load = strtod(line, &ptr);
	fclose(f);
	return load;
	
}

void ok(const char *msg)
{
	printf("+OK %s\r\n", msg);
	flusha();
}

void bye()
{
	ok("Ate mais tarde");
}

void err(const char *msg)
{
	printf("-ERR %s\r\n", msg);
	flusha();
}

int status = 0;

int do_capa()
{
	ok("These are my limits, Sir");
	printf("TOP\r\n");
	printf("USER\r\n");
	printf("LOGIN-DELAY 600\r\n");
	printf("EXPIRE NEVER\r\n");
	printf("IMPLEMENTATION OverLoadPop-1\r\n");
	printf(".\r\n");
	flusha();
}


int do_command(FILE *f)
{
	char buf[1024];
	char *eptr;
	char line[128];
	flusha();
	char *ptr = fgets(buf, 1024, f);

	if (!ptr) {
		sprintf(line, "Erro ao tentar ler comando: %d", errno);
		// perror("perror: Erro ao tentar ler comando: ");
		syslog(LOG_ERR, line);
		return 0;
	}

	for (eptr = (char *)buf; eptr < buf + 1023 && *eptr != '\r' && *eptr != '\n'; eptr++);
	*eptr = 0;

	if (strncasecmp(buf, "PASS", 4)) {
		syslog(LOG_INFO, buf);
	}
	// fprintf(stderr, "Comando: [%s]\n", buf);
	// fflush(stderr);
	if (!strncasecmp(buf, "QUIT", 4)) {
		return do_quit();
	}
	if (!strncasecmp(buf, "CAPA", 4)) {
		do_capa();
		return 1;
	}
	switch(status) {
		case 0:
			if (!strncasecmp(buf, "USER", 4)) {
				ok("Entre sua senha");
				status = 1;
				break;
			}
			if (!strncasecmp(buf, "APOP", 4)) {
				ok("You have 0 messages (0 bytes)");
				status = 2;
				break;
			}
			err("Comando desconhecido");
			break;
		case 1:
			if (!strncasecmp(buf, "PASS", 4)) {
				ok("Senha aceita");
				status = 2;
				break;
			}
			err("Comando desconhecido");
			break;
		case 2:
			if (!strncasecmp(buf, "STAT", 4)) {
				ok("0 0");
				break;
			}
			if (!strcasecmp(buf, "UIDL", 4)) {
				ok("");
				printf(".\r\n");
				flusha();
				break;
			}
			if (!strncasecmp(buf, "UIDL", 4)) {
				err("Mensagem invalida");
				break;
			}
			if (!strncasecmp(buf, "LIST", 4)) {
				ok("0 messages");
				printf(".\r\n");
				flusha();
				break;
			}
			if (!strncasecmp(buf, "TOP", 3)) {
				err("Mensagem desconhecida");
				break;
			}
			if (!strncasecmp(buf, "RETR", 4)) {
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
	flusha();
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

	chdir("/tmp");
	openlog("fakepop", LOG_PID, LOG_LOCAL0);
	if (load < MAXLOAD) {
		execpop();
	}
	srand(getpid() ^ time(NULL));
	banner();
	while(do_command(stdin));
	bye();
}

