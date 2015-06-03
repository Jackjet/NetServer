#include "wLog.h"
#include "CMainEventBase.h"
#include "CMgrRequest.h"
#include "CRedisServiceMgr.h"
#include "CSqlServiceMgr.h"
#include "CRcdServiceMgr.h"
#include "CConfiger.h"
#include "Common.h"
#include "event2/thread.h"

#include <stdio.h>
#include <cstring> 

#define LISTEN_PORT 11181
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
	if (CConfiger::GetInstance()->GetEnableRecord())
		CRcdServiceMgr::DelInstance();
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
	Common::initBase64();
	CSqlServiceMgr::GetInstance()->OnInit();
	CRedisServiceMgr::GetInstance()->OnInit();
	if (CConfiger::GetInstance()->GetEnableRecord())
	{
		CRcdServiceMgr::GetInstance();
	}
}

//����ʼ��ϵͳ��ģ�����
void SystemQuit()
{
	SingletonUnInit();
}


int main(int argc, char** argv)  
{ 
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

	int iListenerPort = LISTEN_PORT;
	int iThreadNum = THREAD_NUM;
	bool enablessl = false;
	if (argc >= 2 && argv[1])
	{
		iListenerPort = atoi(argv[1]);
	}
	if (argc >= 3 && argv[2])
	{
		int iEnable = atoi(argv[2]);
		enablessl = (iEnable == 1 ? true : false);
	}

	//��־��¼��ʼ��
	int port = iListenerPort;
	char buffer[32] = {0};
	sprintf(buffer, "%s%d", argv[0], iListenerPort);
	WLogInit("./SealServerLog",buffer, 10, 1000, 1024);

	
	bool IsStart = false;
	CMainEventBase* pMainServer = new CMainEventBase();
	do 
	{
		SystemInit();
		if(pMainServer)
		{
			iThreadNum = CConfiger::GetInstance()->GetCfgThreads();
			printf("Main::iThreadNum = %d\n", iThreadNum);
#ifndef WIN32
			if(pMainServer->OnInit(iListenerPort, iThreadNum, enablessl) != true)
				break;
#else
			if(pMainServer->OnInit(LISTEN_PORT, 1) != true)
				break;
#endif
			if (CConfiger::GetInstance()->GetEnableRecord())
			{
				CRcdServiceMgr::GetInstance()->OnStart();
			}
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