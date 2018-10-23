#pragma once


class CConfig
{
private:
	string m_strCfgFile;
	string m_strIP;
	string m_strPort;
	string m_strUser;
	string m_strPwd;
	string m_strDBName;

public:
	CConfig();
	~CConfig();
	string getIP();
	string getPort();
	string getUser();
	string getPwd();
	string getDBName();
};

