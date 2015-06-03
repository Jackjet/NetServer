#include "CMgrRequest.h"
#include "CReqHandlerObj.h"
#include "MemoryPoolObj.h"
#include "locker.h"
#include "wLog.h"


//���ӱ���ʱ��
#ifndef ACTIVA_TIMEVAL
#define ACTIVA_TIMEVAL 15
#endif

Locker CMgrRequest::m_locker;
CMgrRequest* CMgrRequest::m_pInstance = NULL;

CMgrRequest::CMgrRequest()
{

}

CMgrRequest::~CMgrRequest()
{

}

CMgrRequest* CMgrRequest::GetInstance()
{
	if (NULL == m_pInstance)
	{
		LockerGuard guard(m_locker);
		if (NULL == m_pInstance)
		{
			m_pInstance = new CMgrRequest();
		}
	}

	return m_pInstance;
}

void CMgrRequest::DelInstance()
{
	if(m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
}

void CMgrRequest::ReleaseHandler(evutil_socket_t& sockfd)
{
	CReqHandlerObj* pObj = NULL;
	MemoryPool* pool = MemPoolObj::GetMemoryPool();
	LockerGuard guard(m_LockerReqMap);
	std::map<int, CReqHandlerObj*>::iterator it = m_ReqHandlerMap.find(sockfd);
	if(it!=m_ReqHandlerMap.end())
	{
		pObj = it->second;
		if (pObj)
		{
			bufferevent* pBevt = (bufferevent*)pObj->GetBufferEvent();
			if(pBevt)
				bufferevent_free(pBevt);
			m_ObjMgr.Delete(pool, pObj);
			m_ReqHandlerMap.erase(it);
		}
	}
}

void CMgrRequest::OnCheckActiveTime()
{
	time_t curTime;
	time(&curTime);
	CReqHandlerObj* pObj = NULL;
	MemoryPool* pool = MemPoolObj::GetMemoryPool();
	LockerGuard guard(m_LockerReqMap);
	std::map<int, CReqHandlerObj*>::iterator it = m_ReqHandlerMap.begin();
	for (it = m_ReqHandlerMap.begin(); it!=m_ReqHandlerMap.end(); )
	{
		pObj = it->second;
		if (pObj)
		{
			int iVal = curTime - pObj->GetActiveTime();
			if (iVal >= ACTIVA_TIMEVAL) //����ACTIVE_TIMEVAL�����ݽ���,�߳�����
			{
				bufferevent* pBevt = (bufferevent*)pObj->GetBufferEvent();
				if(pBevt)
					bufferevent_free(pBevt);
				WLogInfo("CMgrRequest::OnCheckActiveTime::BufferEvent Free!\n");
				m_ObjMgr.Delete(pool, pObj);
				m_ReqHandlerMap.erase(it++);
				continue;
			}

			it++;
		}
		else
		{
			it++;
		}
	}
}

int CMgrRequest::HandleRequest(bufferevent*& bev, const std::string& request, std::string& reply)
{
	reply.clear();
	int iRet = -1;
	bool isDone = false;

	CReqHandlerObj* pObj = NULL;
	evutil_socket_t sockfd = bufferevent_getfd(bev);
	m_LockerReqMap.lock();
	std::map<int, CReqHandlerObj*>::iterator it;
	it = m_ReqHandlerMap.find(sockfd);
	if (it != m_ReqHandlerMap.end())
	{
		pObj = it->second;
		if(pObj)
		{
			iRet = pObj->CheckData(request);
			if(iRet != DATA_DEF) //�����ѽ������
			{
				isDone = true;
				m_ReqHandlerMap.erase(it);
			}
		}
		m_LockerReqMap.unlock();
	}
	else
	{
		m_LockerReqMap.unlock(); //�Ƚ�������Ϊ�ڴ���ڲ���������,�����������
		pObj = m_ObjMgr.Create(MemPoolObj::GetMemoryPool(), bev);
		if(pObj)
		{
			iRet = pObj->CheckData(request);
			if (iRet != DATA_DEF) //�����ѽ������
			{
				isDone = true;	
			}
			else//���ݲ��㣬�������
			{
				m_LockerReqMap.lock();
				m_ReqHandlerMap.insert(std::make_pair(sockfd, pObj));
				m_LockerReqMap.unlock();
			}
		}
	}

	//������ȷ���ҽ�����ɣ���������
	if (pObj && 
		isDone && 
		iRet == DATA_OK)
	{
		iRet = pObj->OnHandle(reply);
	}

	if (pObj && iRet != DATA_DEF)
	{
		//�����ݲ����ΪDATA_ERROR �� UNKNOWN�����ϴ˴�����
		if(DATA_ERROR == iRet || UNKNOW == iRet)
		{
			bufferevent* pBevt = (bufferevent*)pObj->GetBufferEvent();
			if(pBevt)
				bufferevent_free(pBevt);
		}
		m_ObjMgr.Delete(MemPoolObj::GetMemoryPool(), pObj);
	}

	return iRet;
}