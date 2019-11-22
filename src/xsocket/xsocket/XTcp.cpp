#include "XTcp.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
// 兼容Linux
#include <Windows.h>
#define socklen_t int
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define closesocket close
#endif

#include <thread>

XTcp::XTcp()
{
// 初始化库，如果不初始化的话会直接导致后面的socket函数无法使用，但是在初始化前
// 要加载Windows的网络库，就是在项目属性那里加ws2_32.lib
#ifdef WIN32
	static bool first = true;
	if (first) {
		WSADATA ws;
		WSAStartup(MAKEWORD(2, 2), &ws);
		first = false;
	}
#endif
}

int XTcp::CreateSocket() {
	// 使用TCP/IP协议，所以AF_INET，TCP，所以是SOCK_STREAM
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// 创建socket失败，例如Linux中因为超出了每个进程分配的文件具体数量而被拒绝创建
	if (sock == -1) {
		printf("Create socket failed!\n");
	}
	return sock;
}

bool XTcp::Bind(unsigned short port) {
	if (sock <= 0) {
		CreateSocket();
	}
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port); // host to network，本地字节序转换成网络字节序
	saddr.sin_addr.s_addr = htons(0); // 绑定ip地址，0的话其实可以不转。这里是任意的ip发过来的数据都接受的意思
									  // 一个int是4个char，所以可以通过int来表示ip地址

									  // bind端口，很容易失败，一定要有判断
	if (::bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) {	// :: 表示用的是全局的函数
		printf("bind port %d failed!", port);
		return false;
	}
	printf("bind port %d succeeded.", port);
	listen(sock, 10); // 监听指定的端口，只用来创建链接
	return true;
}

XTcp XTcp::Accept()
{
	XTcp tcp;
	sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int client = accept(sock, (sockaddr*)&caddr, &len); // 读取用户连接信息，会创建新的socket，用来单独和这个客户端通信，后面两个
														// 参数要传指针，用来返回端口号和地址
	if (client <= 0) {
		return tcp;
	}
	printf("accept client %d\n", client);
	char *ip = inet_ntoa(caddr.sin_addr);
	strcpy(tcp.ip, ip);
	tcp.port = ntohs(caddr.sin_port); // short，恰好最大65535
	tcp.sock = client;
	printf("client ip is %s, port is %d \n", tcp.ip, tcp.port);
	return tcp;
}

int XTcp::Recv(char* buf, int bufsize) {
	return recv(sock, buf, bufsize, 0);
}

int XTcp::Send(const char* buf, int size) {
	int s = 0;
	while(s != size) {
		int len = send(sock, buf + s, size - s, 0);
		if (len <= 0) {
			break;
		}
		s += len;
	}
	return s;
}

void XTcp::Close() {
	if (sock <= 0) return;
	closesocket(sock);
}

bool XTcp::Connect(const char * ip, unsigned short port, unsigned int timeoutms)
{
	if (sock <= 0) {
		CreateSocket();
	}
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	SetBlock(false);
	fd_set set; // 文件距离的数组
	if (connect(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) {
		FD_ZERO(&set);
		FD_SET(sock, &set);
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;
		if (select(sock + 1, 0, &set, 0, &tm) <= 0) {
			// 只要有一个可写，就会返回文件描述符的值，否则返回-1，超时返回0
			printf("connect timeout or error!\n");
			printf("connect %s:%d failed!: %s\n", ip, port, strerror(errno));
			return false;
		}
	}
	SetBlock(true);
	printf("connect %s:%d succeded!\n", ip, port);
	return true;
}

bool XTcp::SetBlock(bool isblock)
{
	if (sock <= 0) {
		return false;
	}
#ifdef WIN32
	unsigned long ul = 0;
	if (!isblock) {
		ul = 1;
	}
	ioctlsocket(sock, FIONBIO, &ul);
	// 下面是Linux中的设置阻塞方式的代码
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		return false;
	}
	if (isblock) {
		flags = flags&~O_NONBLOCK;
	}
	else {
		flags = flags | O_NONBLOCK; // 非阻塞模式
	}
	if (fcntl(sock, F_SETFL, flags) != 0) {
		return false; // 如果不等于0，那么设定失败
	}
#endif
	return true;
}



XTcp::~XTcp()
{

}