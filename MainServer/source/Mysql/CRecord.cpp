#include "CRecord.h"
#include <string.h>

namespace DataBase
{
	/************************************************************************/
	/* CRecord                                                              */
	/************************************************************************/
	/*
	* 1 ������¼
	* 2 [int ]���� [""]����
	*/

	CRecord::CRecord(CField * m_f)
	{
		m_field =m_f;
	}

	CRecord::~CRecord(){};

	void CRecord::SetData(string value)
	{
		m_rs.push_back (value);
	}
	/* [""]���� */
	string CRecord::operator[](string s)
	{
		if (m_field)
		{

			int iTmp = m_field->GetField_NO(s);
			if (iTmp < 0 ||
				iTmp >= m_rs.size())
			{
				return "";
			}
			return m_rs[iTmp];
		}
		return "";
	}
	string CRecord::operator[](int num)
	{
		if (num < 0 ||
			num >= m_rs.size())
		{
			return "";
		}
		return m_rs[num];
	}
	/* nullֵ�ж� */
	bool CRecord::IsNull(int num)
	{
		if (num < 0 ||
			num >= m_rs.size())
		{
			return false;
		}

		if("" == m_rs[num].c_str ())
			return true;
		else
			return false;
	}
	bool CRecord::IsNull(string s)
	{
		if(m_field)
		{
			int iTmp = m_field->GetField_NO(s);
			if (iTmp < 0 ||
				iTmp >= m_rs.size())
			{
				return false;
			}

			if("" == m_rs[iTmp].c_str())
				return true;
			else 
				return false;
		}
		return false;
	}
	/* ��Ҫ-����:�� value tab value ����ʽ ���ؽ�� */
	string CRecord::GetTabText()
	{
		string temp;
		for(unsigned int i=0 ;i<m_rs.size();i++)
		{
			temp+=m_rs[i];
			if(i<m_rs.size ()-1)
				temp+="\t";
		}
		return temp;
	}



	/************************************************************************/
	/* CRecordSet                                                           */
	/************************************************************************/
	/*
	* 1 ��¼����
	* 2 [int ]���� [""]����
	* 3 ��ṹ����
	* 4 ���ݵĲ����޸�
	*/
	CRecordSet::CRecordSet()
	{
		res = NULL;
		row = NULL;
		pos = 0;
	}
	CRecordSet::CRecordSet(MYSQL *hSQL)
	{
		res = NULL;
		row = NULL;
		m_Data = hSQL;
		pos = 0;
	}
	CRecordSet::~CRecordSet()
	{
	}
	/*
	* �����ض��еĲ�ѯ������Ӱ�������
	* �ɹ�����������ʧ�ܷ���-1
	*/
	int CRecordSet::ExecuteSQL(const char *SQL)
	{
		if ( !mysql_real_query(m_Data,SQL,strlen(SQL)))
		{
			//�����ѯ���
			res = mysql_store_result(m_Data );
			//�õ���¼����
			m_recordcount = (int)mysql_num_rows(res) ; 
			//�õ��ֶ�����
			m_field_num = mysql_num_fields(res) ;
			for (int x = 0 ; fd = mysql_fetch_field(res); x++)
			{
				m_field.m_name.push_back(fd->name);
				m_field.m_type.push_back(fd->type);
			}
			//������������
			while (row = mysql_fetch_row(res))
			{
				CRecord temp(&m_field);
				for (int k = 0 ; k < m_field_num ; k++ )
				{

					if(row[k]==NULL||(!strlen(row[k])))
					{
						temp.SetData ("");
					}
					else
					{

						temp.SetData(row[k]);
					}

				}
				//����¼�¼
				m_s.push_back (temp); 
			}
			mysql_free_result(res ) ;

			return m_s.size();
		}
		return -1;
	}
	/*
	* �����ƶ��α�
	* �����ƶ�����α�λ��
	*/
	long CRecordSet::MoveNext()
	{
		return (++pos);
	}
	/* �ƶ��α� */
	long  CRecordSet::Move(long length)
	{
		int l = pos + length;

		if(l<0)
		{
			pos = 0;
			return 0;
		}else 
		{ 
			if(l >= m_s.size())
			{
				pos = m_s.size()-1;
				return pos;
			}else
			{
				pos = l;
				return pos;
			}
		}

	}
	/* �ƶ��α굽��ʼλ�� */
	bool CRecordSet::MoveFirst()
	{
		pos = 0;
		return true;
	}
	/* �ƶ��α굽����λ�� */
	bool CRecordSet::MoveLast()
	{
		pos = m_s.size()-1;
		return true;
	}
	/* ��ȡ��ǰ�α�λ�� */
	unsigned long CRecordSet::GetCurrentPos()const
	{
		return pos;
	}
	/* ��ȡ��ǰ�α�Ķ�Ӧ�ֶ����� */
	bool CRecordSet::GetCurrentFieldValue(const char * sFieldName,
		char *sValue)
	{
		strcpy(sValue,m_s[pos][sFieldName].c_str());
		return true;
	}

	/* ��ȡ��ǰ�α�Ķ�Ӧ�ֶ����� */
	bool CRecordSet::GetCurrentFieldValue(const char * sFieldName,
		std::string& sValue)
	{
		sValue = m_s[pos][sFieldName];
		return true;
	}

	bool CRecordSet::GetCurrentFieldValue(const int iFieldNum,char *sValue)
	{
		strcpy(sValue,m_s[pos][iFieldNum].c_str());
		return true;
	}
	/* ��ȡ�α�Ķ�Ӧ�ֶ����� */
	bool CRecordSet::GetFieldValue(long index,const char * sFieldName,
		char *sValue)
	{
		strcpy(sValue,m_s[index][sFieldName].c_str());
		return true;
	}
	bool CRecordSet::GetFieldValue(long index,int iFieldNum,char *sValue)
	{
		strcpy(sValue,m_s[index][iFieldNum].c_str());
		return true;
	}
	/* �Ƿ񵽴��α�β�� */
	bool CRecordSet::IsEof()
	{ 
		return (pos == m_s.size())?true:false;
	}
	/* 
	* �õ���¼��Ŀ
	*/
	int CRecordSet::GetRecordCount()
	{
		return m_recordcount;
	}
	/* 
	* �õ��ֶ���Ŀ
	*/
	int CRecordSet::GetFieldNum()
	{
		return m_field_num;
	}
	/* 
	* �����ֶ�
	*/
	CField * CRecordSet::GetField()
	{
		return &m_field;
	}
	/* �����ֶ��� */
	const char * CRecordSet::GetFieldName(int iNum)
	{
		if ( !&m_field || iNum < 0 || iNum >= m_field.m_name.size())
		{
			return NULL;
		}
		return m_field.m_name.at(iNum).c_str();
	}
	/* �����ֶ����� */
	const int CRecordSet::GetFieldType(char * sName)
	{	
		if (&m_field)
		{

			int i = m_field.GetField_NO(sName);
			if (i < 0 ||
				i >= m_field.m_type.size())
			{
				return NULL;
			}
			return m_field.m_type.at(i);
		}
		return -1;
	}
	const int CRecordSet::GetFieldType(int iNum)
	{
		if (!&m_field || iNum < 0 ||
			iNum >= m_field.m_type.size())
		{
			return NULL;
		}
		return m_field.m_type.at(iNum);
	}
	/* 
	* ����ָ����ŵļ�¼
	*/
	CRecord CRecordSet::operator[](int num)
	{
		if (num < 0 ||
			num >= m_s.size())
		{
			return NULL;
		}
		return m_s[num];
	}

}
