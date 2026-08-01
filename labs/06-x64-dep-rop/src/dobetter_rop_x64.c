/*
 * dobetter_rop_x64.c — DEP-on x64 stack overflow, ret2win teaching target
 *
 * ASLR off, DEP/NX on, no cookie. Port 31342.
 */

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifndef LPORT
#define LPORT "31342"
#endif

#define RECVBUFSIZE 4096
#define RESPONSE_SIZE 128

void win(void)
{
	printf("[WIN] x64 ROP reached win() with DEP/NX on.\n");
	fflush(stdout);
	ExitProcess(0);
}

#if defined(__GNUC__) && (defined(__x86_64__) || defined(_M_X64))
void gadget_ret(void);
void gadget_jmp_rsp(void);
__asm__(
	".text\n"
	".globl gadget_ret\n"
	"gadget_ret:\n"
	"  ret\n"
	".globl gadget_jmp_rsp\n"
	"gadget_jmp_rsp:\n"
	"  jmp *%rsp\n"
);
#endif

static void handle_connection(void *param);
static int do_response(SOCKET client, char *name, size_t name_len);

int main(void)
{
	WSADATA wsa = { 0 };
	struct addrinfo hints;
	struct addrinfo *ainfo = NULL;
	SOCKET listen_sock;

	printf("dobetter_rop_x64 - DEP ON, ASLR OFF, x64 ret2win lab\n");
	printf("Listen: 0.0.0.0:%s\n", LPORT);
	printf("[hint] win @ %p\n", (void *)win);
#if defined(__GNUC__)
	printf("[hint] gadget_ret @ %p  gadget_jmp_rsp @ %p\n",
	       (void *)gadget_ret, (void *)gadget_jmp_rsp);
#endif

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, LPORT, &hints, &ainfo) != 0) {
		WSACleanup();
		return 1;
	}

	listen_sock = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
	if (listen_sock == INVALID_SOCKET) {
		freeaddrinfo(ainfo);
		WSACleanup();
		return 1;
	}
	{
		BOOL yes = 1;
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
	}
	if (bind(listen_sock, ainfo->ai_addr, (int)ainfo->ai_addrlen) == SOCKET_ERROR ||
	    listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
		freeaddrinfo(ainfo);
		closesocket(listen_sock);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(ainfo);
	printf("[+] Listening (ret2win with DEP on).\n");

	for (;;) {
		SOCKET client = accept(listen_sock, NULL, NULL);
		if (client == INVALID_SOCKET)
			continue;
		_beginthread(handle_connection, 0, (void *)(uintptr_t)client);
	}
}

static void handle_connection(void *param)
{
	SOCKET client = (SOCKET)(uintptr_t)param;
	char recvbuf[RECVBUFSIZE];
	size_t used = 0;
	const char *banner = "dobetter_rop_x64 ready (DEP on). Send line+\\n\n";

	send(client, banner, (int)strlen(banner), 0);
	memset(recvbuf, 0, sizeof(recvbuf));

	for (;;) {
		int n;
		char *line_start, *line_end;
		if (sizeof(recvbuf) - used < 1)
			break;
		n = recv(client, recvbuf + used, (int)(sizeof(recvbuf) - used), 0);
		if (n <= 0)
			break;
		used += (size_t)n;
		line_start = recvbuf;
		while ((line_end = memchr(line_start, '\n', used - (size_t)(line_start - recvbuf))) != NULL) {
			size_t line_len = (size_t)(line_end - line_start);
			if (line_len > 0 && line_start[line_len - 1] == '\r')
				line_len--;
			if (line_len == 4 && memcmp(line_start, "exit", 4) == 0) {
				send(client, "bye\n", 4, 0);
				closesocket(client);
				return;
			}
			do_response(client, line_start, line_len);
			line_start = line_end + 1;
		}
		used -= (size_t)(line_start - recvbuf);
		memmove(recvbuf, line_start, used);
	}
	closesocket(client);
}

static int do_response(SOCKET client, char *name, size_t name_len)
{
	char response[RESPONSE_SIZE];
	int sent;

	if (name_len < 64) {
		int n = snprintf(response, sizeof(response), "Hello %.*s!!!\n",
				 (int)name_len, name);
		if (n < 0)
			return -1;
		if (n >= (int)sizeof(response))
			n = (int)sizeof(response) - 1;
		sent = send(client, response, n, 0);
		return sent == SOCKET_ERROR ? -1 : 0;
	}

	memcpy(response, name, name_len);
	sent = send(client, "ok\n", 3, 0);
	(void)sent;
	return 0;
}
