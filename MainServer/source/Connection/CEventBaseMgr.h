#ifndef CEVENT_BASE_MGr_H
#define CEVENT_BASE_MGr_H

#include "event2/event.h"
#include "event2/bufferevent.h" 
#include "locker.h"
#include <cstdlib>
#include <cstring>
#include <vector>
#ifndef WIN32
#include <unistd.h>
#endif


/*
* event_base �������
*/

class CMgrRequest;

typedef struct  
{
	event_base* pBase;
	void* pParam;
	struct event* pNotifyEvt;
	pthread_t pid;
	int recvfd;
	int sendfd;
}LibEvThread;

class CEventBaseMgr
{
public:
	CEventBaseMgr(const int iNum, bool enablessl = false);

	~CEventBaseMgr();

	void OnStart();

	void OnStop();

	bool PushTask(evutil_socket_t sockfd);

protected:

	void LibEvInit();

	//�ܵ��ɶ�����������ֵΪ�½������ӵ�sockfd
	static void PipeReadCallBack(evutil_socket_t fd, short event, void* arg);
	void OnPipeReadCallback(evutil_socket_t& fd, void* arg);

	//event_base_dispatch�����߳�
	static void* ThreadFun(void* arg);

	/*event_base�ص�����*/
	//�����ݻص�
	static void SocketReadCallBack(bufferevent* bev, void* arg);
	void OnReadCallback(bufferevent*& bev);

	//д���ݻص�,���п���
	static void SocketWirteCallBack(bufferevent* bev, void* arg);
	void OnWirteCallback(bufferevent*& bev);

	//�쳣�ص����˻ص���ֱ�ӹر������ͷ�bufferevent
	static void SocketEventCallBack(struct bufferevent *bev, short event, void *arg);
	void OnEventCallback(bufferevent*& bev, short& event);

	//��ʱ��������ʱ�������ݽ���������
	static void TimeoutCallBack(evutil_socket_t fd, short event, void* arg);
	void OnTimeoutCallback();
private:
	LibEvThread* m_pLibEvThreads;

	int m_iThreadnum;
	bool m_isStop;
	bool m_enablessl;

	CMgrRequest* m_pRequestMgr;
};
#endif //CEVENT_BASE_MGr_H