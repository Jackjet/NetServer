#ifndef CMAIN_SERVER_H
#define CMAIN_SERVER_H

#include "CEventBaseMgr.h"
#include "openssl/ssl.h"
#include "locker.h"

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
	SSL* CreateSSL(evutil_socket_t& fd);
	//�¼��ص�
	static void EventCallBack(evutil_socket_t fd, short event, void* arg);

	static int SslRecv(SSL* ssl, char* buffer, int ilen);
	static int NormalRecv(evutil_socket_t& fd, char* buffer, int ilen);

	static int SslSend(SSL* ssl, const char* buffer, int ilen);
	static int NormalSend(evutil_socket_t& fd, const char* buffer, int ilen);

	//��ʱ��������ʱ�������ݽ���������
	static void TimeoutCallBack(evutil_socket_t fd, short event, void* arg);
	void OnTimeoutCallback();
#endif

private:
	event_base* m_pMainBase;

#ifndef WIN32
	CEventBaseMgr* m_pEventBaseMgr; //����event_base�Ĺ������
#endif
	
	evutil_socket_t m_listenerFd; //������sock

	int m_listenerport;	//�����˿�

	bool m_isInited;	//�Ƿ��ʼ��
};
#endif //CMAIN_SERVER_H