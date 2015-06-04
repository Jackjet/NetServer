#include "CHttpParse.h"
#include "wLog.h"
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

CHttpParse::CHttpParse(std::string& request)
	:m_requestData(request)
	,m_bHeaderDown(false)
{
	parse_request();
}

CHttpParse::~CHttpParse()
{

}

void CHttpParse::parse_request()
{
	int iret = parse_line_data_type();
	if(iret != 0)
	{
		WLogError("CHttpParse::parse_request::parse_line_data_type::error!\n");
	}
}

int CHttpParse::parse_line_data_type()
{
	std::string str = "";
	while(1)
	{
		if(m_bHeaderDown)
			return 0;
		str.clear();
		get_line(str);
		std::string keys;
		size_t startpos = str.find(": ");
		if (std::string::npos != startpos)
		{
			keys = str.substr(0, startpos);
			str = str.substr(startpos+2);
		}
		else
		{
			//get ����
			startpos = str.find("GET");
			size_t tmpos = str.find("POST");
			if (startpos != std::string::npos)
			{
				parse_header(str);
				m_httpMethod = METHOD_GET;
				continue;
			}	//post ����
			else if (tmpos != std::string::npos)
			{
				parse_header(str);
				m_httpMethod = METHOD_POST;
				continue;
			}
			else
			{	//����
				return -1;
			}
		}

		ToUpper(keys);
		if(parse_line(keys, str))
		{
			WLogError("CHttpParse::parse_line_data_type::parse_line::error, keys = %s, str = %s\n",
				keys.c_str(), str.c_str());
		}
	}
}

int CHttpParse::parse_line(std::string& keys, std::string& str)
{
	if (!keys.compare("HOST"))
	{
		m_host = str;
		return 0;
	}
	else if (!keys.compare("CONNECTION"))
	{
		if (!str.compare("keep-alive"))
		{
			m_connection = CON_KEEP_LIVE;
		}
		else
		{
			m_connection = CON_CLOSE;
		}
	}
	else if (!keys.compare("ACCEPT"))
	{
		m_accept = str;
	}
	else if (!keys.compare("USER-AGENT"))
	{
		m_user_agent = str;
	}
	else if (!keys.compare("ACCEPT-ENCODING"))
	{
		m_accept_encoding = str;
	}
	else if (!keys.compare("ACCEPT-LANGUAGE"))
	{
		m_language = str;
	}
	else if (!keys.compare("REFERER"))
	{
		m_referer = str;
	}
	else if (!keys.compare("CONTENT_LENGHT"))
	{
		m_referer = str;
	}
	else
	{
		return -1;
	}
}

//ȡ��һ�У���ȥ����β�Ļس���
void CHttpParse::get_line(std::string& retstr)
{
	size_t pos = m_requestData.find("\r\n");
	if (std::string::npos != pos)
	{
		retstr = m_requestData.substr(0, pos);
	}
	m_requestData = m_requestData.substr(pos+2);

	if (retstr.empty()) //�������У�����ͷ�Ѿ�������ɣ�����Ϊ����
	{
		m_content = m_requestData;
		m_requestData = "";
		m_bHeaderDown = true;
	}
}

void CHttpParse::ToUpper(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}

void CHttpParse::parse_header(std::string& retstr)
{
	std::vector<std::string> list;
	SplitString(retstr, ' ', list);
	if (list.size() > 2)
	{
		m_version = list.at(2);
	}
}

void CHttpParse::SplitString(std::string s, char splitchar, std::vector<std::string>& vec)
{
	if(vec.size()>0)//��֤vec�ǿյ�
		vec.clear();
	int length = s.length();
	int start=0;
	for(int i=0;i<length;i++)
	{
		if(s[i] == splitchar && i == 0)//��һ���������ָ��
		{
			start += 1;
		}
		else if(s[i] == splitchar)
		{
			vec.push_back(s.substr(start,i - start));
			start = i+1;
		}
		else if(i == length-1)//����β��
		{
			vec.push_back(s.substr(start,i+1 - start));
		}
	}
}