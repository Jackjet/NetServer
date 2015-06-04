#include "CReqHandlerObj.h"
#include "CRedisServiceMgr.h"
#include "CSqlServiceMgr.h"
#include "MemoryPoolObj.h"
#include "CHttpParse.h"

#include "wLog.h"
#include "Common.h"


CReqHandlerObj::CReqHandlerObj(void* arg)
{
	m_pArg = arg;
	m_requestStr.clear();
	m_pRedisMgr = CRedisServiceMgr::GetInstance();
	m_pSqlMgr = CSqlServiceMgr::GetInstance();
}

CReqHandlerObj::~CReqHandlerObj()
{
	m_pArg = NULL;
}

//�˺�����ִ�б�ʾ��ǰΪһ����Ч�����󣬲��ң����ݽ�����������
int CReqHandlerObj::OnHandle(std::string& reply)
{
	CHttpParse parse(m_requestStr);
	std::string content = parse.get_content();

	return SUCCESS;
}

int CReqHandlerObj::CheckData(const std::string& request)
{
	//ÿ��������ͨ�����¶���ʱ�䣬���ⱻ�������޳�
	UpdateActiveTime();
	m_requestStr += request;
	return DATA_OK;
	//����ͷ��β
	m_requestStr += request;
	const char* pHeader = strstr(m_requestStr.c_str(),"##");
	const char* pEnd = strstr(m_requestStr.c_str(),"\r\n");
	if (pHeader && pEnd)
	{
		return DATA_OK;
	}
	else if(!pHeader)
	{
		return DATA_ERROR;
	}
	else if (!pEnd)
	{
		return DATA_DEF;
	}
	return UNKNOW;
}
