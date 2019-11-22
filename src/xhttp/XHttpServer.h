#pragma once

#include "XTcp.h"

class XHttpServer
{
public:
	bool Start(unsigned short port);
	void Main();
	void Stop();
	XHttpServer();
	~XHttpServer();
	XTcp server;
	bool isexit = false;
};

