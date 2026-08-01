/*
 * dobetter_x64.c — intentionally vulnerable Win64 teaching server
 *
 * Companion to dobetter_x86: same protocol, 64-bit stack SRP → RIP control.
 * Build with mitigations OFF. Lab VMs only.
 *
 * Default port: 31338
 *
 * Length-based unsafe copy (not sprintf %s) so payloads may contain 0x00 —
 * required for many x64 little-endian addresses (e.g. 0x14000....).
 */

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifndef LPORT
#define LPORT "31338"
#endif

#define RECVBUFSIZE 4096
#define RESPONSE_SIZE 128

static void handle_connection(void *param);
static int do_response(SOCKET client, char *name, size_t name_len);

/*
 * Teaching gadget: JMP RSP (FF E4) — same encoding as JMP ESP on x86.
 * mona: !mona jmp -r rsp
 */
#if defined(__GNUC__) && (defined(__x86_64__) || defined(_M_X64))
void gadget_jmp_rsp(void);
__asm__(
	".text\n"
	".globl gadget_jmp_rsp\n"
#if defined(_WIN32)
	".globl gadget_jmp_rsp\n"
#endif
	"gadget_jmp_rsp:\n"
	"  jmp *%rsp\n"
);
#elif defined(_MSC_VER)
/* MSVC x64: no inline asm — executable section blob */
#pragma section("gadgets", read, execute)
__declspec(allocate("gadgets"))
const unsigned char gadget_jmp_rsp_bytes[] = { 0xFF, 0xE4, 0xC3 };
#else
#if defined(__GNUC__)
__attribute__((section(".text"), used))
#endif
const unsigned char gadget_jmp_rsp_bytes[] = { 0xFF, 0xE4, 0xC3 };
#endif

int main(void)
{
	WSADATA wsa = { 0 };
	int result;
	struct addrinfo hints;
	struct addrinfo *ainfo = NULL;
	SOCKET listen_sock = INVALID_SOCKET;

	printf("dobetter_x64 — intentional x64 stack BOF teaching server\n");
	printf("Listen: 0.0.0.0:%s  |  mitigations must stay OFF for the lab\n", LPORT);

#if defined(__GNUC__) && (defined(__x86_64__) || defined(_M_X64))
	printf("[hint] gadget_jmp_rsp @ %p (prefer !mona jmp -r rsp)\n", (void *)gadget_jmp_rsp);
#else
	printf("[hint] gadget_jmp_rsp_bytes @ %p (prefer !mona jmp -r rsp)\n",
	       (void *)gadget_jmp_rsp_bytes);
#endif

	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		return 1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, LPORT, &hints, &ainfo);
	if (result != 0) {
		printf("getaddrinfo failed: %d\n", result);
		WSACleanup();
		return 1;
	}

	listen_sock = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
	if (listen_sock == INVALID_SOCKET) {
		printf("socket failed: %d\n", WSAGetLastError());
		freeaddrinfo(ainfo);
		WSACleanup();
		return 1;
	}

	{
		BOOL yes = 1;
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
	}

	if (bind(listen_sock, ainfo->ai_addr, (int)ainfo->ai_addrlen) == SOCKET_ERROR) {
		printf("bind failed: %d\n", WSAGetLastError());
		freeaddrinfo(ainfo);
		closesocket(listen_sock);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(ainfo);

	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
		printf("listen failed: %d\n", WSAGetLastError());
		closesocket(listen_sock);
		WSACleanup();
		return 1;
	}

	printf("[+] Listening. Attach WinDBG (x64), load mona, send line + \\n\n");

	for (;;) {
		SOCKET client = accept(listen_sock, NULL, NULL);
		if (client == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			continue;
		}
		printf("[+] Connection accepted\n");
		_beginthread(handle_connection, 0, (void *)(uintptr_t)client);
	}
}

static void handle_connection(void *param)
{
	SOCKET client = (SOCKET)(uintptr_t)param;
	char recvbuf[RECVBUFSIZE];
	size_t used = 0;
	const char *banner = "dobetter_x64 ready. Send a name line ending with \\n (or 'exit').\n";

	send(client, banner, (int)strlen(banner), 0);
	memset(recvbuf, 0, sizeof(recvbuf));

	for (;;) {
		size_t space;
		int n;
		char *line_start;
		char *line_end;

		space = sizeof(recvbuf) - used;
		if (space < 1) {
			const char *msg = "line too long\n";
			send(client, msg, (int)strlen(msg), 0);
			break;
		}

		n = recv(client, recvbuf + used, (int)space, 0);
		if (n == 0) {
			printf("[-] Client disconnected\n");
			break;
		}
		if (n < 0) {
			printf("[-] recv failed: %d\n", WSAGetLastError());
			break;
		}
		used += (size_t)n;

		line_start = recvbuf;
		while ((line_end = (char *)memchr(line_start, '\n', used - (size_t)(line_start - recvbuf))) != NULL) {
			size_t line_len = (size_t)(line_end - line_start);
			if (line_len > 0 && line_start[line_len - 1] == '\r')
				line_len--;

			if (line_len == 4 && memcmp(line_start, "exit", 4) == 0) {
				const char *bye = "bye\n";
				send(client, bye, (int)strlen(bye), 0);
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
		if (sent == SOCKET_ERROR)
			return -1;
		printf("[*] replied %d bytes (name len=%u)\n", sent, (unsigned)name_len);
		return 0;
	}

	/* Intentionally unsafe — teaching only */
	memcpy(response, name, name_len);
	sent = send(client, "ok\n", 3, 0);
	(void)sent;
	printf("[*] vulnerable memcpy done (name len=%u)\n", (unsigned)name_len);
	return 0;
}
