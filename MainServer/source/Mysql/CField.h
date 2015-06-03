#ifndef CFIELD_H
#define CFIELD_H

#include <mysql.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace DataBase
{

	/*
	* �ֶβ��� 
	*/
	class CField
	{
	public :
		/* �ֶ����� */
		vector<string> m_name;
		/* �ֶ����� */
		vector<enum_field_types> m_type;
	public :
		CField();
		~CField();


		/* �Ƿ������� */
		bool IsNum(int num);
		/* �Ƿ������� */
		bool IsNum(string num);
		/* �Ƿ������� */
		bool IsDate(int num);
		/* �Ƿ������� */
		bool IsDate(string num);
		/* �Ƿ����ַ� */
		bool IsChar(int num);
		/* �Ƿ����ַ� */
		bool IsChar(string num);
		/* �Ƿ�Ϊ���������� */
		bool IsBlob(int num);
		/* �Ƿ�Ϊ���������� */
		bool IsBlob(string num);
		/* �õ�ָ���ֶε���� */
		int GetField_NO(string field_name);
	};

}
#endif//CFIELD_H