#pragma once

#include <QObject>
#include <QModelIndex>

namespace Ui
{
	class MainWindow;
}

class SoldierTableModule;
class HorseTableModule;
class PWeaponTableModule;
class SWeaponTableModule;
class ArmorTableModule;
class ShieldTableModule;
class SkillTableModule;
class EffectTableModule;
class SquadTableModule;
class SquadSkillTableModule;
class StringTableModule;

class QTableView;
class QComboBox;

class DataEditorControl:public QObject
{
	Q_OBJECT
public:
	DataEditorControl(Ui::MainWindow *ui);
	~DataEditorControl();

	void RefreshModList();
	void RefreshLangList();

	QComboBox *mModBox;
	QComboBox *mLangBox;
	
	QTableView *mSoldierTableView;
	QTableView *mHorseTableView;
	QTableView *mPWeaponTableView;
	QTableView *mSWeaponTableView;
	QTableView *mArmorTableView;
	QTableView *mShieldTableView;
	QTableView *mSkillTableView;
	QTableView *mEffectTableView;
	QTableView *mSquadTableView;
	QTableView *mSquadSkillTableView;
	QTableView *mStringTableView;

	SoldierTableModule *mSoldierTableModule;
	HorseTableModule *mHorseTableModule;
	PWeaponTableModule *mPWeaponTableModule;
	SWeaponTableModule *mSWeaponTableModule;
	ArmorTableModule *mArmorTableModule;
	ShieldTableModule *mShieldTableModule;
	SkillTableModule *mSkillTableModule;
	EffectTableModule *mEffectTableModule;
	SquadTableModule *mSquadTableModule;
	SquadSkillTableModule *mSquadSkillTableModule;
	StringTableModule *mStringTableModule;

	//接收界面消息
public slots:
	void ModBoxChanged(const QString & text);
	void LangBoxChanged(const QString & text);
	void AddMod();
	void AddLang();
	void AddSoldier();
	void DelSoldier();
	void AddHorse();
	void DelHorse();
	void AddPWeapon();
	void DelPWeapon();
	void AddSWeapon();
	void DelSWeapon();
	void AddArmor();
	void DelArmor();
	void AddShield();
	void DelShield();
	void AddSkill();
	void DelSkill();
	void AddEffect();
	void DelEffect();
	void AddSquad();
	void DelSquad();
	void AddSquadSkill();
	void DelSquadSkill();
	void AddString();
	void DelString();
	void SelectSquad(QModelIndex _index);
};