#ifndef CMAIN_SERVER_H
#define CMAIN_SERVER_H

#include "locker.h"

#ifndef WIN32
#include "CEventBaseMgr.h"
#endif

#include "event2/listener.h"
#include "event2/thread.h"
#include "event2/bufferevent.h" 
#include <vector>

class CMainEventBase
{
public:
	CMainEventBase();

	~CMainEventBase();

	//��ʼ��,�����Ķ˿ڣ�event_base����
	bool OnInit(int iPort, int iThNum, bool enablessl = false);

	//��������
	void OnStart();

	//�˳�����
	void OnStop();

protected:
	bool TcpInit();

	static void AcceptCallBack(evutil_socket_t fd, short events, void* arg);
	void OnAcceptCallback(evutil_socket_t& fd, short events);

#ifndef WIN32
	void OnLinuxStart();
	void DoAccept();
#else
	void OnWindowsStart();
	/*event_base�ص�����*/
	//�����ݻص�
	static void SocketReadCallBack(bufferevent* bev, void* arg);
	void OnReadCallback(bufferevent*& bev);

	//д���ݻص�
	static void SocketWirteCallBack(bufferevent* bev, void* arg);
	void OnWirteCallback(bufferevent*& bev);

	//�쳣�ص����˻ص���ֱ�ӹر������ͷ�bufferevent
	static void SocketEventCallBack(struct bufferevent *bev, short event, void *arg);
	void OnEventCallback(bufferevent*& bev, short& event);

	//��ʱ��������ʱ�������ݽ���������
	static void TimeoutCallBack(evutil_socket_t fd, short event, void* arg);
	void OnTimeoutCallback();
#endif

private:
	event_base* m_pMainBase; //���߳�event_base��ֻ����accept����

#ifndef WIN32
	CEventBaseMgr* m_pEventBaseMgr; //����event_base�Ĺ������
#endif
	
	evutil_socket_t m_listenerFd; //������sock

	int m_listenerport;	//�����˿�

	bool m_isInited;	//�Ƿ��ʼ��
};


#endif //CMAIN_SERVER_H