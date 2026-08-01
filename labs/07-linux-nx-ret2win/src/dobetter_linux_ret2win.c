/*
 * dobetter_linux_ret2win.c — Linux x86-64, NX on, no PIE, no canary
 * Stack BOF + win() for ret2win under non-executable stack.
 *
 * Interactive stdin or --serve PORT (default 31343).
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define RESPONSE_SIZE 128
#define LINE_MAX 4096
#ifndef DEFAULT_PORT
#define DEFAULT_PORT 31343
#endif

void win(void)
{
	const char *msg = "[WIN] Linux ret2win - code reuse with NX on.\n";
	write(STDOUT_FILENO, msg, strlen(msg));
	_exit(0); /* clean stop after ret2win (avoid returning into smash debris) */
}

void gadget_ret(void);
__asm__(
	".text\n"
	".globl gadget_ret\n"
	".type gadget_ret, @function\n"
	"gadget_ret:\n"
	"  ret\n"
);

static int do_response(int out_fd, const char *name, size_t name_len)
{
	char response[RESPONSE_SIZE];

	if (name_len < 64) {
		int n = snprintf(response, sizeof(response), "Hello %.*s!!!\n",
				 (int)name_len, name);
		if (n < 0)
			return -1;
		if (n >= (int)sizeof(response))
			n = (int)sizeof(response) - 1;
		return write(out_fd, response, (size_t)n) < 0 ? -1 : 0;
	}

	/* VULNERABLE: unbounded memcpy into 128-byte stack buffer */
	memcpy(response, name, name_len);
	(void)write(out_fd, "ok\n", 3);
	return 0;
}

static void handle_line(int out_fd, char *line, size_t len)
{
	if (len > 0 && line[len - 1] == '\r')
		len--;
	if (len == 4 && memcmp(line, "exit", 4) == 0) {
		write(out_fd, "bye\n", 4);
		return;
	}
	do_response(out_fd, line, len);
}

static int serve_tcp(int port)
{
	int srv = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	int yes = 1;

	if (srv < 0)
		return 1;
	setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons((uint16_t)port);
	if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return 1;
	if (listen(srv, 5) < 0)
		return 1;
	printf("[+] listening 0.0.0.0:%d\n", port);

	for (;;) {
		char buf[LINE_MAX];
		size_t used = 0;
		int client = accept(srv, NULL, NULL);
		if (client < 0)
			continue;
		const char *banner = "dobetter_linux_ret2win (NX on). line+\\n\n";
		write(client, banner, strlen(banner));
		used = 0;
		for (;;) {
			ssize_t n;
			char *nl;
			if (used >= sizeof(buf))
				break;
			n = read(client, buf + used, sizeof(buf) - used);
			if (n <= 0)
				break;
			used += (size_t)n;
			while ((nl = memchr(buf, '\n', used)) != NULL) {
				size_t line_len = (size_t)(nl - buf);
				handle_line(client, buf, line_len);
				used -= line_len + 1;
				memmove(buf, nl + 1, used);
			}
		}
		close(client);
	}
}

int main(int argc, char **argv)
{
	printf("dobetter_linux_ret2win - NX on, no PIE, stack BOF -> ret2win\n");
	printf("[hint] win @ %p  gadget_ret @ %p\n", (void *)win, (void *)gadget_ret);

	if (argc >= 2 && strcmp(argv[1], "--serve") == 0) {
		int port = DEFAULT_PORT;
		if (argc >= 3)
			port = atoi(argv[2]);
		return serve_tcp(port);
	}

	printf("Interactive: type name+Enter. Ctrl-D quit. (--serve [port] for TCP)\n");
	for (;;) {
		char *line = NULL;
		size_t cap = 0;
		ssize_t n = getline(&line, &cap, stdin);
		if (n < 0) {
			free(line);
			break;
		}
		if (n > 0 && line[n - 1] == '\n')
			n--;
		handle_line(STDOUT_FILENO, line, (size_t)n);
		free(line);
	}
	return 0;
}
