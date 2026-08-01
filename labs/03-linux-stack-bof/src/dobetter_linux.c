/*
 * dobetter_linux.c — intentionally vulnerable Linux teaching target
 *
 * Mitigations OFF: no canary, executable stack, no PIE (see Makefile).
 * Lab / CTF-style learning only.
 *
 * Modes:
 *   default     — interactive stdin line → do_response (stack BOF)
 *   --serve PORT — TCP server (same bug), default port 31339
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
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
#define DEFAULT_PORT 31339
#endif

/* Teaching gadget: pure JMP *RSP / *ESP (no endbr64 prefix). */
#if defined(__x86_64__)
void gadget_jmp_rsp(void);
__asm__(
	".text\n"
	".globl gadget_jmp_rsp\n"
	".type gadget_jmp_rsp, @function\n"
	"gadget_jmp_rsp:\n"
	"  jmp *%rsp\n"
);
#elif defined(__i386__)
void gadget_jmp_esp(void);
__asm__(
	".text\n"
	".globl gadget_jmp_esp\n"
	".type gadget_jmp_esp, @function\n"
	"gadget_jmp_esp:\n"
	"  jmp *%esp\n"
);
#endif

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
		const char *bye = "bye\n";
		(void)write(out_fd, bye, 4);
		return;
	}
	(void)do_response(out_fd, line, len);
}

static int serve_tcp(int port)
{
	int srv, client;
	struct sockaddr_in addr;
	int yes = 1;

	srv = socket(AF_INET, SOCK_STREAM, 0);
	if (srv < 0) {
		perror("socket");
		return 1;
	}
	setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons((uint16_t)port);
	if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}
	if (listen(srv, 5) < 0) {
		perror("listen");
		return 1;
	}
	printf("[+] dobetter_linux listening on 0.0.0.0:%d\n", port);
#if defined(__x86_64__)
	printf("[hint] gadget_jmp_rsp @ %p\n", (void *)gadget_jmp_rsp);
#endif

	for (;;) {
		char buf[LINE_MAX];
		size_t used = 0;
		client = accept(srv, NULL, NULL);
		if (client < 0) {
			perror("accept");
			continue;
		}
		const char *banner = "dobetter_linux ready. Send a line + \\n\n";
		(void)write(client, banner, strlen(banner));
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
				if (line_len == 4 && memcmp(buf, "exit", 4) == 0) {
					close(client);
					goto next_client;
				}
				used -= line_len + 1;
				memmove(buf, nl + 1, used);
			}
		}
		close(client);
	next_client:
		(void)0;
	}
}

int main(int argc, char **argv)
{
	printf("dobetter_linux - intentional stack BOF (mitigations off)\n");
#if defined(__x86_64__)
	printf("[hint] gadget_jmp_rsp @ %p (prefer objdump/ROPgadget)\n", (void *)gadget_jmp_rsp);
#elif defined(__i386__)
	printf("[hint] gadget_jmp_esp @ %p\n", (void *)gadget_jmp_esp);
#endif

	if (argc >= 2 && strcmp(argv[1], "--serve") == 0) {
		int port = DEFAULT_PORT;
		if (argc >= 3)
			port = atoi(argv[2]);
		return serve_tcp(port);
	}

	printf("Interactive mode: type a name, Enter. Ctrl-D to quit. (--serve [port] for TCP)\n");
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
