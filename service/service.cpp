// service.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "conio.h"                                                      
#include <stdio.h>
#include <WinSock2.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 100
SOCKET servSock;
SOCKET clntSock;
int Rvalue = 1;           //用于接收函数返回值
int startReturn = -1;     //判定服务器是否成功启动





//函数前置声明
void menu();
int start();             //启动服务器
void acceptclient();     //接收客户端请求
void stop();
void sendfile();
void talk_with();


int main() {
	int n;
	menu();
	scanf("%d", &n);
	while (n)
	{
		switch (n)
		{
		case 1:
			if (!start())
				startReturn = 0;
			//printf("测试用%d\n", startReturn);      //测试用
			break;
		case 2:
			acceptclient();
			break;
		case 3:
			talk_with();
			break;
		case 4:
		    sendfile();
			break;
		case 5:
			system("cls"); menu();
			break;
		case 6:
			 stop();
			break;
		case 7:
			 exit(0);
			break;
		
		default:printf("输入错误！！请重新输入。\n");
		}
		menu();/*执行完功能再次显示菜单界面*/
		scanf("%d", &n);
	}

	return 0;
}

//启动服务器
int start() 
{
	//printf("开始执行stat函数\n");          //测试用
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf("服务器启动中、、、、、、、、\n\n\n");
	Sleep(2000);         //停顿2秒钟，造成服务器启动假象
						 //创建套接字
	servSock = socket(AF_INET, SOCK_STREAM, 0);//协议域、指定socket类型、指定协议
	//绑定套接字
	sockaddr_in sockAddr;
	//memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;  //使用IPv4地址
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址 
	sockAddr.sin_port = htons(5001);  //端口
									  //接收绑定的返回值
	Rvalue = bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	if (Rvalue)  //接收返回值，成功则返回0, 失败返回-1
	{
		printf("服务器绑定端口失败\n");
		return -1;
	}
	printf("服务器绑定端口成功\n");
	//进入监听状态
	Rvalue = listen(servSock, 20);//可以有20个客户端连上来
	if (Rvalue)
	{
		printf("监听端口失败\n");
		return -1;
	}
	printf("开始监听端口，等待连接······\n");
	//printf("start函数执行完毕\n");     //测试用
	return 0;

	
}

//接收客户端请求
void acceptclient()
{
	//printf("开始执行acceptclient函数\n");          //测试用
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	char sendBuf[BUF_SIZE] = "连接服务器成功\r\n";
	if (!startReturn) 
	{
		printf("等待连接中、、、、、、");
		//接受连接
		while (1)
		{
			clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);//2.用于返回客户端的协议地址3.协议地址的长度
			if (clntSock == -1)
			{
				printf("客户连接失败\n");
				system("cls");
				continue;
			}
			system("cls");
			
			printf("客户端已成功连接到服务器\n");
			//printf("服务器:%s\n",sendBuf);
			send(clntSock, sendBuf, strlen(sendBuf) + 1, 0);
			break;	

		}

	}
	else
	{
		printf("服务器尚未启动...\n");
		return ;
	}

	//printf("acceptclient函数执行完毕\n");//测试用
}

//信息交互
void talk_with()
{

	char buffer[BUF_SIZE];  //发送缓冲区
	char recvBuf[BUF_SIZE]; //接收缓冲区
	int timeOut = 3;                //超时
	int strLen;                     //接收缓冲区的值
	//交互通信
	while (1)
	{
		// 判断键盘是否有输入，有则进入发送模式
		if (kbhit())//系统函数获取键盘输入值
		{
			printf("服务器:");
			gets_s(buffer);
			send(clntSock, buffer, strlen(buffer) + 1, 0);
			if (!strcmp(buffer, "bye") || !strcmp(buffer, "再见") || !strcmp(buffer, "quit"))
			{
				system("cls");

				menu();
				closesocket(clntSock);
				stop();
				break;
			}

		}

		//设置recv的超时时间，超时则返回，不等待
		setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(int));
		strLen = recv(clntSock, recvBuf, BUF_SIZE, 0);  //接收客户端的数据

		if (strLen > 0)
		{
			recvBuf[strLen] = '\0';
			printf("客户机:%s\n", recvBuf);
			if (!strcmp(recvBuf, "bye") || !strcmp(recvBuf, "再见") || !strcmp(recvBuf, "quit"))
			{
				system("cls");

				menu();
				closesocket(clntSock);
				printf("客户端已断开连接!!\n");
				break;
			}
		}
	}
}

//菜单
void menu()
{
	printf("\n\n");
	printf("------网络通信服务器端------\n");
	printf("\t1、启动服务器\n");
	printf("\t2、开始同意客户端连接\n");
	printf("\t3、开始与客户端对话\n");
	printf("\t4、向客户端发送文件\n");
	printf("\t5、返回主菜单\n");
	printf("\t6、关闭服务器\n");
	printf("\t7、退出系统\n");
	printf("\t请选择：");

}

//关闭连接
void stop()
{
	if (!startReturn) closesocket(servSock);
	WSACleanup();               //终止 DLL 的使用
	printf("服务器已关闭\n");
}

//发送文件
void sendfile()
{
	char fl[100];
	if (!startReturn)
	{

		printf("请输入要发送的文件的路径和文件名（如：D:\\send.txt）：");
		scanf("%s", &fl);
		printf("输入的路径为：%s\n", fl);
		//检查文件是否存在
		char *filename = fl;               //文件名
		FILE *fp = fopen(filename, "rb");  //以二进制方式打开文件
		if (fp == NULL) {
			printf("找不到文件, 按任意键返回主菜单!\n");
			getch();
			
		}
		else
		{

		
			printf("文件发送中····\n");
			char buffer[BUF_SIZE] = { 0 };  //缓冲区
			int nCount;
			//循环发送数据，直到文件结尾
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) 
			{
				send(clntSock, buffer, nCount, 0);
			}
			shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
			//recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
			fclose(fp);
			printf("文件发送成功。");
		}
	}
	else
	{
		printf("服务器尚未启动....\n");
	}
}