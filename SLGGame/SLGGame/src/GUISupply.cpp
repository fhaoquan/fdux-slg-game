#include "GUISupply.h"

#include "DataLibrary.h"
#include "StringTable.h"
#include "StateManager.h"
#include "AVGSquadManager.h"

#include "boost/format.hpp"
#include <string>
#include <map>

GUISupply::GUISupply(int width,int height):GUIScene("supply.layout",width,height)
{
	setSceneLanguage();

	assignWidget(mSupplyBG,"SupplyBG");

	assignWidget(mSaveButton,"T_SupplySave");
	assignWidget(mLoadButton,"T_SupplyLoad");
	assignWidget(mExitButton,"T_SupplyExit");

	mSaveButton->eventMouseButtonClick+= MyGUI::newDelegate(this, &GUISupply::onSave);
	mLoadButton->eventMouseButtonClick+= MyGUI::newDelegate(this, &GUISupply::onLoad);
	mExitButton->eventMouseButtonClick+= MyGUI::newDelegate(this, &GUISupply::onExit);

	MyGUI::ItemBox* baseItemBox;
	assignWidget(baseItemBox,"PWeaponItemBox");
	mPWeaponItemBox=new WeaponItemBox(baseItemBox);
	mPWeaponItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mPWeaponItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"SWeaponItemBox");
	mSWeaponItemBox=new WeaponItemBox(baseItemBox);
	mSWeaponItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mSWeaponItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"ShieldItemBox");
	mShieldItemBox=new WeaponItemBox(baseItemBox);
	mShieldItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mShieldItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"HorseItemBox");
	mHorseItemBox=new WeaponItemBox(baseItemBox);
	mHorseItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mHorseItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"ArmorItemBox");
	mArmorItemBox=new WeaponItemBox(baseItemBox);
	mArmorItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mArmorItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"SoilderItemBox");
	mSoilderItemBox=new WeaponItemBox(baseItemBox);
	mSoilderItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventMouseItemActivate);
	mSoilderItemBox->getItemBox()->eventSelectItemAccept+= MyGUI::newDelegate(this, &GUISupply::eventSelectItemAccept);

	assignWidget(baseItemBox,"ArmyList");
	mSquadItemBox=new SquadItemBox(baseItemBox);
	mSquadItemBox->getItemBox()->eventMouseItemActivate+= MyGUI::newDelegate(this, &GUISupply::eventSquadMouseItemActivate);

	assignWidget(mSquadImage,"SquadImage");
	assignWidget(mTextSquadLeadName,"SquadLeadName");
	assignWidget(mTextSquadType,"SquadType");
	assignWidget(mTextSquadLv,"SquadLv");
	assignWidget(mTextSquadExp,"SquadExp");
	assignWidget(mTextSquadPWeapon,"SquadPWeapon");
	assignWidget(mTextSquadSWeapon,"SquadSWeapon");
	assignWidget(mTextSquadShield,"SquadShield");
	assignWidget(mTextSquadArmor,"SquadArmor");
	assignWidget(mTextSqureHorse,"SqureHorse");
	assignWidget(mTextSquadAttack,"SquadAttack");
	assignWidget(mTextSquadRangeAttack,"SquadRangeAttack");
	assignWidget(mTextSquadDefense,"SquadDefense");
	assignWidget(mTextSquadArray,"SquadArray");
	assignWidget(mTextSquadAgility,"SquadAgility");
	assignWidget(mTextSquadDetect,"SquadDetect");
	assignWidget(mTextSquadConcealment,"SquadConcealment");
	assignWidget(mTextSquadTenacity,"SquadTenacity");
	assignWidget(mTextSquadCounterattack,"SquadCounterattack");
	assignWidget(mTextSquadPeople,"SquadPeople");
	assignWidget(mTextSquadAP,"SquadAP");

	assignWidget(mTextItemName,"ItemName");
	assignWidget(mTextItemPrice,"ItemPrice");
	assignWidget(mItemIcon,"ItemIcon");
	assignWidget(mTextItemInfo,"ItemInfo");

	assignWidget(mTextArmyInfo,"ArmyInfo");

	assignWidget(mWeaponTabControl,"WeaponTabControl");
	mWeaponTabControl->eventTabChangeSelect+=MyGUI::newDelegate(this, &GUISupply::eventTabChangeSelect);

	int i=0;
	for(i=0;i<SQUAD_SKILL_NUM;i++)
	{
		assignWidget(mSquadSkillIcon[i],str(boost::format("SquadSkillIcon%1%")%(i+1)));
	}

	for(i=0;i<SQUAD_PASSIVE_SKILL_NUM;i++)
	{
		assignWidget(mSquadPassiveSkillIcon[i],str(boost::format("SquadPassiveSkillIcon%1%")%(i+1)));
	}

	for(i=0;i<SQUAD_USE_EQUIP_NUM;i++)
	{
		assignWidget(mSquadUseEquipIcon[i],str(boost::format("SquadUseEquipIcon%1%")%(i+1)));
	}

	for(i=0;i<SQUAD_EFFECT_NUM;i++)
	{
		assignWidget(mSquadEffectIcon[i],str(boost::format("SquadEffectIcon%1%")%(i+1)));
	}

	createBattleSquad();
	refreshArmyList();

	m_CurrSquadEquipItem=NULL;
	m_CurrSelectType=EQUIP_PWEAPON;
	m_CurrSquadIndex=-1;;
	m_Money=0;
	DataLibrary::getSingletonPtr()->getData("GameData/StoryData/Gold",m_Money);
	showArmyInfo();
}

