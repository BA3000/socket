#pragma once

#include "XTcp.h"
#include "XHttpResponse.h"

class XHttpClient
{
public:
	XHttpClient();
	~XHttpClient();
	void Main();
	bool Start(XTcp client);
	XTcp client;
	XHttpResponse res;
};
