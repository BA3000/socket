#include "XTcp.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
// ����Linux
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
// ��ʼ���⣬�������ʼ���Ļ���ֱ�ӵ��º����socket�����޷�ʹ�ã������ڳ�ʼ��ǰ
// Ҫ����Windows������⣬��������Ŀ���������ws2_32.lib
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
	// ʹ��TCP/IPЭ�飬����AF_INET��TCP��������SOCK_STREAM
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// ����socketʧ�ܣ�����Linux����Ϊ������ÿ�����̷�����ļ��������������ܾ�����
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
	saddr.sin_port = htons(port); // host to network�������ֽ���ת���������ֽ���
	saddr.sin_addr.s_addr = htons(0); // ��ip��ַ��0�Ļ���ʵ���Բ�ת�������������ip�����������ݶ����ܵ���˼
									  // һ��int��4��char�����Կ���ͨ��int����ʾip��ַ

									  // bind�˿ڣ�������ʧ�ܣ�һ��Ҫ���ж�
	if (::bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) {	// :: ��ʾ�õ���ȫ�ֵĺ���
		printf("bind port %d failed!", port);
		return false;
	}
	printf("bind port %d succeeded.", port);
	listen(sock, 10); // ����ָ���Ķ˿ڣ�ֻ������������
	return true;
}

XTcp XTcp::Accept()
{
	XTcp tcp;
	sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int client = accept(sock, (sockaddr*)&caddr, &len); // ��ȡ�û�������Ϣ���ᴴ���µ�socket����������������ͻ���ͨ�ţ���������
														// ����Ҫ��ָ�룬�������ض˿ںź͵�ַ
	if (client <= 0) {
		return tcp;
	}
	printf("accept client %d\n", client);
	char *ip = inet_ntoa(caddr.sin_addr);
	strcpy(tcp.ip, ip);
	tcp.port = ntohs(caddr.sin_port); // short��ǡ�����65535
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
	fd_set set; // �ļ����������
	if (connect(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) {
		FD_ZERO(&set);
		FD_SET(sock, &set);
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;
		if (select(sock + 1, 0, &set, 0, &tm) <= 0) {
			// ֻҪ��һ����д���ͻ᷵���ļ���������ֵ�����򷵻�-1����ʱ����0
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
	// ������Linux�е�����������ʽ�Ĵ���
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		return false;
	}
	if (isblock) {
		flags = flags&~O_NONBLOCK;
	}
	else {
		flags = flags | O_NONBLOCK; // ������ģʽ
	}
	if (fcntl(sock, F_SETFL, flags) != 0) {
		return false; // ���������0����ô�趨ʧ��
	}
#endif
	return true;
}



XTcp::~XTcp()
{

}