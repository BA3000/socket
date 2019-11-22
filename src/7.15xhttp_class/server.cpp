/*********************************************************************************************
**                                                                                          **
**                                 Jiedi(China nanjing)Ltd.                                 **
**	                          版权归夏曹俊所有，此代码作为学习参考                          **                                                                            **
*********************************************************************************************/

/*****************************    联系方式   *************************************************
** 购买课程的同学可以加群398872593咨询讨论网络编程技术问题，老师一有时间就会解答您的问题
** 加群请注明购买平台和购买账号，一般一天内就会审核通过
** 如果觉得课程内容对您有帮助，希望给出五星好评，并留下您的意见，我会根据您的意见调整修改课程
** Contact       : xiacaojun@qq.com
**        博客   : http://blog.csdn.net/jiedichina
**        QQ	 : 99981441
**		  QQ群	 ：398872593
**   微信公众号  : jiedi2007
**
********************************************************************************************/
#include <stdlib.h>
#include "XTcp.h"
#include <stdio.h>
#include "XHttpServer.h"
using namespace std;
#include <signal.h>
int main(int argc, char *argv[])
{
	signal(SIGPIPE,SIG_IGN);
	unsigned short port = 8080;

	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	XHttpServer server;
	server.Start(port);
	getchar();
	return 0;
}