GUISupply::~GUISupply(void)
{
	clearBattleSquad();
}

void GUISupply::clearBattleSquad()
{
	//����BattleDate
	for(std::vector<Squad*>::iterator it=mBattleSquad.begin();it!=mBattleSquad.end();it++)
	{
		delete (*it);
	}

	mBattleSquad.clear();
}

void GUISupply::createBattleSquad()
{
	//��ȡBattleData
	clearBattleSquad();

	std::vector<std::string> child=DataLibrary::getSingletonPtr()->getChildList("GameData/StoryData/SquadData");
	for(std::vector<std::string>::iterator it=child.begin();it!=child.end();it++)
	{
		Squad* bs=new Squad(std::string("GameData/StoryData/Supply/")+(*it));
		bs->init(std::string("GameData/StoryData/SquadData/")+(*it));
		mBattleSquad.push_back(bs);
	}
}

void GUISupply::refreshArmyList()
{
	mSquadItemBox->removeAllItems();
	for(std::vector<Squad*>::iterator it=mBattleSquad.begin();it!=mBattleSquad.end();it++)
	{
		mSquadItemBox->addItem(new SquadItemData((*it)->getSquadType(),(*it)->getName(),(*it)->getLevel()));
	}
}

void GUISupply::showArmy( int index )
{
	Squad* army=mBattleSquad.at(index);
	
	std::string tempstr,temppath;
	tempstr = army->getLeaderId();
	temppath = str(boost::format("StaticData/LeaderDate/%1%/Picture")%tempstr);
	bool re = DataLibrary::getSingletonPtr()->getData(temppath, tempstr);
	mSquadImage->setImageTexture(tempstr);

	mTextSquadLeadName->setCaption(army->getName());
	mTextSquadType->setCaption(getItemNameFormLanguage("LeaderDate",army->getLeaderId()));
	mTextSquadLv->setCaption(Ogre::StringConverter::toString(army->getLevel()));
	mTextSquadExp->setCaption(Ogre::StringConverter::toString(army->getExp()));
	mTextSquadPWeapon->setCaption(getItemNameFormLanguage("PweaponData",army->getPweaponId()));
	mTextSquadSWeapon->setCaption(getItemNameFormLanguage("SweaponData",army->getSweaponId()));
	mTextSquadShield->setCaption(getItemNameFormLanguage("ShieldData",army->getShieldId()));
	mTextSquadArmor->setCaption(getItemNameFormLanguage("ArmorData",army->getArmorId()));
	mTextSqureHorse->setCaption(getItemNameFormLanguage("HorseData",army->getHorseId()));

	showAttribute(index,0,"");
}

std::string GUISupply::itemCompare(Squad* compareSquad,Squad* squad,AttrType type)
{
	float oldAttr=squad->getAttr(type,ATTRCALC_FULL);

	if(compareSquad!=NULL)
	{
		float newAttr=compareSquad->getAttr(type,ATTRCALC_FULL);

		float difference=newAttr-oldAttr;
		if(difference>0)
		{
			return "#00FF00" + Ogre::StringConverter::toString(newAttr) +" (+"+Ogre::StringConverter::toString(difference)+")";
		}
		else if(difference==0)
		{
			return Ogre::StringConverter::toString(oldAttr);
		}
		else
		{
			return "#FF0000"+Ogre::StringConverter::toString(newAttr) +" (-"+Ogre::StringConverter::toString(abs(difference))+")";
		}
	}
	else
	{
		return Ogre::StringConverter::toString(oldAttr);
	}
}

void GUISupply::eventSquadMouseItemActivate(MyGUI::ItemBox* _sender, size_t _index)
{
	if(_index!=-1)
	{
		m_CurrSquadIndex=_index;
		showArmy(_index);
		setItemInfo(NULL);
		showItem(EQUIP_PWEAPON);
	}
}

