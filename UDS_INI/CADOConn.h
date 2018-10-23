#pragma once
#include <Windows.h>
#include <comutil.h>
#include <comdef.h>
#include "Logger.h"

#import ".\\msado15.dll" no_namespace rename("EOF", "adoEOF") rename("BOF", "adoBOF")

class CADOConn
{
public:
	// ���һ��ָ��Connection�����ָ��
	_ConnectionPtr m_pConnection;
	// ���һ��ָ��Recordset�����ָ��
	_RecordsetPtr m_pRecordset;
	// �����ַ���
	_bstr_t m_bstrConnect;
	Logger *m_lpLog;

public:
	CADOConn(_bstr_t bstrIP, _bstr_t bstrPort, _bstr_t bstrUser, _bstr_t bstrPwd, _bstr_t bstrDBName, Logger *lpLog);
	virtual ~CADOConn();

	// ��ʼ��--�������ݿ�
	bool OnInitADOConn();
	// ִ�в�ѯ
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
	// ִ��SQL���
	bool ExecuteSQL(_bstr_t bstrSQL);
	// �Ͽ����ݿ�����
	void ExitConnect();
	// ��ȡRecordSet�е�ĳһ����¼����Ҫ��GetRecordSet֮��ִ��
	// size_t iIndexΪRecordSet�е�������
	string GetRecord(_bstr_t bstrField, size_t iIndex);
	void RecordSetClose();
	_RecordsetPtr& GetSchemaTables();
	_RecordsetPtr& GetSchemaColumns(_bstr_t bstrSQL);
	string VariantToString(const _variant_t &var);
};