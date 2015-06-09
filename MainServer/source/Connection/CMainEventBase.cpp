#include "CMainEventBase.h"
#include "CMgrRequest.h"
#include "CConfiger.h"
#include "wLog.h"

#include <stdio.h>
#include <errno.h>
#define LISTEN_FLAG 1024

#ifdef WIN32
#define CA_CERT_FILE "server/ca.crt"
#define SERVER_CERT_FILE "server/server.crt"
#define SERVER_KEY_FILE "server/server.key"
#endif

CMainEventBase::CMainEventBase()
{
	m_listenerFd = -1;
	m_pMainBase = NULL;
	m_isInited = false;
#ifndef WIN32
	m_pEventBaseMgr = NULL;
#endif
}

CMainEventBase::~CMainEventBase()
{
#ifndef WIN32
	if(m_pEventBaseMgr)
		delete m_pEventBaseMgr;
	m_pEventBaseMgr = NULL;
#endif

}

//初始化
bool CMainEventBase::OnInit(int iPort, int iThNum, bool enablessl)
{
	m_listenerport = iPort;
#ifndef WIN32
	//创建event_base管理对象
	m_pEventBaseMgr = new CEventBaseMgr(iThNum);
	if (!m_pEventBaseMgr)
	{
		return false;
	}
#endif

	m_isInited = TcpInit();;

	return m_isInited;
}

typedef struct sockaddr SA;
bool CMainEventBase::TcpInit()
{
	evutil_socket_t listener;
	do{
		listener = ::socket(AF_INET, SOCK_STREAM, 0);
		if( listener == -1 )
			return false;

		//允许多次绑定同一个地址。要用在socket和bind之间
		evutil_make_listen_socket_reuseable(listener);

		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = 0;
		sin.sin_port = htons(m_listenerport);

		if( ::bind(listener, (SA*)&sin, sizeof(sin)) < 0 )
			break;

		if( ::listen(listener, LISTEN_FLAG) < 0)
			break;

#ifdef WIN32
		//跨平台统一接口，将套接字设置为非阻塞状态
		evutil_make_socket_nonblocking(listener);
#endif
		m_listenerFd = listener;

		return true;
	}while(0);

	evutil_closesocket(listener);
	WLogInfo("CMainEventBase::TcpInit!\nstart listener==>port = %d, errno = %d\n", m_listenerport, errno);

	return false;
}

#ifndef WIN32
void CMainEventBase::DoAccept()
{
	struct sockaddr_in chiaddr;
	socklen_t clilen = sizeof( chiaddr );
	evutil_socket_t connfd = -1;
	while(true)
	{
		connfd = -1;
		if( ( connfd  = accept( m_listenerFd, (struct sockaddr *)&chiaddr, &clilen ) ) == -1 )
		{                                        //!> accept 返回的还是套接字
			static int iAcptCount;
			perror("CMainEventBase::accept error! \n");
			WLogError("CMainEventBase::OnAcceptCallback::accept error! iAcptCount = %d\n", iAcptCount++);
			continue;
		}
		m_pEventBaseMgr->PushTask(connfd);
	}
}

void CMainEventBase::OnLinuxStart()
{
	m_pEventBaseMgr->OnStart();
	DoAccept();
}
#else

void CMainEventBase::OnWindowsStart()
{
	m_pMainBase = event_base_new();

	//添加监听客户端请求连接事件
	struct event* ev_listen = event_new(m_pMainBase, m_listenerFd, EV_READ | EV_PERSIST, AcceptCallBack, this);
	event_add(ev_listen, NULL);
	//注册定时器回调
	struct timeval tv;
	tv.tv_sec=10; //间隔10s
	tv.tv_usec=0;
	struct event* evtime = event_new(NULL, -1, 0, NULL, NULL);
	event_assign(evtime, m_pMainBase, -1, EV_TIMEOUT | EV_PERSIST, 
		TimeoutCallBack, this);
	event_add(evtime,&tv);


	WLogInfo("CMainEventBase::OnStart::listenerPort = %d, pthreadID = %d\n", m_listenerport, pthread_self());
	event_base_dispatch(m_pMainBase);

	event_base_free(m_pMainBase);
	m_pMainBase = NULL;
}
#endif

//启动服务
void CMainEventBase::OnStart()
{
	if( m_listenerFd == -1 )
	{
		perror(" tcp_server_init error ");
		exit(-1);
	}	

#ifndef WIN32
	OnLinuxStart();

#else
	OnWindowsStart();

#endif
}

//退出服务
void CMainEventBase::OnStop()
{
	if(!m_isInited)
		return;

	evutil_closesocket(m_listenerFd);

#ifndef WIN32
	m_pEventBaseMgr->OnStop();

#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500;
	event_base_loopexit(m_pMainBase, &tv);
#endif
}