void GUISupply::eventMouseItemActivate(MyGUI::ItemBox* _sender, size_t _index)
{
	if(_index!=-1)
	{
		WeaponItemData* item=*(_sender->getItemDataAt<WeaponItemData*>(_index));
		showAttribute(m_CurrSquadIndex,m_CurrSelectType,item->getID());
		setItemInfo(item);
	}
	else
	{
		showAttribute(m_CurrSquadIndex,0,"");
		setItemInfo(NULL);
	}
}

void GUISupply::eventSelectItemAccept(MyGUI::ItemBox* _sender, size_t _index)
{
	if(_index!=-1)
	{
		WeaponItemData* item=*(_sender->getItemDataAt<WeaponItemData*>(_index));
		buyItem(m_CurrSquadIndex,item);
	}
}

void GUISupply::showAttribute(int index,int itemType,std::string itemID)
{
	Squad* army=mBattleSquad.at(index);
	Squad* copySquad=NULL;
	if(itemID!="")
	{
		DataLibrary::getSingletonPtr()->copyNode(army->getPath(),"GameData/StoryData/SquadData/TempSquad");
		copySquad=new Squad("GameData/StoryData/SquadData/TempSquad");
		copySquad->equipEquipment(itemType,itemID);

	}

	mTextSquadAttack->setCaption(itemCompare(copySquad,army,ATTR_ATTACK));
	mTextSquadRangeAttack->setCaption(itemCompare(copySquad,army,ATTR_RANGEDATTACK));
	mTextSquadDefense->setCaption(itemCompare(copySquad,army,ATTR_DEFENCE));
	mTextSquadArray->setCaption(itemCompare(copySquad,army,ATTR_FORM));
	mTextSquadAgility->setCaption(itemCompare(copySquad,army,ATTR_INITIATIVE));
	mTextSquadDetect->setCaption(itemCompare(copySquad,army,ATTR_DETECTION));
	mTextSquadConcealment->setCaption(itemCompare(copySquad,army,ATTR_COVERT));
	mTextSquadTenacity->setCaption(itemCompare(copySquad,army,ATTR_TOUGHNESS));
	mTextSquadAP->setCaption(itemCompare(copySquad,army,ATTR_ACTIONPOINT));
	mTextSquadCounterattack->setCaption(itemCompare(copySquad,army,ATTR_CONTER));
	mTextSquadPeople->setCaption(Ogre::StringConverter::toString(army->getUnitNum()));
	
	m_SquadSkillNum=0;
	m_SquadPassiveSkillNum=0;
	m_SquadUseEquipNum=0;
	m_SquadEffectNum=0;
	std::map<std::string,enumtype> skillmap;
	if(itemID!="")
		skillmap=copySquad->getSkillTable();
	else
		skillmap=army->getSkillTable();

	for(std::map<std::string,enumtype>::iterator it=skillmap.begin();it!=skillmap.end();it++)
	{
		if(it->second==SKILLTYPE_ACTIVE)
		{
			mSquadSkillIcon[m_SquadSkillNum]->setItemResource(it->first);
			m_SquadSkillNum++;
		}
		else if(it->second==SKILLTYPE_PASSIVE)
		{
			mSquadPassiveSkillIcon[m_SquadPassiveSkillNum]->setItemResource(it->first);
			m_SquadPassiveSkillNum++;
		}
		else if (it->second==SKILLTYPE_EQUIP)
		{
			mSquadUseEquipIcon[m_SquadUseEquipNum]->setItemResource(it->first);
			m_SquadUseEquipNum++;
		}
	}

	if(itemID!="")
	{
		delete copySquad;
		DataLibrary::getSingletonPtr()->delNode("GameData/StoryData/SquadData/TempSquad");
	}

}

