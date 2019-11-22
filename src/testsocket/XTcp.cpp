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

XTcp::~XTcp()
{

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
	tcp.ip = inet_ntoa(caddr.sin_addr);
	tcp.port = ntohs(caddr.sin_port); // short��ǡ�����65535
	tcp.sock = client;
	printf("client ip is %s, port is %d \n", tcp.ip.c_str(), tcp.port);
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