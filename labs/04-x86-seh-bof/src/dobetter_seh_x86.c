/*
 * dobetter_seh_x86.c — intentionally vulnerable Win32 SEH teaching server
 *
 * Lab use only. Mitigations off (no GS/ASLR/DEP). SEH chain must remain enabled
 * (do NOT link with --no-seh).
 *
 * Protocol: TCP 31340, newline-terminated lines (length-based copy; 0x00 OK).
 *
 * Bug: large stack buffer + installed SEH frame + forced access violation.
 * Overflow reaches nSEH / SEH (classic next-record + handler layout).
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
#define LPORT "31340"
#endif

#define RECVBUFSIZE 8192
#define BUF_SIZE 512

typedef struct _TEACHING_SEH_RECORD {
	struct _TEACHING_SEH_RECORD *next;
	void *handler;
} TEACHING_SEH_RECORD;

/* Proper-looking handler used only as a placeholder before overflow. */
static EXCEPTION_DISPOSITION NTAPI placeholder_handler(
	EXCEPTION_RECORD *rec,
	void *frame,
	CONTEXT *ctx,
	void *dispatch)
{
	(void)rec;
	(void)frame;
	(void)ctx;
	(void)dispatch;
	return ExceptionContinueSearch;
}

/*
 * Teaching gadgets (discover with mona; do not hardcode in exploits):
 *   POP reg; POP reg; RET  — classic SEH "pass-the-handler" trampoline
 *   JMP short-friendly bytes may also appear; prefer mona seh / mona popad
 */
#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
void gadget_pop_pop_ret(void);
__asm__(
	".text\n"
	".globl _gadget_pop_pop_ret\n"
	"_gadget_pop_pop_ret:\n"
	"  popl %eax\n"
	"  popl %eax\n"
	"  ret\n"
);
#elif defined(_MSC_VER) && defined(_M_IX86)
__declspec(naked) void gadget_pop_pop_ret(void)
{
	__asm {
		pop eax
		pop eax
		ret
	}
}
#endif

static void handle_connection(void *param);
static void trigger_seh_overflow(const char *data, size_t len);

int main(void)
{
	WSADATA wsa = { 0 };
	struct addrinfo hints;
	struct addrinfo *ainfo = NULL;
	SOCKET listen_sock;
	int result;

	printf("dobetter_seh_x86 - intentional SEH overwrite teaching server\n");
	printf("Listen: 0.0.0.0:%s | DEP/ASLR/GS off; SEH chain enabled\n", LPORT);

#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
	printf("[hint] gadget_pop_pop_ret @ %p (prefer !mona seh / !mona find -type instr)\n",
	       (void *)gadget_pop_pop_ret);
#elif defined(_MSC_VER) && defined(_M_IX86)
	printf("[hint] gadget_pop_pop_ret @ %p (prefer !mona seh)\n", (void *)gadget_pop_pop_ret);
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

	if (getaddrinfo(NULL, LPORT, &hints, &ainfo) != 0) {
		printf("getaddrinfo failed\n");
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

	printf("[+] Listening. Attach WinDBG, then overflow to nSEH/SEH and force AV.\n");

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
		"dobetter_seh_x86 ready. Send a line + \\n (or 'exit'). "
		"Long lines overwrite SEH chain; handler then crashes on purpose.\n";

	send(client, banner, (int)strlen(banner), 0);
	memset(recvbuf, 0, sizeof(recvbuf));

	for (;;) {
		size_t space = sizeof(recvbuf) - used;
		int n;
		char *line_start;
		char *line_end;

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
				const char *bye = "bye\n";
				send(client, bye, 4, 0);
				closesocket(client);
				return;
			}

			/* Echo a short ack then process (may never return if exploit works). */
			{
				const char *ack = "processing...\n";
				send(client, ack, (int)strlen(ack), 0);
			}
			trigger_seh_overflow(line_start, line_len);

			line_start = line_end + 1;
		}
		used -= (size_t)(line_start - recvbuf);
		memmove(recvbuf, line_start, used);
	}
	closesocket(client);
}

/*
 * Install a stack SEH record after a fixed buffer, copy without bounds checks,
 * then force an access violation so the (possibly overwritten) handler runs.
 *
 * Stack (high → low addresses for growth; write direction is low → high):
 *   [ buf[BUF_SIZE] ][ seh.next (nSEH) ][ seh.handler (SEH) ]
 */
static void trigger_seh_overflow(const char *data, size_t len)
{
	/*
	 * Pack buffer + SEH record so overflow layout is stable across compilers:
	 *   frame.buf[BUF_SIZE] | frame.seh.next (nSEH) | frame.seh.handler (SEH)
	 */
	struct {
		char buf[BUF_SIZE];
		TEACHING_SEH_RECORD seh;
	} frame;
	TEACHING_SEH_RECORD *prev;
	size_t i;

	memset(&frame, 0, sizeof(frame));
	frame.seh.next = (TEACHING_SEH_RECORD *)0xFFFFFFFF;
	frame.seh.handler = (void *)placeholder_handler;

	/* Install SEH frame at FS:[0] (x86 Windows). */
#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
	__asm__ volatile("movl %%fs:0, %0" : "=r"(prev));
	frame.seh.next = prev;
	__asm__ volatile("movl %0, %%fs:0" : : "r"(&frame.seh));
#elif defined(_MSC_VER) && defined(_M_IX86)
	prev = (TEACHING_SEH_RECORD *)__readfsdword(0);
	frame.seh.next = prev;
	__writefsdword(0, (unsigned long)&frame.seh);
#else
#error "x86 Windows only"
#endif

	/* VULNERABLE: unbounded copy into buf — continues into seh record. */
	for (i = 0; i < len; i++)
		((volatile char *)frame.buf)[i] = data[i];

	/* Keep frame live for the optimizer. */
	printf("[*] copied %u bytes into %u-byte buf (frame %p); forcing AV for SEH\n",
	       (unsigned)len, (unsigned)BUF_SIZE, (void *)&frame);
	printf("[*] nSEH slot @ %p  SEH slot @ %p\n",
	       (void *)&frame.seh.next, (void *)&frame.seh.handler);

	/* Force exception so the SEH chain is consulted. */
	*(volatile int *)0 = 0x41414141;

	/* Uninstall if we somehow continue (best-effort). */
#if defined(__GNUC__) && (defined(__i386__) || defined(_X86_))
	__asm__ volatile("movl %0, %%fs:0" : : "r"(frame.seh.next));
#endif
}
