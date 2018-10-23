#include "pch.h"
#include "Logger.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"

//默认构造函数
Logger::Logger()
{
	//初始化
	memset(m_strLogPath, 0, MAX_STR_LEN);
	memset(m_strCurLogName, 0, MAX_STR_LEN);
	m_pFileStream = NULL;
	//设置默认的写日志级别
	m_nLogLevel = EnumLogLevel::LogLevelNormal;
	//初始化临界区变量
	InitializeCriticalSection(&m_cs);
	//创建日志文件名
	GenerateLogName();
}

//构造函数
Logger::Logger(const char * strLogPath, EnumLogLevel nLogLevel) :m_nLogLevel(nLogLevel)
{
	//初始化
	m_pFileStream = NULL;
	strcpy_s(m_strLogPath, strLogPath);
	InitializeCriticalSection(&m_cs);
	CreateLogPath();
	GenerateLogName();
}


//析构函数
Logger::~Logger()
{
	//释放临界区
	DeleteCriticalSection(&m_cs);
	//关闭文件流
	if (m_pFileStream)
		fclose(m_pFileStream);
}

// 关闭文件流
void Logger::CloseFile()
{
	if (m_pFileStream)
	{
		fclose(m_pFileStream);
		m_pFileStream = NULL;
	}
}

//写关键信息接口
void Logger::TraceFatal(const char * strInfo, ...)
{
	if (!strInfo)
		return;
	char pTemp[MAX_STR_LEN] = { 0 };
	GetDateTime(pTemp, MAX_STR_LEN);
	strcat_s(pTemp, FATALPREFIX);
	//获取可变形参
	va_list arg_ptr = NULL;
	va_start(arg_ptr, strInfo);
	vsprintf_s(pTemp + strlen(pTemp), MAX_STR_LEN - strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	//写日志文件
	Trace(pTemp);
	arg_ptr = NULL;
	CloseFile();

}

//写错误信息
void Logger::TraceError(const char* strInfo, ...)
{
	//判断当前的写日志级别，若设置为不写日志则函数返回
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

//写警告信息
void Logger::TraceWarning(const char * strInfo, ...)
{
	//判断当前的写日志级别，若设置为只写错误信息则函数返回
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


//写一般信息
void Logger::TraceInfo(const char * strInfo, ...)
{
	//判断当前的写日志级别，若设置只写错误和警告信息则函数返回
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

//获取系统当前时间
void Logger::GetDateTime(char *pTemp, int length)
{
	time_t curTime;
	tm timeInfo;
	time(&curTime);
	localtime_s(&timeInfo, &curTime);
	sprintf_s(pTemp, length, "%04d-%02d-%02d %02d:%02d:%02d", timeInfo.tm_year + 1990, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

//设置写日志级别
void Logger::SetLogLevel(EnumLogLevel nLevel)
{
	m_nLogLevel = nLevel;
}

//写文件操作
void Logger::Trace(const char * strInfo)
{
	if (!strInfo)
		return;
	try
	{
		//进入临界区
		EnterCriticalSection(&m_cs);
		//若文件流没有打开，则重新打开
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
		//写日志信息到文件流
		fprintf(m_pFileStream, "%s\n", strInfo);
		fflush(m_pFileStream);
		//离开临界区
		LeaveCriticalSection(&m_cs);
	}
	//若发生异常，则先离开临界区，防止死锁
	catch (...)
	{
		LeaveCriticalSection(&m_cs);
	}
}

//创建日志文件的名称
void Logger::GenerateLogName()
{
	time_t curTime;
	tm timeInfo;
	time(&curTime);
	localtime_s(&timeInfo, &curTime);
	char temp[1024] = { 0 };
	//日志的名称如：2013-01-01.log
	sprintf_s(temp, "%04d-%02d-%02d.log", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);
	if (0 != strcmp(m_strCurLogName, temp))
	{
		strcpy_s(m_strCurLogName, temp);
		if (m_pFileStream)
			fclose(m_pFileStream);
		char temp[1024] = { 0 };
		strcat_s(temp, m_strLogPath);
		strcat_s(temp, m_strCurLogName);
		//以追加的方式打开文件流
		fopen_s(&m_pFileStream, temp, "a+");
	}

}

//创建日志文件的路径
void Logger::CreateLogPath()
{
	if (0 != strlen(m_strLogPath))
	{
		strcat_s(m_strLogPath, "\\");
	}
	mkMultiDir(m_strLogPath);
}