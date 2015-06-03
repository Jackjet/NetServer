#include "CUtilSql.h"
#include "Common.h"

CUtilSql::CUtilSql()
{

}

CUtilSql::~CUtilSql()
{

}

//�������eseal���sql���
std::string CUtilSql::MakeInsertESealSql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, INSERT_ESEAL_FORMAT,
		objRequest.szDataParam[0].c_str(), objRequest.szDataParam[1].c_str(), objRequest.szDataParam[2].c_str(),
		objRequest.szDataParam[3].c_str(), objRequest.szDataParam[4].c_str(), objRequest.szDataParam[5].c_str(), 
		objRequest.strAreaCode.c_str(), objRequest.szDataParam[6].c_str(),
		objRequest.szDataParam[7].c_str(), objRequest.szDataParam[8].c_str(), objRequest.szDataParam[9].c_str(),
		objRequest.szDataParam[10].c_str(), objRequest.szDataParam[11].c_str(), objRequest.szDataParam[12].c_str(),
		objRequest.szDataParam[15].c_str(), objRequest.szDataParam[16].c_str(), objRequest.szDataParam[17].c_str(), 
		objRequest.szDataParam[18].c_str(), objRequest.szDataParam[19].c_str());

	sqlStr = std::string() + INSERT_ESEAL + sqlBuf;

	return sqlStr;
}

//�������log_produceeseal���sql���
std::string CUtilSql::MakeInsertLogProduceEsealSql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, LOG_PRODUCEESEAL_ESEAL_FORMAT,
		uuid, objRequest.szDataParam[0].c_str(), objRequest.szDataParam[1].c_str(), objRequest.szDataParam[2].c_str(),
		objRequest.szDataParam[3].c_str(), objRequest.szDataParam[4].c_str(), objRequest.szDataParam[5].c_str(),
		objRequest.szDataParam[11].c_str(), objRequest.szDataParam[12].c_str(), objRequest.szDataParam[13].c_str(),
		objRequest.szDataParam[14].c_str(), objRequest.szDataParam[15].c_str(), objRequest.szDataParam[16].c_str(),
		objRequest.szDataParam[18].c_str(), objRequest.szDataParam[19].c_str(), objRequest.szDataParam[20].c_str(),
		objRequest.szDataParam[21].c_str(), objRequest.szDataParam[22].c_str(), objRequest.szDataParam[23].c_str(), objRequest.szDataParam[24].c_str(),
		objRequest.strAreaCode.c_str(), SRC_ESTATUS_OK);

	sqlStr = std::string() + INSERT_LOG_PRODUCEESEAL + sqlBuf;

	return sqlStr;
}

//�������log_esign���sql���
std::string CUtilSql::MakeInsertLogESignSql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, LOG_ESIGN_FORMAT, uuid,
		objRequest.szDataParam[0].c_str(), objRequest.szDataParam[1].c_str(), objRequest.szDataParam[2].c_str(),
		objRequest.szDataParam[3].c_str(), objRequest.szDataParam[4].c_str(), objRequest.szDataParam[5].c_str(),
		objRequest.szDataParam[6].c_str(), objRequest.szDataParam[7].c_str(), objRequest.szDataParam[8].c_str(),
		objRequest.szDataParam[9].c_str(), objRequest.szDataParam[10].c_str(), objRequest.szDataParam[11].c_str(),
		objRequest.szDataParam[12].c_str(), objRequest.szDataParam[13].c_str(), objRequest.szDataParam[14].c_str(),
		objRequest.szDataParam[15].c_str(), objRequest.szDataParam[16].c_str(), objRequest.szDataParam[17].c_str(),
		objRequest.szDataParam[18].c_str(), objRequest.szDataParam[19].c_str(), objRequest.szDataParam[20].c_str(),
		objRequest.szDataParam[21].c_str(), objRequest.szDataParam[22].c_str(), objRequest.szDataParam[23].c_str(),
		objRequest.szDataParam[24].c_str(), objRequest.szDataParam[25].c_str(), objRequest.strAreaCode.c_str());

	sqlStr = /*std::string() + INSERT_LOG_ESIGN + */sqlBuf;

	return sqlStr;
}

//�������log_signrevoke���sql���
std::string CUtilSql::MakeInsertLogSignRevokeSql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, LOG_SIGNREVOKE_FORMAT, uuid,
		objRequest.szDataParam[0].c_str(), objRequest.szDataParam[1].c_str(), objRequest.szDataParam[2].c_str(),
		objRequest.szDataParam[3].c_str(), objRequest.szDataParam[4].c_str(), objRequest.szDataParam[5].c_str(),
		objRequest.szDataParam[6].c_str(), objRequest.szDataParam[7].c_str(), objRequest.szDataParam[8].c_str(),
		objRequest.strAreaCode.c_str(), objRequest.szDataParam[11].c_str(), objRequest.szDataParam[12].c_str(),
		objRequest.szDataParam[15].c_str(), objRequest.szDataParam[16].c_str(), objRequest.szDataParam[19].c_str(),
		objRequest.szDataParam[20].c_str(), objRequest.szDataParam[21].c_str(), objRequest.szDataParam[22].c_str(), 
		objRequest.szDataParam[23].c_str());

	sqlStr = sqlBuf;

	return sqlStr;
}

//�������log_signverify���sql���
std::string CUtilSql::MakeInsertLogSignVerifySql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, LOG_SIGNVERIFY_FORMAT, uuid,
		objRequest.szDataParam[0].c_str(), objRequest.szDataParam[1].c_str(), objRequest.szDataParam[2].c_str(),
		objRequest.szDataParam[3].c_str(), objRequest.szDataParam[4].c_str(), objRequest.szDataParam[5].c_str(),
		objRequest.szDataParam[6].c_str(), objRequest.szDataParam[7].c_str(), objRequest.szDataParam[8].c_str(),
		objRequest.szDataParam[9].c_str(), objRequest.szDataParam[10].c_str(), objRequest.szDataParam[11].c_str(),
		objRequest.szDataParam[12].c_str(), objRequest.szDataParam[13].c_str(), objRequest.szDataParam[14].c_str(),
		objRequest.szDataParam[15].c_str(), objRequest.strAreaCode.c_str());

	sqlStr = sqlBuf;
	return sqlStr;
}

//�������eseal���sql���
std::string CUtilSql::MakeUpdateESealSql(ComProtocol& objRequest)
{
	std::string sqlStr;
	char sqlBuf[1024] = { 0x00 };
	char uuid[64] = { 0x00 };
	Common::GetUid(uuid);

	sprintf(sqlBuf, UPDATE_ESEAL_FORMAT, objRequest.szDataParam[2].c_str(), objRequest.szDataParam[11].c_str(), objRequest.szDataParam[12].c_str(),
		objRequest.szDataParam[15].c_str(), objRequest.szDataParam[16].c_str(), objRequest.szDataParam[0].c_str());
	sqlStr = sqlBuf;

	return sqlStr;
}

//�����ѯeseal���sql���
std::string CUtilSql::MakeSelectESealSql(std::string& key)
{
	std::string sqlStr;
	//״̬ ����ʱ�� ��ǰʱ��
	char szBufTemp[1024] = { 0x00 };
	sprintf(szBufTemp, SELECT_ESEAL_FORMAT, key.c_str());

	sqlStr = szBufTemp;
	return sqlStr; 
}