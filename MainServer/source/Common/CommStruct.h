#ifndef COMM_STRUCT_H
#define COMM_STRUCT_H

#include <string>


#define REPLY_UNKNOW "unknown error!"
#define REPLY_FAIL "command fail!"
#define REPLY_DATA_DEF "data deficiencies!"
#define REPLY_ERROR "data error!"
#define REPLY_ZONECODE_ERR "areacode invalid!"
#define REPLY_CMD_INVALID "invalid command!"

#define _INSERT_LOG_ESIGN "_INSERT_LOG_ESIGN"
#define _INSERT_LOG_SIGNVERIFY "_INSERT_LOG_SIGNVERIFY"
#define _INSERT_LOG_SIGNREVOKE "_INSERT_LOG_SIGNREVOKE"
#define _INSERT_LOG_PRODUCEESEAL	"_INSERT_LOG_PRODUCEESEAL"
#define _INSERT_ESEAL "_INSERT_ESEAL"
#define _UPDATE_ESEAL "_UPDATE_ESEAL"

//���ݰ������������
typedef enum 
{
	UNKNOW = -1,	//δ֪����
	SUCCESS = 0,	//�ɹ�
	FAIL = 1,		//ʧ��
	DATA_OK = 2,	//��������
	DATA_DEF = 3,	//���ݲ��㣨���ְ�������ʾ��Ҫ��������
	DATA_ERROR = 4,	//���ݴ���
	ZONECODE_ERR = 5,	//���������Ч
	CMD_INVALID = 6,	//������Ч	

}HAND_REPLY;

//���ݰ������������
typedef enum 
{
	E_UNKNOW = -1,		//δ֪
	E_APPROVED = 1,		//������
	E_UNDERTAKED = 2,	//�ѳн�
	E_PRODUCED = 3,		//������
	E_PAID = 4,			//��֧��
	E_SCRAPPED = 5,		//�ѱ���
	E_CANCELED = 6,		//��ע��
	E_LOSS = 7,			//�ѹ�ʧ	
	E_LOCKED = 8,		//������
	E_EXPIRED = 9		//�ѹ���

}EN_ESTATUS;


//�����������
typedef enum
{
	REQ_UNKNOW = -1,
	REQ_SEAL_REQ = 0x05,	//ǩ������
	REQ_SEAL_LOG = 0x06,	//ǩ����־
	REQ_CANCEL_REQ = 0x07,	//��������
	REQ_CANCEL_LOG = 0x08,	//������־
	REQ_VERIFY_REQ = 0x09,	//��֤����
	REQ_CREATE_REQ = 0x10	//��������
}REQUEST_TYPE;

//������Ϣ
typedef struct
{
	std::string st_address;
	std::string st_port;
	std::string st_username;
	std::string st_password;
	std::string st_dbname;
}ConfigInfo;

//ͨ��Э��
typedef struct  
{
	int ilen;
	std::string strAreaCode;
	std::string strVersion;
	REQUEST_TYPE eType; //��������
	std::string strTime;//ʱ��
	std::string strLicence; //������ 
	std::string strData; //����������Ϣ
	std::string szDataParam[32];
	std::string strSealCode;
	int iCrc;	//У��
}ComProtocol;

#endif