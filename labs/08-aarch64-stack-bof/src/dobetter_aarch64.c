/*
 * dobetter_aarch64.c — intentionally vulnerable AArch64 Linux stack BOF
 *
 * Mitigations off for teaching: -fno-stack-protector -z execstack -no-pie
 * Same Hello+name protocol as other labs (interactive or --serve).
 *
 * Build: make -C labs/08-aarch64-stack-bof
 *   (needs aarch64-linux-gnu-gcc on x86 hosts, or native gcc on ARM64)
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
#define DEFAULT_PORT 31344
#endif

/* Teaching: branch to register (similar role to JMP RSP). */
#if defined(__aarch64__)
void gadget_br_x0(void);
__asm__(
	".text\n"
	".global gadget_br_x0\n"
	".type gadget_br_x0, %function\n"
	"gadget_br_x0:\n"
	"  br x0\n"
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
	if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0 || listen(srv, 5) < 0)
		return 1;
	printf("[+] aarch64 lab listening :%d\n", port);
	for (;;) {
		char buf[LINE_MAX];
		size_t used = 0;
		int c = accept(srv, NULL, NULL);
		if (c < 0)
			continue;
		const char *b = "dobetter_aarch64 ready\n";
		write(c, b, strlen(b));
		for (;;) {
			ssize_t n;
			char *nl;
			if (used >= sizeof(buf))
				break;
			n = read(c, buf + used, sizeof(buf) - used);
			if (n <= 0)
				break;
			used += (size_t)n;
			while ((nl = memchr(buf, '\n', used)) != NULL) {
				size_t ll = (size_t)(nl - buf);
				handle_line(c, buf, ll);
				used -= ll + 1;
				memmove(buf, nl + 1, used);
			}
		}
		close(c);
	}
}

int main(int argc, char **argv)
{
	printf("dobetter_aarch64 - stack BOF teaching target (AArch64)\n");
#if defined(__aarch64__)
	printf("[hint] gadget_br_x0 @ %p\n", (void *)gadget_br_x0);
#else
	printf("[hint] build with an AArch64 toolchain for gadget_br_x0\n");
#endif

	if (argc >= 2 && strcmp(argv[1], "--serve") == 0) {
		int port = DEFAULT_PORT;
		if (argc >= 3)
			port = atoi(argv[2]);
		return serve_tcp(port);
	}

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
