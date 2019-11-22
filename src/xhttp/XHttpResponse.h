#pragma once
#include <string>
class XHttpResponse
{
public:
	std::string GetHead();
	int Read(char *buf, int bufsize);
	bool SetRequest(std::string request);
	XHttpResponse();
	~XHttpResponse();

private:
	int filesize = 0;
	FILE *fp = NULL;
};

