#pragma once

#include <ticpp.h>
#include <string>
#include "Modifier.h"


class ArmorManager
{
public:
	ArmorManager();
	~ArmorManager();

	//bool LoadMod(std::wstring modName,std::wstring langName, bool editorMode);
	//bool LoadLang(std::wstring langName);

	//编辑器用函数
	//bool SaveData();
	//bool SaveLang();

	int GetNum(); //获取总数
	std::wstring GetID(int index);
	std::wstring GetName(std::wstring id);
	std::wstring GetDescription (std::wstring id);
	int GetAttr(std::wstring id,BasicAttr attrType);
	std::wstring GetScriptName(std::wstring id);
	int GetArmorType(std::wstring id);
	int GetValue(std::wstring id);

	void AddArmor();
	void DelArmor(std::wstring id);

	bool SetID(std::wstring oldid, std::wstring id);
	bool SetName(std::wstring id, std::wstring name);
	bool SetDescription(std::wstring id, std::wstring descripition);
	bool SetAttr(std::wstring id, BasicAttr attrType, int attr);
	bool SetScriptName(std::wstring id, std::wstring script);
	bool SetArmorType(std::wstring id, int type);
	bool SetValue(std::wstring id, int ivalue);

private:

	//std::wstring modPath_;
	//std::string dataPath_;
	//std::string langPath_;

	//ticpp::Document dataFile_;
	//ticpp::Document langFile_;

	//bool CreateDataFile();
	//bool CreateLangFile();
};