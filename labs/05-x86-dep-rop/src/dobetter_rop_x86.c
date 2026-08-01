/*
 * dobetter_rop_x86.c — DEP-on stack overflow for ROP teaching
 *
 * - ASLR / dynamic base: OFF
 * - DEP / NX: ON  (stack is NOT executable)
 * - Stack cookies: OFF
 *
 * Part A: ret2win — redirect EIP to win()
 * Part B: prepare for mona ROP → VirtualProtect + shellcode (see TUTORIAL)
 *
 * Port: 31341
 * Lab VMs only.
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
#define LPORT "31341"
#endif

#define RECVBUFSIZE 4096
#define RESPONSE_SIZE 128

/* Force an import mona can use for VirtualProtect chains. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static void *force_vp_import = (void *)VirtualProtect;
#pragma GCC diagnostic pop

#if defined(_MSC_VER)
#pragma comment(linker, "/include:_VirtualProtect@16")
#endif

void win(void)
{
	const char *msg =
		"[WIN] ROP reached win() - code reuse works with DEP/NX on.\n";
	printf("%s", msg);
	fflush(stdout);
	ExitProcess(0);
}

/*
 * Teaching gadgets for manual chains (still discover with mona on real targets).
 * Symbol names help students correlate objdump ↔ mona.
 */
#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
void gadget_pop_eax_ret(void);
void gadget_pop_ecx_ret(void);
void gadget_pop_edx_ret(void);
void gadget_pop_ebx_ret(void);
void gadget_ret(void);
void gadget_jmp_esp(void);
void gadget_push_esp_ret(void);
__asm__(
	".text\n"
	".globl _gadget_pop_eax_ret\n_gadget_pop_eax_ret:\n  popl %eax\n  ret\n"
	".globl _gadget_pop_ecx_ret\n_gadget_pop_ecx_ret:\n  popl %ecx\n  ret\n"
	".globl _gadget_pop_edx_ret\n_gadget_pop_edx_ret:\n  popl %edx\n  ret\n"
	".globl _gadget_pop_ebx_ret\n_gadget_pop_ebx_ret:\n  popl %ebx\n  ret\n"
	".globl _gadget_ret\n_gadget_ret:\n  ret\n"
	".globl _gadget_jmp_esp\n_gadget_jmp_esp:\n  jmp *%esp\n"
	".globl _gadget_push_esp_ret\n_gadget_push_esp_ret:\n  pushl %esp\n  ret\n"
);
#elif defined(_MSC_VER) && defined(_M_IX86)
__declspec(naked) void gadget_pop_eax_ret(void) { __asm { pop eax  ret } }
__declspec(naked) void gadget_pop_ecx_ret(void) { __asm { pop ecx  ret } }
__declspec(naked) void gadget_pop_edx_ret(void) { __asm { pop edx  ret } }
__declspec(naked) void gadget_pop_ebx_ret(void) { __asm { pop ebx  ret } }
__declspec(naked) void gadget_ret(void) { __asm { ret } }
__declspec(naked) void gadget_jmp_esp(void) { __asm { jmp esp } }
__declspec(naked) void gadget_push_esp_ret(void) { __asm { push esp  ret } }
#endif

static void handle_connection(void *param);
static int do_response(SOCKET client, char *name, size_t name_len);

int main(void)
{
	WSADATA wsa = { 0 };
	struct addrinfo hints;
	struct addrinfo *ainfo = NULL;
	SOCKET listen_sock;
	int result;

	(void)force_vp_import;

	printf("dobetter_rop_x86 - DEP ON, ASLR OFF, stack BOF for ROP teaching\n");
	printf("Listen: 0.0.0.0:%s\n", LPORT);
	printf("[hint] win              @ %p\n", (void *)win);
#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
	printf("[hint] pop eax;ret      @ %p\n", (void *)gadget_pop_eax_ret);
	printf("[hint] pop ecx;ret      @ %p\n", (void *)gadget_pop_ecx_ret);
	printf("[hint] pop edx;ret      @ %p\n", (void *)gadget_pop_edx_ret);
	printf("[hint] pop ebx;ret      @ %p\n", (void *)gadget_pop_ebx_ret);
	printf("[hint] jmp esp          @ %p\n", (void *)gadget_jmp_esp);
	printf("[hint] push esp;ret     @ %p\n", (void *)gadget_push_esp_ret);
	printf("[hint] VirtualProtect   @ IAT — mona rop / appendix for pinned MinGW\n");
#elif defined(_MSC_VER) && defined(_M_IX86)
	printf("[hint] pop eax;ret      @ %p\n", (void *)gadget_pop_eax_ret);
	printf("[hint] jmp esp          @ %p\n", (void *)gadget_jmp_esp);
	printf("[hint] VirtualProtect imported for mona rop\n");
#endif
	printf("[*] Stack should be NON-executable (DEP). Shellcode on stack will crash until ROP.\n");

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

	printf("[+] Listening. Part A: ret2win. Part B: mona ROP + VirtualProtect.\n");

	for (;;) {
		SOCKET client = accept(listen_sock, NULL, NULL);
		if (client == INVALID_SOCKET)
			continue;
		printf("[+] Connection accepted\n");
		_beginthread(handle_connection, 0, (void *)(uintptr_t)client);
	}
}

static void handle_connection(void *param)
{
	SOCKET client = (SOCKET)(uintptr_t)param;
	char recvbuf[RECVBUFSIZE];
	size_t used = 0;
	const char *banner =
		"dobetter_rop_x86 ready (DEP on). Send line+\\n. Try ret2win first.\n";

	send(client, banner, (int)strlen(banner), 0);
	memset(recvbuf, 0, sizeof(recvbuf));

	for (;;) {
		size_t space = sizeof(recvbuf) - used;
		int n;
		char *line_start, *line_end;

		if (space < 1)
			break;
		n = recv(client, recvbuf + used, (int)space, 0);
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
	printf("[*] vulnerable memcpy done (name len=%u)\n", (unsigned)name_len);
	return 0;
}
