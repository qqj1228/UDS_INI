#pragma once
#include <Windows.h>
#include <comutil.h>
#include <comdef.h>
#include "Logger.h"

#import ".\\msado15.dll" no_namespace rename("EOF", "adoEOF") rename("BOF", "adoBOF")

class CADOConn
{
public:
	// 添加一个指向Connection对象的指针
	_ConnectionPtr m_pConnection;
	// 添加一个指向Recordset对象的指针
	_RecordsetPtr m_pRecordset;
	// 连接字符串
	_bstr_t m_bstrConnect;
	Logger *m_lpLog;

public:
	CADOConn(_bstr_t bstrIP, _bstr_t bstrPort, _bstr_t bstrUser, _bstr_t bstrPwd, _bstr_t bstrDBName, Logger *lpLog);
	virtual ~CADOConn();

	// 初始化--连接数据库
	bool OnInitADOConn();
	// 执行查询
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
	// 执行SQL语句
	bool ExecuteSQL(_bstr_t bstrSQL);
	// 断开数据库连接
	void ExitConnect();
	// 获取RecordSet中的某一条记录，需要在GetRecordSet之后执行
	// size_t iIndex为RecordSet中的索引号
	string GetRecord(_bstr_t bstrField, size_t iIndex);
	void RecordSetClose();
	_RecordsetPtr& GetSchemaTables();
	_RecordsetPtr& GetSchemaColumns(_bstr_t bstrSQL);
	string VariantToString(const _variant_t &var);
};