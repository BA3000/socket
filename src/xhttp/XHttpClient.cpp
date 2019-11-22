#include "XHttpClient.h"
#include <thread>

using namespace std;

void XHttpClient::Main() {
	char buf[10240] = { 0 };
	for (;;) {
		int len = client.Recv(buf, sizeof(buf) - 1);

		if (len <= 0) {
			break;
		}
		buf[len] = '\0';
		if (!res.SetRequest(buf)) {
			break;
		}
		string head = res.GetHead();
		if (client.Send(head.c_str(), head.size()) <= 0) {
			break;
		}
		int size = sizeof(buf);
		bool error = false;
		for (;;) {
			len = res.Read(buf, size);
			if (len < 0) {
				error = true;
				break;
			}
			// 读到结尾
			if (len == 0) {
				break;
			}
			if (client.Send(buf, len) <= 0) {
				error = true;
				break;
			}
		}
	}
	
	client.Close();
	delete this;
}

bool XHttpClient::Start(XTcp client)
{
	this->client = client;
	thread sth(&XHttpClient::Main, this);
	sth.detach();

	return true;
}

XHttpClient::XHttpClient()
{
}


XHttpClient::~XHttpClient()
{
}