#ifdef WIN32
SSL* CMainEventBase::CreateSSL(evutil_socket_t& fd)
{
	if (!CConfiger::GetInstance()->GetEnableSSL())
	{
		return NULL;
	}

	SSL_CTX* ctx = NULL;  
	SSL* ssl = NULL; 
	ctx = SSL_CTX_new (SSLv23_method());
	if( ctx == NULL)
	{
		printf("SSL_CTX_new error!\n");
		return NULL;
	}

	// 要求校验对方证书  
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);  

	// 加载CA的证书  
	if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
	{
		SSL_CTX_free(ctx);
		printf("SSL_CTX_load_verify_locations error!\n");
		return NULL;
	}

	// 加载自己的证书  
	if(SSL_CTX_use_certificate_file(ctx, SERVER_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
	{
		SSL_CTX_free(ctx);
		printf("SSL_CTX_use_certificate_file error!\n");
		return NULL;
	}

	// 加载自己的私钥  
	if(SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
	{
		SSL_CTX_free(ctx);
		printf("SSL_CTX_use_PrivateKey_file error!\n");
		return NULL;
	}

	// 判定私钥是否正确  
	if(!SSL_CTX_check_private_key(ctx))
	{
		SSL_CTX_free(ctx);
		printf("SSL_CTX_check_private_key error!\n");
		return NULL;
	}

	// 将连接付给SSL  
	ssl = SSL_new (ctx);
	if(!ssl)
	{
		SSL_CTX_free(ctx);
		printf("SSL_new error!\n");
		return NULL;
	}

	SSL_set_fd(ssl, fd);  
	int count = 0;
	while(true)
	{
		if(SSL_accept(ssl) != 1)
		{
			if(count > 1000)
			{
				int icode = -1;
				int iret = SSL_get_error(ssl, icode);
				SSL_free(ssl);
				SSL_CTX_free(ctx);
				printf("SSL_accept error! code = %d, iret = %d\n", icode, iret);
				return NULL;
			}
		}
		else
			break;
	}

	return ssl;
}

void CMainEventBase::AcceptCallBack(evutil_socket_t fd, short events, void* arg)
{
	CMainEventBase* pThisObj = (CMainEventBase*)arg;
	if(pThisObj)
		pThisObj->OnAcceptCallback(fd, events);
}

void CMainEventBase::OnAcceptCallback(evutil_socket_t& fd, short events)
{
	evutil_socket_t sockfd = -1;
	struct sockaddr_in client;
	int len = sizeof(client);

	sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
	if(sockfd < 0)
	{
		static int iAcptCount;
		perror("CMainEventBase::accept error! \n");
		WLogError("CMainEventBase::OnAcceptCallback::accept error! iAcptCount = %d\n", iAcptCount++);
		return;
	}
	evutil_make_socket_nonblocking(sockfd);

	SSL* ssl = CreateSSL(sockfd);
	if(CConfiger::GetInstance()->GetEnableSSL() && !ssl)
	{
		WLogError("CMainEventBase::OnAcceptCallback::CreateSSL error! ssl NULL ");
		return ;
	}

	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);

	//将动态创建的结构体作为event的回调参数
	EventCtx* pCtx = new EventCtx();
	pCtx->pEvent = ev;
	pCtx->pSsl = ssl;
	pCtx->sock = sockfd;

	event_assign(ev, m_pMainBase, sockfd, EV_READ | EV_PERSIST,
		EventCallBack, (void*)pCtx);

	event_add(ev, NULL);

}

//事件回调处理
void CMainEventBase::EventCallBack(evutil_socket_t fd, short event, void* arg)
{
	int iret = -1;
	EventCtx* pCtx= (EventCtx*)arg;
	if(pCtx)
	{
		bool enablessl = CConfiger::GetInstance()->GetEnableSSL();
		CMgrRequest* pMgr = CMgrRequest::GetInstance();
		struct event* pEvent = pCtx->pEvent;
		SSL* pssl = pCtx->pSsl;

		char buffer[4096] = {0};
		int len = 0;
		if (enablessl)
		{
			len = SSL_read(pssl, buffer, sizeof(buffer));
		}
		else
		{
			len = recv(fd, buffer, sizeof(buffer), 0);
		}

		if (len <= 0)//客户端关闭连接，或者socket出现错误
		{
			delete pCtx;
			pCtx = NULL;
			return;
		}

		std::string reply = "ERROR";
		iret = pMgr->HandleRequest(pCtx, buffer, reply);
		if(iret != DATA_DEF)//若为数据不足，则暂时不回复，等待下次回调
		{
			if (enablessl)
			{
				len = SSL_write(pssl, reply.c_str(), reply.length());
			}
			else
			{
				len = send(fd, reply.c_str(), reply.length(), 0);
			}

			if (len < 0)
			{
				WLogError("CMainEventBase::EventCallBack::SSL_write error!\n");
			}
		}
	}
	else
	{
		WLogError("CMainEventBase::EventCallBack::EventCtx is NULL error!\n");
	}
}

void CMainEventBase::TimeoutCallBack(evutil_socket_t fd, short event, void* arg)
{
	CMainEventBase* pThisObj = (CMainEventBase*)arg;
	if (pThisObj)
	{
		pThisObj->OnTimeoutCallback();
	}
}

void CMainEventBase::OnTimeoutCallback()
{
	if (CMgrRequest::GetInstance())
	{
		CMgrRequest::GetInstance()->OnCheckActiveTime();
	}
}
#endif