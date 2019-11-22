#include "XHttpResponse.h"
#include <string>
#include <regex>

using namespace std;


std::string XHttpResponse::GetHead()
{
	// 回应http GET请求
	// 消息头
	string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n";
	rmsg += "Server: XHttp\r\n";
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	char bsize[128] = { 0 };
	sprintf(bsize, "%d", filesize);
	rmsg += bsize;
	//rmsg += "10\r\n";
	rmsg += "\r\n\r\n"; // 浏览器看到这里就知道是结束了
	return rmsg;
}

int XHttpResponse::Read(char * buf, int bufsize)
{
	// 发送正文
	return fread(buf, 1, bufsize, fp); // 1代表一次性读1个，一块多少个字节，读多少块
}

bool XHttpResponse::SetRequest(std::string request)
{
	string src = request;
	//string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
	string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
	regex r(pattern);
	smatch mas;
	regex_search(src, mas, r); // 源数据，结果，表达式
	if (mas.size() == 0) {
		printf("%s failed!\n", pattern.c_str());
		return false;
	}
	string type = mas[1];
	string path = "/";
	path += mas[2];
	string filetype = mas[3];
	string query = mas[4];
	printf("filetype:%s\n", filetype.c_str());
	if (type != "GET") {
		printf("Not GET!\n");
		return false;
	}
	string filename = path;
	if (path == "/") {
		filename = "/index.html";
	}

	string filepath = "www";
	filepath += filename;

	if (filetype == ".php") {
		printf("loading php file\n");
		string cmd = "php-cgi ";
		cmd += filepath;
		cmd += " ";

		// 将&替换为空格
		for (int i = 0; i < query.size(); ++i) {
			if (query[i] == '&') {
				query[i] = ' ';
			}
		}
		cmd += query;

		cmd += " > ";
		filepath += ".html";
		cmd += filepath;

		printf("%s\n", cmd.c_str());
		system(cmd.c_str());
	}

	fp = fopen(filepath.c_str(), "rb");
	if (fp == NULL) {
		printf("failed to read file %s! \n", filepath.c_str());
		return false;
	}
	// 获取文件大小
	fseek(fp, 0, SEEK_END); // 把指针移动到最末端，从而能够读取出文件大小
	filesize = ftell(fp); // 最大值有限制
	fseek(fp, 0, 0);
	printf("file size is %d\n", filesize);

	if (filetype == ".php") {
		char c = 0;
		int headsize = 0;
		while (fread(&c, 1, 1, fp) > 0) {
			++headsize;
			if (c == '\r') {
				fseek(fp, 3, SEEK_CUR);
				headsize += 3;
				break;
			}
		}
		filesize = filesize - headsize;
	}

	return true;
}

XHttpResponse::XHttpResponse()
{
}


XHttpResponse::~XHttpResponse()
{
}
