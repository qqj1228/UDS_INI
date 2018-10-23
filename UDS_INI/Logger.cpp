#include "pch.h"
#include "Logger.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"

//Ĭ�Ϲ��캯��
Logger::Logger()
{
	//��ʼ��
	memset(m_strLogPath, 0, MAX_STR_LEN);
	memset(m_strCurLogName, 0, MAX_STR_LEN);
	m_pFileStream = NULL;
	//����Ĭ�ϵ�д��־����
	m_nLogLevel = EnumLogLevel::LogLevelNormal;
	//��ʼ���ٽ�������
	InitializeCriticalSection(&m_cs);
	//������־�ļ���
	GenerateLogName();
}

//���캯��
Logger::Logger(const char * strLogPath, EnumLogLevel nLogLevel) :m_nLogLevel(nLogLevel)
{
	//��ʼ��
	m_pFileStream = NULL;
	strcpy_s(m_strLogPath, strLogPath);
	InitializeCriticalSection(&m_cs);
	CreateLogPath();
	GenerateLogName();
}


//��������
Logger::~Logger()
{
	//�ͷ��ٽ���
	DeleteCriticalSection(&m_cs);
	//�ر��ļ���
	if (m_pFileStream)
		fclose(m_pFileStream);
}

// �ر��ļ���
void Logger::CloseFile()
{
	if (m_pFileStream)
	{
		fclose(m_pFileStream);
		m_pFileStream = NULL;
	}
}

//д�ؼ���Ϣ�ӿ�
void Logger::TraceFatal(const char * strInfo, ...)
{
	if (!strInfo)
		return;
	char pTemp[MAX_STR_LEN] = { 0 };
	GetDateTime(pTemp, MAX_STR_LEN);
	strcat_s(pTemp, FATALPREFIX);
	//��ȡ�ɱ��β�
	va_list arg_ptr = NULL;
	va_start(arg_ptr, strInfo);
	vsprintf_s(pTemp + strlen(pTemp), MAX_STR_LEN - strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	//д��־�ļ�
	Trace(pTemp);
	arg_ptr = NULL;
	CloseFile();

}

//д������Ϣ
void Logger::TraceError(const char* strInfo, ...)
{
	//�жϵ�ǰ��д��־����������Ϊ��д��־��������
	if (m_nLogLevel >= EnumLogLevel::LogLevelStop)
		return;
	if (!strInfo)
		return;
	char pTemp[MAX_STR_LEN] = { 0 };
	GetDateTime(pTemp, MAX_STR_LEN);
	strcat_s(pTemp, ERRORPREFIX);
	va_list arg_ptr = NULL;
	va_start(arg_ptr, strInfo);
	vsprintf_s(pTemp + strlen(pTemp), MAX_STR_LEN - strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	Trace(pTemp);
	arg_ptr = NULL;
	CloseFile();
}

//д������Ϣ
void Logger::TraceWarning(const char * strInfo, ...)
{
	//�жϵ�ǰ��д��־����������Ϊֻд������Ϣ��������
	if (m_nLogLevel >= EnumLogLevel::LogLevelNormal)
		return;
	if (!strInfo)
		return;
	char pTemp[MAX_STR_LEN] = { 0 };
	GetDateTime(pTemp, MAX_STR_LEN);
	strcat_s(pTemp, WARNINGPREFIX);
	va_list arg_ptr = NULL;
	va_start(arg_ptr, strInfo);
	vsprintf_s(pTemp + strlen(pTemp), MAX_STR_LEN - strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	Trace(pTemp);
	arg_ptr = NULL;
	CloseFile();
}


//дһ����Ϣ
void Logger::TraceInfo(const char * strInfo, ...)
{
	//�жϵ�ǰ��д��־����������ֻд����;�����Ϣ��������
	if (m_nLogLevel >= EnumLogLevel::LogLevelMid)
		return;
	if (!strInfo)
		return;
	char pTemp[MAX_STR_LEN] = { 0 };
	GetDateTime(pTemp, MAX_STR_LEN);
	strcat_s(pTemp, INFOPREFIX);
	va_list arg_ptr = NULL;
	va_start(arg_ptr, strInfo);
	vsprintf_s(pTemp + strlen(pTemp), MAX_STR_LEN - strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	Trace(pTemp);
	arg_ptr = NULL;
	CloseFile();
}

//��ȡϵͳ��ǰʱ��
void Logger::GetDateTime(char *pTemp, int length)
{
	time_t curTime;
	tm timeInfo;
	time(&curTime);
	localtime_s(&timeInfo, &curTime);
	sprintf_s(pTemp, length, "%04d-%02d-%02d %02d:%02d:%02d", timeInfo.tm_year + 1990, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

//����д��־����
void Logger::SetLogLevel(EnumLogLevel nLevel)
{
	m_nLogLevel = nLevel;
}

//д�ļ�����
void Logger::Trace(const char * strInfo)
{
	if (!strInfo)
		return;
	try
	{
		//�����ٽ���
		EnterCriticalSection(&m_cs);
		//���ļ���û�д򿪣������´�
		if (!m_pFileStream)
		{
			char temp[1024] = { 0 };
			strcat_s(temp, m_strLogPath);
			strcat_s(temp, m_strCurLogName);
			fopen_s(&m_pFileStream, temp, "a+");
			if (!m_pFileStream)
			{
				LeaveCriticalSection(&m_cs);
				cout << "Can not open log file: " << string(temp) << endl;
				return;
			}
		}
		//д��־��Ϣ���ļ���
		fprintf(m_pFileStream, "%s\n", strInfo);
		fflush(m_pFileStream);
		//�뿪�ٽ���
		LeaveCriticalSection(&m_cs);
	}
	//�������쳣�������뿪�ٽ�������ֹ����
	catch (...)
	{
		LeaveCriticalSection(&m_cs);
	}
}

//������־�ļ�������
void Logger::GenerateLogName()
{
	time_t curTime;
	tm timeInfo;
	time(&curTime);
	localtime_s(&timeInfo, &curTime);
	char temp[1024] = { 0 };
	//��־�������磺2013-01-01.log
	sprintf_s(temp, "%04d-%02d-%02d.log", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);
	if (0 != strcmp(m_strCurLogName, temp))
	{
		strcpy_s(m_strCurLogName, temp);
		if (m_pFileStream)
			fclose(m_pFileStream);
		char temp[1024] = { 0 };
		strcat_s(temp, m_strLogPath);
		strcat_s(temp, m_strCurLogName);
		//��׷�ӵķ�ʽ���ļ���
		fopen_s(&m_pFileStream, temp, "a+");
	}

}

//������־�ļ���·��
void Logger::CreateLogPath()
{
	if (0 != strlen(m_strLogPath))
	{
		strcat_s(m_strLogPath, "\\");
	}
	mkMultiDir(m_strLogPath);
}