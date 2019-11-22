/*********************************************************************************************
**                                                                                          **
**                                 Jiedi(China nanjing)Ltd.                                 **
**	                          鐗堟潈褰掑鏇逛繆鎵€鏈夛紝姝や唬鐮佷綔涓哄涔犲弬鑰?                         **                                                                            **
*********************************************************************************************/

/*****************************    鑱旂郴鏂瑰紡   *************************************************
** 璐拱璇剧▼鐨勫悓瀛﹀彲浠ュ姞缇?98872593鍜ㄨ璁ㄨ缃戠粶缂栫▼鎶€鏈棶棰橈紝鑰佸笀涓€鏈夋椂闂村氨浼氳В绛旀偍鐨勯棶棰?
** 鍔犵兢璇锋敞鏄庤喘涔板钩鍙板拰璐拱璐﹀彿锛屼竴鑸竴澶╁唴灏变細瀹℃牳閫氳繃
** 濡傛灉瑙夊緱璇剧▼鍐呭瀵规偍鏈夊府鍔╋紝甯屾湜缁欏嚭浜旀槦濂借瘎锛屽苟鐣欎笅鎮ㄧ殑鎰忚锛屾垜浼氭牴鎹偍鐨勬剰瑙佽皟鏁翠慨鏀硅绋?
** Contact       : xiacaojun@qq.com
**        鍗氬   : http://blog.csdn.net/jiedichina
**        QQ	 : 99981441
**		  QQ缇? 锛?98872593
**   寰俊鍏紬鍙? : jiedi2007
**
********************************************************************************************/
#include "XHttpResponse.h"
#include <string>
#include <regex>
using namespace std;
bool XHttpResponse::SetRequest(std::string request)
{
	string src = request;
	// /   /index.html /ff
	string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
	regex r(pattern);
	smatch mas;
	regex_search(src,mas,r);
	if(mas.size() == 0)
	{
		printf("%s failed!\n",pattern.c_str());
		return false ;
	}
	string type = mas[1];
	string path = "/";
	path += mas[2];
	string filetype = mas[3];
	string query = mas[4];

	if(type != "GET")
	{
		printf("Not GET!!!\n");
		return false;
	}
	string filename = path;
	if(path == "/")
	{
		filename = "/index.html";
	}
	
	string filepath = "www";
	filepath += filename;
	
	//php-cgi www/index.php id=1 name=xcj >  www/index.php.html
	if(filetype == "php")
	{
		string cmd = "php-cgi ";
		cmd += filepath;
		cmd += " ";
		//query id=1&name=xcj 
		// id=1 name=xcj
		for(int i =0; i< query.size(); i++)
		{
			if(query[i] == '&') query[i] = ' ';
		}
		cmd += query;
	
		cmd += " > ";
		filepath += ".html";
		cmd += filepath;
	
		//printf("%s\n",cmd.c_str());
		system(cmd.c_str());
	}
	
	
	fp = fopen(filepath.c_str(),"rb");
	printf("open file %s \n", filepath.c_str());
	if(fp == NULL)
	{
		printf("open file %s failed!\n",filepath.c_str());
		return false;
	}
	//鑾峰彇鏂囦欢澶у皬
	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,0);
	printf("file size is %d\n",filesize);
	
	if(filetype == "php")
	{
		char c = 0;
		//\r\n\r\n
		int headsize = 0;
		while(fread(&c,1,1,fp) > 0)
		{
			headsize++;
			if(c=='\r')
			{
				fseek(fp,3,SEEK_CUR);
				headsize += 3;
				break;
			}
		}
		filesize = filesize - headsize;
	}

	return true;
}
std::string XHttpResponse::GetHead()
{

	string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n";
	rmsg += "Server: XHttp\r\n";
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	char bsize[128] = { 0 };
	sprintf(bsize, "%d", filesize);
	rmsg += bsize;
	//rmsg += 
	//rmsg += "10\r\n";
	rmsg += "\r\n\r\n";
	return rmsg;
}
int XHttpResponse::Read(char *buf, int bufsize)
{

	return fread(buf, 1, bufsize, fp);
}
XHttpResponse::XHttpResponse()
{
}


XHttpResponse::~XHttpResponse()
{
}
