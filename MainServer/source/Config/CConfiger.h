#ifndef CCONFIG_H
#define CCONFIG_H

#include "locker.h"
#include "CommStruct.h"
#include <map>
#include <vector>
#include <stdlib.h>

/*
* ���ö��󣬶�ȡ�����ļ���Ϊȫ���ṩ���û�ȡ����
* 
* �����ļ���ʽ��
* HuNan=mysql&431027&192.168.1.211&3306&admin&123456
* ������=����������&����&ip��ַ&�˿�&�û���&����
* GuangXi=redis&431028&192.168.1.210&3306&&admin&123456
* ������=����������&����&ip��ַ&�˿�&�û���&����
* threads=10	������ҵ���߳���
* insert_wait=100 ����ȴ�ʱ�� ����
* insert_max_count=100 ÿ�β����¼���������
* enable_record=true or false ��ǰ�ڵ��Ƿ�����ӡ�²�����¼ģ��
*/
class CConfiger
{
private:
	CConfiger();

public:
	~CConfiger();

	static CConfiger* GetInstance();
	static void DelInstance();

	bool OnInit(const std::string& filename);

	void GetConfigInfo(int iZoneCode, ConfigInfo& info, bool isRedis = false);
	void GetConfigInfos(std::map<int, ConfigInfo>& configs, bool isRedis = false);

	int GetCfgThreads()
	{
		return m_threads;
	}
	bool GetEnableRecord()
	{
		return m_isEnableRecord;
	}
	int GetInsertWait()
	{
		return m_insert_wait;
	}
	int GetInsertCount()
	{
		return m_insert_count;
	}

	bool GetEnableSSL()
	{
		return m_isEnablessl;
	}
protected:
	bool ReadLine(std::ifstream& ifs, std::string& outString);

	bool ParaseLine(const std::string& str);

	//����������������������߳�����ÿ�β����¼������ģ���Ƿ����õ�
	bool PerformanceItem(const std::string& str);

	//��������Դ�������mysql  redis���
	bool SourceDataItme(const std::string& str);

	void SplitString(const std::string& str, std::vector<std::string>& list);
	void SplitString2(std::string s, char splitchar, std::vector<std::string>& list);

private:
	static Locker m_locker;
	static CConfiger* m_pInstance;

	std::string m_fileName;

	Locker m_configMysqlLocker;
	std::map<int, ConfigInfo*> m_configMysqlMap;

	Locker m_configRedisLocker;
	std::map<int, ConfigInfo*> m_configRedisMap;

	int m_threads;
	bool m_isEnableRecord;
	bool m_isEnablessl;
	int m_insert_wait;
	int m_insert_count;
	int m_iLine;
};

#endif