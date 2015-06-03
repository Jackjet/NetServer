#ifndef CRECORD_H
#define CRECORD_H

#include "CField.h"


namespace DataBase
{


	/*
	* 1 ������¼
	* 2 [int ]���� [""]����
	*/
	class CRecord
	{
	public:
		/* ����� */
		vector<string> m_rs;
		/* �ֶ���Ϣ ռ��4�ֽڵ��ڴ� ����¼���ܴ��ǻز����������� */
		CField *m_field;
	public :
		CRecord(){};
		CRecord(CField* m_f);
		~CRecord();


		void SetData(string value);
		/* [""]���� */
		string operator[](string s);
		string operator[](int num);
		/* nullֵ�ж� */
		bool IsNull(int num);
		bool IsNull(string s);
		/* �� value tab value ����ʽ ���ؽ�� */
		string GetTabText();
	};


	/*
	* 1 ��¼����
	* 2 [int ]���� [""]����
	* 3 ��ṹ����
	* 4 ���ݵĲ����޸�
	*/
	class CRecordSet
	{
	private :
		/* ��¼�� */
		vector<CRecord> m_s;
		/* �α�λ��*/
		unsigned long pos;
		/* ��¼�� */
		int m_recordcount;
		/* �ֶ��� */
		int m_field_num;
		/* �ֶ���Ϣ */
		CField  m_field;

		MYSQL_RES * res ;
		MYSQL_FIELD * fd ;
		MYSQL_ROW row;
		MYSQL* m_Data ;
	public :
		CRecordSet();
		CRecordSet(MYSQL *hSQL);
		~CRecordSet();

		/* �����ض��еĲ�ѯ������Ӱ������� */
		int ExecuteSQL(const char *SQL);
		/* �õ���¼��Ŀ */
		int GetRecordCount();
		/* �õ��ֶ���Ŀ */
		int GetFieldNum();
		/* �����ƶ��α� */
		long MoveNext();
		/* �ƶ��α� */
		long Move(long length);
		/* �ƶ��α굽��ʼλ�� */
		bool MoveFirst();
		/* �ƶ��α굽����λ�� */
		bool MoveLast();
		/* ��ȡ��ǰ�α�λ�� */
		unsigned long GetCurrentPos()const;
		/* ��ȡ��ǰ�α�Ķ�Ӧ�ֶ����� */
		bool GetCurrentFieldValue(const char * sFieldName,char *sValue);
		bool GetCurrentFieldValue(const int iFieldNum,char *sValue);
		bool GetCurrentFieldValue(const char * sFieldName, std::string& sValue);
		/* ��ȡ�α�Ķ�Ӧ�ֶ����� */
		bool GetFieldValue(long index,const char * sFieldName,char *sValue);
		bool GetFieldValue(long index,int iFieldNum,char *sValue);
		/* �Ƿ񵽴��α�β�� */
		bool IsEof();



		/* �����ֶ� */
		CField* GetField();
		/* �����ֶ��� */
		const char * GetFieldName(int iNum);
		/* �����ֶ����� */
		const int GetFieldType(char * sName);
		const int GetFieldType(int iNum);
		/* ����ָ����ŵļ�¼ */
		CRecord operator[](int num);

	};

}
#endif