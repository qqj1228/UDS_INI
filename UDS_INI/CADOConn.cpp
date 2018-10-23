#include "pch.h"
#include "CADOConn.h"
#include "util.h"

CADOConn::CADOConn(_bstr_t bstrIP, _bstr_t bstrPort, _bstr_t bstrUser, _bstr_t bstrPwd, _bstr_t bstrDBName, Logger *lpLog)
{
	m_bstrConnect = "Provider=SQLOLEDB.1;Password=" + bstrPwd + ";User ID=" + bstrUser;
	m_bstrConnect += ";Initial Catalog=" + bstrDBName + ";Data Source=";
	m_bstrConnect += bstrIP + "," + bstrPort;
	m_lpLog = lpLog;
}


CADOConn::~CADOConn()
{
}

bool CADOConn::OnInitADOConn()
{
	// ��ʼ��OLE/COM�⻷��
	::CoInitialize(NULL);
	try
	{
		// ����connection����
		m_pConnection.CreateInstance("ADODB.Connection");
		// ���������ַ���
		_bstr_t strConnect = m_bstrConnect;

		// SERVER��UID,PWD�����ø���ʵ�����������
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		return true;
	}
	// ��׽�쳣
	catch (_com_error e)
	{
		// ��ʾ������Ϣ
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
		return false;
	}
}

_RecordsetPtr& CADOConn::GetRecordSet(_bstr_t bstrSQL)
{
	try
	{
		// �������ݿ⣬���connection����Ϊ�գ��������������ݿ�
		if (m_pConnection == NULL)
			OnInitADOConn();
		// ������¼������
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		// ȡ�ñ��еļ�¼
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		m_lpLog->TRACE_INFO("SQL: %s", (char *)bstrSQL);
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
	}
	// ���ؼ�¼��
	return m_pRecordset;
}

bool CADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	_variant_t RecordsAffected;
	try
	{
		// �Ƿ����������ݿ�
		if (m_pConnection == NULL)
			OnInitADOConn();
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
		m_lpLog->TRACE_INFO("SQL: %s", (char *)bstrSQL);
		return true;
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
		return false;
	}
}

void CADOConn::ExitConnect()
{
	try
	{
		//�رռ�¼��������
		if (m_pRecordset != NULL)
			m_pRecordset->Close();
		m_pConnection->Close();
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
	}
	//�ͷŻ���
	::CoUninitialize();
}

string CADOConn::GetRecord(_bstr_t bstrField, size_t iIndex)
{
	_variant_t var = "";
	string str;
	try
	{
		for (size_t i = 0; i < iIndex && !m_pRecordset->adoEOF; i++)
		{
			m_pRecordset->MoveNext();
		}
		var = m_pRecordset->GetCollect(bstrField);
		str = VariantToString(var);
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
		return str;
	}
	return str;
}

void CADOConn::RecordSetClose()
{
	try
	{
		if (m_pRecordset != NULL)
		{
			m_pRecordset->Close();
			m_pRecordset = NULL;
		}
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
	}
}

_RecordsetPtr& CADOConn::GetSchemaTables()
{
	try
	{
		if (m_pConnection == NULL)
			OnInitADOConn();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset = m_pConnection->OpenSchema(adSchemaTables);
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
	}
	return m_pRecordset;
}

_RecordsetPtr& CADOConn::GetSchemaColumns(_bstr_t bstrSQL)
{
	try
	{
		if (m_pConnection == NULL)
			OnInitADOConn();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset = m_pConnection->OpenSchema(adSchemaColumns);
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			m_lpLog->TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
	}
	return m_pRecordset;
}

string CADOConn::VariantToString(const _variant_t &var)
{
	string strValue;
	stringstream ss;

	switch (var.vt)
	{
	case VT_BSTR: //�ַ���
	case VT_LPSTR:
	case VT_LPWSTR:
		strValue = string(_bstr_t(var));
		break;
	case VT_I1: //�޷����ַ�
	case VT_UI1:
		ss << var.bVal;
		ss >> strValue;
		break;
	case VT_I2: //������
		ss << var.iVal;
		ss >> strValue;
		break;
	case VT_UI2: //�޷��Ŷ�����
		ss << var.uiVal;
		ss >> strValue;
		break;
	case VT_INT: //����
		ss << var.intVal;
		ss >> strValue;
		break;
	case VT_I4: //����
	case VT_I8: //������
		ss << var.lVal;
		ss >> strValue;
		break;
	case VT_UINT: //�޷�������
		ss << var.uintVal;
		ss >> strValue;
		break;
	case VT_UI4: //�޷�������
	case VT_UI8: //�޷��ų�����
		ss << var.ulVal;
		ss >> strValue;
		break;
	case VT_VOID:
		ss << var.byref;
		ss >> strValue;
		break;
	case VT_R4: //������
		ss << var.fltVal;
		ss >> strValue;
		break;
	case VT_R8: //˫������
		ss << var.dblVal;
		ss >> strValue;
		break;
	case VT_DECIMAL: //С��
		ss << (double)var;
		ss >> strValue;
		break;
		//case VT_CY:
		//{
		//	COleCurrency cy = var.cyVal;
		//	strValue = cy.Format();
		//}
		break;
	case VT_BLOB:
	case VT_BLOB_OBJECT:
	case 0x2011:
		strValue = "[BLOB]";
		break;
	case VT_BOOL: //������
		strValue = var.boolVal ? "TRUE" : "FALSE";
		break;
		//case VT_DATE: //������
		//{
		//	DATE dt = var.date;
		//	COleDateTime da = COleDateTime(dt);
		//	strValue = da.Format("%Y-%m-%d %H:%M:%S");
		//}
		break;
	case VT_NULL: //NULLֵ
	case VT_EMPTY: //��
		strValue = "";
		break;
	case VT_UNKNOWN: //δ֪����
	default:
		strValue = "UN_KNOW";
		break;
	}
	return strValue;
}