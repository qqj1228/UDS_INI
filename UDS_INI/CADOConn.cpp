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
	// 初始化OLE/COM库环境
	::CoInitialize(NULL);
	try
	{
		// 创建connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		// 设置连接字符串
		_bstr_t strConnect = m_bstrConnect;

		// SERVER和UID,PWD的设置根据实际情况来设置
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		return true;
	}
	// 捕捉异常
	catch (_com_error e)
	{
		// 显示错误信息
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
		// 连接数据库，如果connection对象为空，则重新连接数据库
		if (m_pConnection == NULL)
			OnInitADOConn();
		// 创建记录集对象
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		// 取得表中的记录
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
	// 返回记录集
	return m_pRecordset;
}

bool CADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	_variant_t RecordsAffected;
	try
	{
		// 是否已连接数据库
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
		//关闭记录集和连接
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
	//释放环境
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
	case VT_BSTR: //字符串
	case VT_LPSTR:
	case VT_LPWSTR:
		strValue = string(_bstr_t(var));
		break;
	case VT_I1: //无符号字符
	case VT_UI1:
		ss << var.bVal;
		ss >> strValue;
		break;
	case VT_I2: //短整型
		ss << var.iVal;
		ss >> strValue;
		break;
	case VT_UI2: //无符号短整型
		ss << var.uiVal;
		ss >> strValue;
		break;
	case VT_INT: //整型
		ss << var.intVal;
		ss >> strValue;
		break;
	case VT_I4: //整型
	case VT_I8: //长整型
		ss << var.lVal;
		ss >> strValue;
		break;
	case VT_UINT: //无符号整型
		ss << var.uintVal;
		ss >> strValue;
		break;
	case VT_UI4: //无符号整型
	case VT_UI8: //无符号长整型
		ss << var.ulVal;
		ss >> strValue;
		break;
	case VT_VOID:
		ss << var.byref;
		ss >> strValue;
		break;
	case VT_R4: //浮点型
		ss << var.fltVal;
		ss >> strValue;
		break;
	case VT_R8: //双精度型
		ss << var.dblVal;
		ss >> strValue;
		break;
	case VT_DECIMAL: //小数
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
	case VT_BOOL: //布尔型
		strValue = var.boolVal ? "TRUE" : "FALSE";
		break;
		//case VT_DATE: //日期型
		//{
		//	DATE dt = var.date;
		//	COleDateTime da = COleDateTime(dt);
		//	strValue = da.Format("%Y-%m-%d %H:%M:%S");
		//}
		break;
	case VT_NULL: //NULL值
	case VT_EMPTY: //空
		strValue = "";
		break;
	case VT_UNKNOWN: //未知类型
	default:
		strValue = "UN_KNOW";
		break;
	}
	return strValue;
}