#include "wLog.h"
#include "CMainEventBase.h"
#include "CMgrRequest.h"
#include "CRedisServiceMgr.h"
#include "CSqlServiceMgr.h"
#include "CConfiger.h"
#include "Common.h"
#include "event2/thread.h"

#include <stdio.h>
#include <cstring> 

#define LISTEN_PORT 8080
#define THREAD_NUM 5
#define CONFIG_FILE "server.conf"


//windows ���ǵ��̵߳ķ���
//linux���Ƕ��event_base�����̣߳�ʹ�ùܵ���ʽȥ���˼����ͽ���

/*
* ��������
* ��������ʵ���ѱ�֤�̰߳�ȫ�����������������н�������
*/
void SingletonInit()
{
	CConfiger::GetInstance();
	CMgrRequest::GetInstance();
	CRedisServiceMgr::GetInstance();
	CSqlServiceMgr::GetInstance();
}

/*
* ��������
*/
void SingletonUnInit()
{
	CMgrRequest::DelInstance();
	CRedisServiceMgr::DelInstance();
	CSqlServiceMgr::DelInstance();
	CConfiger::DelInstance();
}

//��ʼ��ϵͳ��ģ�����
void SystemInit()
{
	SingletonInit();

	if(CConfiger::GetInstance()->OnInit(CONFIG_FILE) != true)
	{
		SingletonUnInit();
		exit(-1);
	}

	if (CConfiger::GetInstance()->GetEnableSSL())
	{
		SSLeay_add_ssl_algorithms();  
		OpenSSL_add_all_algorithms();  
		SSL_load_error_strings();  
		ERR_load_BIO_strings();  
	}

	Common::initBase64();
	CSqlServiceMgr::GetInstance()->OnInit();
	CRedisServiceMgr::GetInstance()->OnInit();

#ifdef WIN32
	//windows��ʼ�����绷��
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		exit(-1);
	}
	printf("Server Running in WONDOWS\n");
#else
	printf("Server Running in LINUX\n");
#endif
}

//����ʼ��ϵͳ��ģ�����
void SystemQuit()
{
	SingletonUnInit();
}


int main(int argc, char** argv)  
{ 
	int iListenerPort = LISTEN_PORT;
	int iThreadNum = THREAD_NUM;

	if (argc >= 2 && argv[1])
	{
		iListenerPort = atoi(argv[1]);
	}

	//��־��¼��ʼ��
	int port = iListenerPort;
	char buffer[32] = {0};
	sprintf(buffer, "%s_%d", argv[0], iListenerPort);
	WLogInit("./SealServerLog",buffer, 10, 1000, 1024);

	bool IsStart = false;
	CMainEventBase* pMainServer = new CMainEventBase();
	do 
	{
		SystemInit();
		if(pMainServer)
		{
			if (CConfiger::GetInstance()->GetEnableSSL())
			{
				WLogInfo("*******************Server Start enable_ssl!********************\n");
			}
			else
			{
				WLogInfo("*******************Server Start disenable_ssl!********************\n");
			}

			iThreadNum = CConfiger::GetInstance()->GetCfgThreads();
#ifndef WIN32
			if(pMainServer->OnInit(iListenerPort, iThreadNum) != true)
				break;
#else
			if(pMainServer->OnInit(LISTEN_PORT, 1) != true)
				break;
#endif
			pMainServer->OnStart();		//event_base_dispatch ����
			pMainServer->OnStop();
			IsStart = true;
		}
		else
		{
			printf("main::new CMainEventBase error!\n");
		}
	} while (0);

	if(IsStart)
	{
		SystemQuit();
	}

	delete pMainServer;
	pMainServer = NULL;

	return 0;
} 