#include "pch.h"
#include "CConfig.h"


CConfig::CConfig()
{
	m_strCfgFile = CONFIG_PATH;
	m_strCfgFile += "\\";
	m_strCfgFile += CONFIG_FILE;
	char buf[BUFSIZ];

	GetPrivateProfileString("DataBase", "IP", "127.0.0.1", buf, BUFSIZ, m_strCfgFile.c_str());
	m_strIP = buf;

	GetPrivateProfileString("DataBase", "Port", "1433", buf, BUFSIZ, m_strCfgFile.c_str());
	m_strPort = buf;

	GetPrivateProfileString("DataBase", "User", "sa", buf, BUFSIZ, m_strCfgFile.c_str());
	m_strUser = buf;

	GetPrivateProfileString("DataBase", "Pwd", "sh49", buf, BUFSIZ, m_strCfgFile.c_str());
	m_strPwd = buf;

	GetPrivateProfileString("DataBase", "DBName", "JAC_UDS", buf, BUFSIZ, m_strCfgFile.c_str());
	m_strDBName = buf;

}


CConfig::~CConfig()
{
}

string CConfig::getIP()
{
	return m_strIP;
}

string CConfig::getPort()
{
	return m_strPort;
}

string CConfig::getUser()
{
	return m_strUser;
}

string CConfig::getPwd()
{
	return m_strPwd;
}

string CConfig::getDBName()
{
	return m_strDBName;
}
