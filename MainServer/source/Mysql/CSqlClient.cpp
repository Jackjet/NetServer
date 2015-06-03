#include "CSqlClient.h"
#include "CRecord.h"
#include "wLog.h"
#include <string.h>

CSqlClient::CSqlClient()
{
	m_SqlHandle = NULL;
	m_iReconnectCount = 0;
}

CSqlClient::~CSqlClient()
{
	OnDisConnect();
}

bool CSqlClient::OnInit(std::string& address, unsigned int& port, 
	std::string& user, std::string& password,
	std::string& dbName, unsigned long& flag)
{
	m_port = port;
	m_address = address;
	m_dbName = dbName;
	m_flag = flag;
	m_user = user;
	m_password = password;

	return OnConnect();
}

int CSqlClient::ExecSqlCmd(std::string& sql, std::string& result)
{
	MYSQL_RES  *res_ptr;
	MYSQL_ROW  sqlrow;
	int iTableRow,iTableCol,i,j;

	LockerGuard guard(m_locker);
	char szTemp[1024]= {0x00};
	if (!m_SqlHandle)
	{
		return -1;
	}
	int iRet = mysql_query(m_SqlHandle, sql.c_str());//ִ��SQL���
	if (iRet)
	{
		WLogError("CSqlClient::ExecSqlCmd::mysql_query::%s==>%d: %s !\n",
			sql.c_str(),
			mysql_errno(m_SqlHandle),
			mysql_error(m_SqlHandle));//>��ӡ�����������Ϣ
		return iRet;
	}

	do
	{
		res_ptr = mysql_store_result(m_SqlHandle);//����
		if( res_ptr )
		{
			iTableRow = mysql_num_rows(res_ptr);//��
			iTableCol = mysql_num_fields(res_ptr);//��
			for(i=0; i<iTableRow; i++)
			{
				sqlrow = mysql_fetch_row(res_ptr);
				for(j=0; j<iTableCol; j++)
				{
					if(j>0)
						result+=";";
					result+=sqlrow[j];
				}
			}
			mysql_free_result(res_ptr);//��ɶ����ݵ����в�����,���ô˺�������MySQL������������Ķ���
		}
	}
	while( !mysql_next_result(m_SqlHandle ) );

	return iRet;
}

int CSqlClient::ExecSqlCmd(const char* sql)
{
	LockerGuard guard(m_locker);
	if (!m_SqlHandle)
	{
		return -1;
	}
	if ( !mysql_real_query(m_SqlHandle, sql, strlen(sql)) )
	{
		return 0;
	}

	return -1;
}

bool CSqlClient::OnConnect()
{
	LockerGuard guard(m_locker);
	int ml_outtime = 500;
	m_SqlHandle = mysql_init(NULL);
	if(!m_SqlHandle) 
	{
		WLogError("CSqlClient::OnConnect::mysql_init error!\n");
		return false;
	}
	mysql_options(m_SqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, &ml_outtime);
	
	if(mysql_real_connect( m_SqlHandle, m_address.c_str(), m_user.c_str(), m_password.c_str(), m_dbName.c_str(), m_port, NULL, m_flag))
	{
		std::string charter = "gb2312";

		if (!mysql_set_character_set(m_SqlHandle, charter.c_str()))
		{
			WLogInfo("New client character set: %s\n", mysql_character_set_name(m_SqlHandle));
		}
		//ѡ�����ݿ�ʧ��
		if ( mysql_select_db( m_SqlHandle, m_dbName.c_str()) < 0 ) 
		{
			WLogError("CSqlClient::OnConnect::mysql_select_db error!\n");
			mysql_close( m_SqlHandle);
			m_SqlHandle = NULL;
			return false ;
		}
	}
	else 
	{
		WLogError("CSqlClient::OnConnect::mysql_real_connect error! address=%s, user=%s, passwd = %s, dbName = %s, port = %d\n"
			,m_address.c_str(), m_user.c_str(), m_password.c_str(), m_dbName.c_str(), m_port);
		mysql_close( m_SqlHandle );
		m_SqlHandle = NULL;
		return false;
	}
	WLogInfo("CSqlClient::OnConnected success!\n");
	//�ɹ�
	return true;
}

void CSqlClient::OnDisConnect()
{
	LockerGuard guard(m_locker);
	
	if(m_SqlHandle)
	{
		mysql_close( m_SqlHandle);
	}
	m_SqlHandle = NULL;
}

bool CSqlClient::ReConnect()
{
	OnDisConnect();
	return OnConnect();
}

bool CSqlClient::BeginTrans()
{
	return mysql_autocommit(m_SqlHandle, 0);//�ر��Զ��ύ
}

bool CSqlClient::TransRollback()
{
	return mysql_rollback(m_SqlHandle);
}

bool CSqlClient::TransCommit()
{
	return mysql_commit(m_SqlHandle);
	mysql_autocommit(m_SqlHandle, 1);//�ָ��Զ��ύ
}

bool CSqlClient::TestOnline()
{
	if(!m_SqlHandle)
		return false;
	int isline = -1;
	{
		LockerGuard guard(m_locker);
		isline = mysql_ping(m_SqlHandle);
	} 
	if(isline)
	{
		if(ReConnect())
		{
			WLogInfo("CSqlClient::TestOnline::SqlClient Re Online! ip=%s, port=%d\n", m_address.c_str(), m_port);
			m_iReconnectCount = 0;
			return true;
		}
		else //�����Ѿ��Ͽ�����ʼ�������������������Ժ����Ӳ�������Ϊ�����ݿ������ߣ�����client�����н����޳�
		{
			m_iReconnectCount++;
			if(m_iReconnectCount > 3)
			{
				WLogError("CSqlClient::TestOnline::SqlClient Off! ip=%s, port=%d\n", m_address.c_str(), m_port);
				return false;
			}
			else
			{
				return true;
			}
		}
	} 
	else
	{
		m_iReconnectCount = 0;
		return true;
	}
}