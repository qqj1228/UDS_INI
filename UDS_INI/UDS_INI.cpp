// UDS_INI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "CADOConn.h"
#include "Logger.h"
#include "CConfig.h"
#include "util.h"

void writeINI(const string strSection, const string strKey, const string strValue, const string strFilePath) {
	DWORD dwResult = WritePrivateProfileString(strSection.c_str(), strKey.c_str(), strValue.c_str(), strFilePath.c_str());
	if (!dwResult) {
		throw dwResult;
	}

}

int main()
{
	const string strLogPath = LOG_DIR;
	const int iMaxFileNum = LOG_FILE_QTY;

	Logger log(strLogPath.c_str(), LogLevelAll);
	updateFileNum(strLogPath, iMaxFileNum);
	CConfig cfg;

	CADOConn ado(
		(cfg.getIP()).c_str(),
		(cfg.getPort()).c_str(),
		(cfg.getUser()).c_str(),
		(cfg.getPwd()).c_str(),
		(cfg.getDBName()).c_str(),
		&log
	);

	if (!ado.OnInitADOConn()) {
		WORD wOrigin = setConsoleColor(12, 14);
		cout << "Open DataBase failed!" << endl;
		setConsoleColor(wOrigin);
		log.TRACE_FATAL("Open DataBase failed!");
		return -1;
	}

	// 填充mapVehicleType，<VehicleType, ECUConfig>
	map<string, vector<int>> mapVehicleType;
	string sql = "select * from VehicleType";
	_RecordsetPtr pRecordset = ado.GetRecordSet(sql.c_str());

	vector<string> vct;
	vector<int> vctECUConfig;
	string strVehicleType;
	string strECUConfig;
	try
	{
		while (!pRecordset->adoEOF) {
			strVehicleType = ado.VariantToString(pRecordset->GetCollect("VehicleType"));
			strECUConfig = ado.VariantToString(pRecordset->GetCollect("ECUConfig"));
			vct.clear();
			splitString(strECUConfig, vct, ",");
			vctECUConfig.clear();
			for (auto item : vct) {
				vctECUConfig.push_back(stoi(item));
			}
			mapVehicleType.insert(pair<string, vector<int>>(strVehicleType, vctECUConfig));
			pRecordset->MoveNext();
		}
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			log.TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			log.TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
		return -1;
	}
	ado.RecordSetClose();

	// 填充mapECU，<ID, <ECUName, TestConfig>>
	map<int, pair<string, string>> mapECU;
	sql = "select * from ECU";
	pRecordset = ado.GetRecordSet(sql.c_str());

	int ID = 0;
	string strECUName;
	string strTestConfig;
	try
	{
		while (!pRecordset->adoEOF) {
			ID = (pRecordset->GetCollect("ID")).intVal;
			strECUName = ado.VariantToString(pRecordset->GetCollect("ECUName"));
			strTestConfig = ado.VariantToString(pRecordset->GetCollect("TestConfig"));
			mapECU.insert(pair<int, pair<string, string>>(ID, pair<string, string>(strECUName, strTestConfig)));
			pRecordset->MoveNext();
		}
	}
	catch (_com_error e)
	{
		WORD wOrigin = setConsoleColor(12, 14);
		if (e.Description() == _bstr_t("")) {
			cout << "COM Error: " << e.ErrorMessage() << endl;
			log.TRACE_ERR("COM Error: %s", e.ErrorMessage());
		} else {
			cout << "COM Error: " << e.Description() << endl;
			log.TRACE_ERR("COM Error: %s", (char *)e.Description());
		}
		setConsoleColor(wOrigin);
		return -1;
	}
	ado.RecordSetClose();

	ado.ExitConnect();

#ifdef _DEBUG
	cout << "========= mapECU =========" << endl;
	for (auto item : mapECU) {
		cout << item.first << ": (" << item.second.first << ": " << item.second.second << ")" << endl;
	}
#endif // _DEBUG

	// 生成INI文件
	string strFilePath;
	for (auto item : mapVehicleType) {
		strFilePath = INI_PATH;
		strFilePath += "\\" + item.first + ".ini";
		mkMultiDir(strFilePath);
		string strSection;
		string strKey;
		string strValue;
		size_t iECUQty = 0;
		try {
			strSection = "MAIN";
			strKey = "VEHICLE_TYPE";
			strValue = item.first;
			writeINI(strSection, strKey, strValue, strFilePath);

			strSection = "MAIN";
			strKey = "ECU_QTY";
			iECUQty = item.second.size();
			strValue = to_string(iECUQty);
			writeINI(strSection, strKey, strValue, strFilePath);

			for (size_t i = 0; i < iECUQty; i++) {
				strSection = "ECU" + to_string(i + 1);
				strKey = "ECU_NAME";
				strValue = mapECU.at(item.second[i]).first;
				writeINI(strSection, strKey, strValue, strFilePath);

				strSection = "ECU" + to_string(i + 1);
				strKey = "TEST_ITEM_LIST";
				strValue = mapECU.at(item.second[i]).second;
				writeINI(strSection, strKey, strValue, strFilePath);
			}

		} catch (DWORD) {
			WORD wOrigin = setConsoleColor(12, 14);
			cout << "Write " << item.first << ".ini:" << strKey << "=" << strValue << " failed!" << endl;
			setConsoleColor(wOrigin);
			log.TRACE_ERR("Write %s.ini:%s=%s failed!", item.first.c_str(), strKey.c_str(), strValue.c_str());
			return -1;
		}
	}
	return 0;
}
