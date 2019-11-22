#include "XTcp.h"
#include <stdlib.h>
#include <iostream>

int main() {
	XTcp client;
	client.CreateSocket();
	//client.SetBlock(true);

	client.Connect("192.168.56.101", 8080);
	client.Send("client", 6);
	char buf[1024] = { 0 };
	client.Recv(buf, sizeof(buf));
	printf("%s\n", buf);
	getchar();
	return 0;
}