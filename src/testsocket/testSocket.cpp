// testSocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "XTcp.h"

#ifdef WIN32
// 兼容Linux
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

class TcpThread {
public:
	void Main() {
		char buf[1024] = { 0 };

		for (;;) {
			int recvlen = client.Recv(buf, sizeof(buf) - 1); // Win中没有read，虽然Linux下都可以用，但是为了兼容所以用recv
																 // buf的大小减一是为了接受字符串，要保存最后面的\0，最后一个参数与自定义的操作有关，这里不管
																 // len用来保存接收到的数据长度
			if (recvlen <= 0) {
				break;
			}
			buf[recvlen] = '\0';
			if (strstr(buf, "quit") != NULL) {
				char re[] = "quit success!\n";
				client.Send(re, strlen(re) + 1); // 回复ok，可以发3个字节
				break;
			}
			int sendlen = client.Send("ok\n", 4); // 回复ok，可以发3个字节
			printf("recv %s\n", buf);
		}
		client.Close();
		delete this;
	}
	XTcp client;
};

int main(int argc, char *argv[])
{
	unsigned short port = 8080;

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	XTcp server;
	server.CreateSocket();
	server.Bind(port);
	for (;;) {
		XTcp client = server.Accept();
		TcpThread *th = new TcpThread();
		th->client = client;
		// 创建线程
		std::thread sth(&TcpThread::Main, th);

		// 释放父线程拥有的子线程资源
		sth.detach();
	}


	getchar();
	return 0;
}