void GUISupply::showItem(int type)
{
	std::string path;
	WeaponItemBox* itemBox;
	std::string equipID;
	int equipType=type;
	Squad* army=mBattleSquad.at(m_CurrSquadIndex);
	m_CurrSelectType=(EquipmentType)type;

	switch(type)
	{
	case EQUIP_PWEAPON:
		path="StaticData/PweaponData";
		itemBox=mPWeaponItemBox;
		equipID=army->getPweaponId();
		break;
	case EQUIP_SWEAPON:
		path="StaticData/SweaponData";
		itemBox=mSWeaponItemBox;
		equipID=army->getSweaponId();
		break;
	case EQUIP_SHIELD:
		path="StaticData/ShieldData";
		itemBox=mShieldItemBox;
		equipID=army->getShieldId();
		break;
	case EQUIP_ARMOR:
		path="StaticData/ArmorData";
		itemBox=mArmorItemBox;
		equipID=army->getArmorId();
		break;
	case EQUIP_HORSE:
		path="StaticData/HorseData";
		itemBox=mHorseItemBox;
		equipID=army->getHorseId();
		break;
	case EQUIP_SOILDER:
		path="StaticData/SoilderData";
		itemBox=mSoilderItemBox;
		equipID=army->getSoilderId();
		break;
	}

	itemBox->removeAllItems();
	std::vector<std::string> child=DataLibrary::getSingletonPtr()->getChildList(path);
	for(std::vector<std::string>::iterator it=child.begin();it!=child.end();it++)
	{
		WeaponItemData* data=new WeaponItemData(equipType,(*it),army->getUnitNum());
		if((*it)==equipID)
		{
			data->setEquip(true);
			m_CurrSquadEquipItem=data;
		}
		itemBox->addItem(data);
	}
}

void GUISupply::buyItem(int index,WeaponItemData* item)
{
	Squad* army=mBattleSquad.at(index);

	if(!item->getEquip() && item->getCanBuy())
	{
		army->equipEquipment(item->getType(),item->getID());

		showArmy(m_CurrSquadIndex);
		showItem(item->getType());

		m_Money-=item->getPriceValue();

		if(m_CurrSquadEquipItem!=NULL)
			m_Money+=m_CurrSquadEquipItem->getPriceValue()/2;

		DataLibrary::getSingletonPtr()->setData("GameData/StoryData/Gold",m_Money);
		showArmyInfo();
	}
}

std::string GUISupply::getItemNameFormLanguage(std::string type,std::string name)
{

	if(name == "none")
		return StringTable::getSingleton().getString(name);
	else
	{
		std::string tempstr;
		std::string temppath = std::string("StaticData/")+type+"/"+ name + std::string("/Name");
		DataLibrary::getSingletonPtr()->getData(temppath,tempstr);
		return tempstr;
	}
}

void GUISupply::eventTabChangeSelect(MyGUI::TabControl* _sender, size_t _index)
{
	if(m_CurrSquadIndex!=-1)
	{
		showAttribute(m_CurrSquadIndex,0,"");
		setItemInfo(NULL);
		switch(_index)
		{
		case 0:
			showItem(EQUIP_PWEAPON);
			break;
		case 1:
			showItem(EQUIP_SWEAPON);
			break;
		case 2:
			showItem(EQUIP_SHIELD);
			break;
		case 3:
			showItem(EQUIP_HORSE);
			break;
		case 4:
			showItem(EQUIP_ARMOR);
			break;
		case 5:
			showItem(EQUIP_SOILDER);
			break;
		}
	}
}

void GUISupply::setItemInfo(WeaponItemData* item)
{
	if(item!=NULL)
	{
		Squad* army=mBattleSquad.at(m_CurrSquadIndex);
		mTextItemName->setCaption(item->getName());
		mTextItemPrice->setCaption(str(boost::format(StringTable::getSingletonPtr()->getString("ItemPrice"))%item->getPriceValue()%item->getOnePrice()%army->getUnitNum()));
		mItemIcon->setItemResourcePtr(item->getImage());
		mItemIcon->setItemGroup("States");
		mItemIcon->setVisible(true);
	}
	else
	{
		mTextItemInfo->setCaption("");
		mTextItemPrice->setCaption("");
		mTextItemName->setCaption("");
		mItemIcon->setVisible(false);
	}
}

void GUISupply::showArmyInfo()
{
	DataLibrary::getSingletonPtr()->getData("GameData/StoryData/Gold",m_Money);
	std::string info;
	info+=str(boost::format(StringTable::getSingletonPtr()->getString("ArmyInfo_Money"))%m_Money);
	info+="\n";
	mTextArmyInfo->setCaption(info);
}

void GUISupply::onSave(MyGUI::Widget* _sender)
{
}

void GUISupply::onLoad(MyGUI::Widget* _sender)
{
}

void GUISupply::onExit(MyGUI::Widget* _sender)
{
	for(std::vector<Squad*>::iterator it=mBattleSquad.begin();it!=mBattleSquad.end();it++)
	{
		AVGSquadManager::getSingletonPtr()->dumpSquad((*it)->getSquadId(),(*it));
	}
	
	StateManager::getSingletonPtr()->changeState(mNextScript,StateManager::AVG);
}

void GUISupply::showScene( std::string arg )
{
	mNextScript=arg;
}

void GUISupply::hideScene()
{

}

void GUISupply::FrameEvent()
{

}
