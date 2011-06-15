#include "SquadGraphics.h"

#include <stdlib.h>
#include <time.h>

#include "Core.h"
#include "DataLibrary.h"

#include "UnitGrap.h"
#include "AnimationBlender.h"
#include "BillboardManager.h"
#include "GUISquadBillBoard.h"

#include "Terrain.h"

const Ogre::Vector3 LineVector[5]={Ogre::Vector3(0,0,4),Ogre::Vector3(-4,0,4),Ogre::Vector3(4,0,4),Ogre::Vector3(2,0,-2),Ogre::Vector3(-2,0,-2)};
const Ogre::Vector3 CircularVector[5]={Ogre::Vector3(0,0,3),Ogre::Vector3(-3,0,1),Ogre::Vector3(3,0,1),Ogre::Vector3(2,0,-2),Ogre::Vector3(-2,0,-2)};
const Ogre::Vector3 LooseVector[5]={Ogre::Vector3(0,0,0),Ogre::Vector3(-3,0,3),Ogre::Vector3(3,0,3),Ogre::Vector3(3,0,-3),Ogre::Vector3(-3,0,-3)};

#define FORMATION_KEYFRAME_TIME 1
#define RELIEF_MOVE_TIME 1

SquadGraphics::SquadGraphics(std::string squadName,std::string datapath,Ogre::Vector2& grid,Direction direction,Formation f,unsigned int index,int soldierCount):
mSquadId(squadName),
mID(index),
mPUSystem(NULL),
mPUSystemEnd(false),
mNodeAnimation(NULL),
mNodeAnimationState(NULL),
mDeathUnit(NULL),
mReliefUnit(NULL),
mSoldierIndex(0),
mFormation(f),
mDirection(direction)
{
	mSceneMgr=Core::getSingletonPtr()->mSceneMgr;
	mNode=mSceneMgr->getRootSceneNode()->createChildSceneNode(squadName+Ogre::StringConverter::toString(index));

	//获取数据
	//std::string mainWeaponName;
	//std::string secWeaponName;
	//std::string shieldName;

	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/CommanderUnit"),mCommanderUnitName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/CommanderMainWeapon"),mainWeaponName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/CommanderSecWeapon"),secWeaponName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/CommanderShield"),shieldName);

	DataLibrary* datalib = DataLibrary::getSingletonPtr();
	datalib->getData(datapath + std::string("/LeaderMesh"),mLeaderMesh);
	datalib->getData(datapath + std::string("/LeaderMat"),mLeaderMat);
	datalib->getData(datapath + std::string("/UnitMesh"),mSoilderMesh);
	datalib->getData(datapath + std::string("/UnitMat"),mSoilderMat);
	datalib->getData(datapath + std::string("/UnitEffect/EffectName"),mSoilderEffect);
	datalib->getData(datapath + std::string("/UnitEffect/EffectOffect"),mSoilderEffectOffect);

	std::string tempid;
	datalib->getData(datapath + std::string("/PweaponId"),tempid);
	if(tempid != "none")
	{
		datalib->getData( std::string("StaticData/PweaponData/") + tempid + std::string("/Mesh"), mPWeaponMesh);
		datalib->getData( std::string("StaticData/PweaponData/") + tempid + std::string("/Mat"), mPWeaponMat);
	}
	else
	{
		mPWeaponMesh = "none";
		mPWeaponMat = "none";
	}
	datalib->getData(datapath + std::string("/SweaponId"),tempid);
	if(tempid != "none")
	{
		datalib->getData( std::string("StaticData/SweaponData/") + tempid + std::string("/Mesh"), mSWeaponMesh);
		datalib->getData( std::string("StaticData/SweaponData/") + tempid + std::string("/Mat"), mSWeaponMat);
	}
	else
	{
		mSWeaponMesh = "none";
		mSWeaponMat = "none";
	}
	datalib->getData(datapath + std::string("/ShieldId"),tempid);
	if(tempid != "none")
	{
		datalib->getData( std::string("StaticData/ShieldData/") + tempid + std::string("/Mesh"), mShieldMesh);
		datalib->getData( std::string("StaticData/ShieldData/") + tempid + std::string("/Mat"), mShieldMat);
	}
	else
	{
		mShieldMesh = "none";
		mShieldMat = "none";
	}
	datalib->getData(datapath + std::string("/TeamId"),tempid);
	datalib->getData( std::string("GameData/BattleData/Team/") + tempid + std::string("/FactionId"), tempid);
	datalib->getData( std::string("StaticData/FactionData/") + tempid + std::string("/Texture"), mFactionTexture);
	

	//组建单位队伍与组建武器
	mCommanderUnit=new UnitGrap(mLeaderMesh,mLeaderMat,mFactionTexture,mSceneMgr->getRootSceneNode()->createChildSceneNode(mNode->getName()+"_Commander"));
	mCommanderUnit->createWeapon(mPWeaponMesh,mPWeaponMat,UnitGrap::MainWepon);
	mCommanderUnit->createWeapon(mSWeaponMesh,mSWeaponMesh,UnitGrap::SecWepon);
	mCommanderUnit->createWeapon(mShieldMesh,mShieldMat,UnitGrap::Shield);

	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/SoldierUnit"),mSoldierUnitName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/SoldierMainWeapon"),mMainWeaponName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/SoldierSecWeapon"),mSecWeaponName);
	//DataLibrary::getSingletonPtr()->getData(std::string("GameData/Squad/")+squadName+std::string("/SoldierShield"),mShieldName);
	for (int i=0;i<soldierCount;i++)
	{
		createSoldier();
	}
	
	//设置参数
	setGrid(grid.x,grid.y);
	setFormation(f,false);
	setDirection(direction,false);
	setWeaponMode(SquadGraphics::MainWepon);
	
	//Camera* cam=mSceneMgr->getCamera("PlayerCam");
	//Matrix4 viewMatrix = cam->getViewMatrix();
	//Matrix4 projectMatrix = cam->getProjectionMatrix();
	//Matrix4 eyeMatrix = (Matrix4(0.5,0,0,0.5, 0,-0.5,0,0.5, 0,0,1,0, 0,0,0,1)*(projectMatrix*viewMatrix));
	//Vector4 temp = eyeMatrix*v4;
	//Vector3 ScreenPos = Vector3(temp.x/temp.w,temp.y/temp.w,temp.z/temp.w);


	//mNode->attachObject(BillboardManager::getSingletonPtr()->getBillboardSet());
	mSquadBB=new GUISquadBillBoard(mCommanderUnit->mNode);
	BillboardManager::getSingletonPtr()->addBillBoard(mSquadBB);

}

SquadGraphics::~SquadGraphics(void)
{
	delete mCommanderUnit;
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		delete (*it);
	}

	if (mPUSystem!=NULL)
	{
		mNode->detachObject(mPUSystem);
		Core::getSingletonPtr()->destroyPUSystem(mPUSystem);
		mPUSystem=NULL;
	}

	if (mNodeAnimationState!=NULL)
	{
		mSceneMgr->destroyAnimationState(mNode->getName()+"_AniState");
		mSceneMgr->destroyAnimation(mNode->getName()+"_Ani");
	}
	
	mNode->removeAndDestroyAllChildren();
	mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());

	BillboardManager::getSingletonPtr()->destroyBillBoard(mSquadBB);
}

UnitGrap* SquadGraphics::createSoldier()
{
	mSoldierIndex++;
	UnitGrap* unit=new UnitGrap(mSoilderMesh,mSoilderMat,mFactionTexture,mSceneMgr->getRootSceneNode()->createChildSceneNode(mNode->getName()+"_Soldier"+Ogre::StringConverter::toString(mSoldierIndex)));

	unit->createWeapon(mPWeaponMesh,mPWeaponMat,UnitGrap::MainWepon);
	unit->createWeapon(mSWeaponMesh,mSWeaponMesh,UnitGrap::SecWepon);
	unit->createWeapon(mShieldMesh,mShieldMat,UnitGrap::Shield);

	if (mSoilderEffect!="none")
	{
		unit->setEffect(mSoilderEffect,mSoilderEffectOffect);
	}

	mSoldierUnits.push_back(unit);

	return unit;
}

void SquadGraphics::setMovePath(std::map<int,Ogre::Vector3>& vectors,std::map<int,Ogre::Quaternion>& quaternions,std::map<int,Direction>& directions)
{
	/*mNodeAnimation = mSceneMgr->createAnimation(mNode->getName()+"_Ani", vectors.size()*MOVE_KEYFRAME_TIME);
	mNodeAnimation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	Ogre::NodeAnimationTrack* track = mNodeAnimation->createNodeTrack(1, mNode);
	
	float timePosition=0;
	Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(timePosition);
	kf->setTranslate(mNode->getPosition());
	kf->setRotation(mNode->getOrientation());

	std::map<int,Ogre::Vector3>::iterator itr  =  vectors.begin();
	for(  ;  itr !=  vectors.end();  ++itr )
	{
		timePosition+=MOVE_KEYFRAME_TIME;
		kf = track->createNodeKeyFrame(timePosition);

		kf->setTranslate(itr->second);

		std::map<int,Ogre::Quaternion>::iterator itr1;
		itr1 = quaternions.find(itr->first);

		if( itr1 != quaternions.end() )
		{
			kf->setRotation(itr1->second);
		}
	}
	
	mNodeAnimationState = mSceneMgr->createAnimationState(mNode->getName()+"_Ani");

	setCheckUnitHeight(true);
	mNodeAnimationState->setLoop(false);
	mNodeAnimationState->setEnabled(true);

	mCommanderUnit->setAnimation(mCommanderUnit->mWalkName,true,false);
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->setAnimation((*it)->mWalkName,true,false);
	}
	mReturnInitAni=true;*/
	
	if (vectors.size()==0)
	{
		return;
	}

	Ogre::Vector3 v=vectors[0];
	Ogre::Vector3 df=mNode->getPosition();
	
	if(v.z!=df.z)
	{
		if (v.z-df.z<0)
		{
			setDirection(North,false);
		}
		else
		{
			setDirection(South,false);
		}
	}

	if(v.x!=df.x)
	{
		if (v.x-df.x<0)
		{
			setDirection(West,false);
		}
		else
		{
			setDirection(East,false);
		}
	}

	mNode->setPosition((vectors.end().operator --())->second);
	mDirection=(directions.end().operator --())->second;

	Ogre::Vector3 CommanderVector;
	Ogre::Vector3 SoldierVector[4];

	getFormationPosition(mFormation,mDirection,CommanderVector,SoldierVector);

	std::map<int,Ogre::Vector3>* newVectors=getUnitMovePath(mCommanderUnit,vectors,directions,true);
	mCommanderUnit->setMovePath(*newVectors,quaternions);
	mCommanderUnit->mOffsetX=CommanderVector.x;
	mCommanderUnit->mOffsetY=CommanderVector.z;
	delete newVectors;

	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		newVectors=getUnitMovePath((*it),vectors,directions,false);
		(*it)->setMovePath(*newVectors,quaternions);
		(*it)->mOffsetX=SoldierVector[(*it)->mFormationPosition].x;
		(*it)->mOffsetY=SoldierVector[(*it)->mFormationPosition].z;
		delete newVectors;
	}
}

std::map<int,Ogre::Vector3>* SquadGraphics::getUnitMovePath( UnitGrap* unit,std::map<int,Ogre::Vector3>& vectors,std::map<int,Direction>& directions,bool isCommander)
{
	std::map<int,Ogre::Vector3>* newVectors=new std::map<int,Ogre::Vector3>();
	std::map<int,Ogre::Vector3>::iterator itr  =  vectors.begin();
	float w1,w2=0;

	for(  ;  itr !=  vectors.end();  ++itr )
	{
		std::map<int,Direction>::iterator itr1;
		itr1 = directions.find(itr->first);

		if( itr1 != directions.end() )
		{
			
			Ogre::Vector3 dv;
			Ogre::Vector3 v=vectors[itr->first];
			
			Ogre::Vector3 CommanderVector;
			Ogre::Vector3 SoldierVector[4];

			getFormationPosition(mFormation,itr1->second,CommanderVector,SoldierVector);

			if (isCommander)
			{
				dv=CommanderVector;
			}
			else
			{
				dv=SoldierVector[unit->mFormationPosition];
			}

			v.x=v.x+dv.x;
			v.z=v.z+dv.z;

			(*newVectors)[itr->first]=v;
		}
	}

	return newVectors;
}

bool SquadGraphics::isTransformOver()
{
	if (mCommanderUnit->mNodeAnimationState!=NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void SquadGraphics::stopTransform()
{
	mSceneMgr->destroyAnimationState(mNode->getName()+"_Ani");
	mSceneMgr->destroyAnimation(mNode->getName()+"_Ani");
	mNodeAnimation=NULL;
	mNodeAnimationState=NULL;
	setCheckUnitHeight(false);
}

void SquadGraphics::setAnimation(std::string name,Object object,bool isLoop,bool returnInit)
{
	switch(object)
	{
	case Squad:
		{

		}
	case Commander:
		{
			mCommanderUnit->setAnimation(name,isLoop,returnInit);
			if (object!=Squad)
			{
				break;
			}
		}
	case Soldier:
		{
			for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
			{
				(*it)->setAnimation(name,isLoop,returnInit);
			}
			break;
		}
	}
}


void SquadGraphics::setInitAnimation( Object object )
{
	switch(object)
	{
	case Squad:
		{

		}
	case Commander:
		{
			mCommanderUnit->mAniBlender->BackToInit();
			if (object!=Squad)
			{
				break;
			}
		}
	case Soldier:
		{
			for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
			{
				(*it)->mAniBlender->BackToInit();
			}
			break;
		}
	}
}


bool SquadGraphics::isAnimationOver(Object object)
{
	if (object==Commander)
	{
		return mCommanderUnit->mAniBlender->complete;
	}
	else
	{
		if (mSoldierUnits.size()!=0)
		{
			return mSoldierUnits.at(0)->mAniBlender->complete;
		}
		else
		{
			return true;
		}
	}
}


void SquadGraphics::setEffect( std::string name,Object object)
{
	switch(object)
	{
	case Squad:
		{
			if (mPUSystem!=NULL)
			{
				mNode->detachObject(mPUSystem);
				Core::getSingletonPtr()->destroyPUSystem(mPUSystem);
				mPUSystem=NULL;
			}

			mPUSystem=Core::getSingletonPtr()->createPUSystem(mNode->getName()+"_PU",name);
			mPUSystem->addParticleSystemListener(this);
			mPUSystem->prepare();
			mNode->attachObject(mPUSystem);
			mPUSystem->start();
			break;
		}
	case Commander:
		{
			mCommanderUnit->setEffect(name);
			break;
		}
	case Soldier:
		{
			for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
			{
				(*it)->setEffect(name);
			}
			break;
		}
	}
}

void SquadGraphics::getFormationPosition(Formation f,Direction d,Ogre::Vector3& CommanderVector,Ogre::Vector3 SoldierVector[])
{
	switch(f)//确定各个位置
	{
	case Line:
		{
			CommanderVector=LineVector[0];

			for (int i=1;i<5;i++)
			{
				SoldierVector[i-1]=LineVector[i];
			}
			break;
		}
	case Circular:
		{
			CommanderVector=CircularVector[0];

			for (int i=1;i<5;i++)
			{
				SoldierVector[i-1]=CircularVector[i];
			}
			break;
		}
	case Loose:
		{
			CommanderVector=LooseVector[0];

			for (int i=1;i<5;i++)
			{
				SoldierVector[i-1]=LooseVector[i];
			}
			break;
		}
	}

	int w1,w2;
	switch(d)
	{
	case North:
		{
			w1=-1;
			w2=0;
			break;
		}
	case South:
		{
			w1=1;
			w2=0;
			break;
		}
	case West:
		{
			w1=0;
			w2=1;
			break;
		}
	case East:
		{
			w1=0;
			w2=-1;
			break;
		}
	}

	//x'=xcos-ysin y'=ycos+xsin
	Ogre::Vector3 V;
	V.x=CommanderVector.x*w1-CommanderVector.z*w2;
	V.y=CommanderVector.y;
	V.z=CommanderVector.z*w1+CommanderVector.x*w2;
	CommanderVector=V;

	for (int i=0;i<4;i++)
	{
		V.x=SoldierVector[i].x*w1-SoldierVector[i].z*w2;
		V.y=SoldierVector[i].y;
		V.z=SoldierVector[i].z*w1+SoldierVector[i].x*w2;
		SoldierVector[i]=V;
	}
}

void SquadGraphics::setFormation( Formation f,bool isAnim )
{
	mFormation=f;
	Ogre::Vector3 CommanderVector;
	Ogre::Vector3 SoldierVector[4];

	getFormationPosition(f,mDirection,CommanderVector,SoldierVector);

	if (isAnim)
	{
		mNodeAnimation = mSceneMgr->createAnimation(mNode->getName()+"_Ani",FORMATION_KEYFRAME_TIME);
		mNodeAnimation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
		Ogre::NodeAnimationTrack* track = mNodeAnimation->createNodeTrack(0,mCommanderUnit->mNode);

		Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(0);
		kf->setTranslate(mCommanderUnit->mNode->getPosition());
		kf->setRotation(mCommanderUnit->mNode->getOrientation());
		kf->setScale(mCommanderUnit->mNode->getScale());
		Ogre::Quaternion q=mCommanderUnit->mNode->getOrientation();

		kf=track->createNodeKeyFrame(FORMATION_KEYFRAME_TIME);
		Ogre::Vector3 v=CommanderVector;
		mCommanderUnit->mOffsetX=v.x;
		mCommanderUnit->mOffsetY=v.z;
		Ogre::Vector3 np=mNode->getPosition();
		v+=np;
		v.y=Terrain::getSingletonPtr()->getHeight(v.x,v.z);
		kf->setTranslate(v);
		//Ogre::Quaternion destQ;
		//Ogre::Vector3 lv=mCommanderUnit->mNode->getPosition();
		//destQ.FromAngleAxis(lv.angleBetween(v),Ogre::Vector3(0,1,0));
		kf->setRotation(mCommanderUnit->mNode->getOrientation());
		kf->setScale(mCommanderUnit->mNode->getScale());

	/*	kf=track->createNodeKeyFrame(2);
		kf->setTranslate(v);
		kf->setRotation(mCommanderUnit->mNode->getOrientation());
		kf->setScale(mCommanderUnit->mNode->getScale());*/

		int i=0;
		for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
		{
			track = mNodeAnimation->createNodeTrack(i+1,(*it)->mNode);

			Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(0);
			kf->setTranslate((*it)->mNode->getPosition());
			kf->setRotation((*it)->mNode->getOrientation());
			kf->setScale((*it)->mNode->getScale());
			//Ogre::Quaternion q=(*it)->mNode->getOrientation();

			//kf=track->createNodeKeyFrame(1.5);
			v=SoldierVector[i];
			(*it)->mOffsetX=v.x;
			(*it)->mOffsetY=v.z;
			v+=mNode->getPosition();
			v.y=Terrain::getSingletonPtr()->getHeight(v.x,v.z);
			//kf->setTranslate(v);
			//kf->setRotation((*it)->mNode->getPosition().getRotationTo(v));
			//kf->setScale((*it)->mNode->getScale());

			kf=track->createNodeKeyFrame(FORMATION_KEYFRAME_TIME);
			kf->setTranslate(v);
			kf->setRotation((*it)->mNode->getOrientation());
			kf->setScale((*it)->mNode->getScale());
			(*it)->mFormationPosition=i;
			i++;
		}

		mNodeAnimationState = mSceneMgr->createAnimationState(mNode->getName()+"_Ani");

		setCheckUnitHeight(true);
		mNodeAnimationState->setLoop(false);
		mNodeAnimationState->setEnabled(true);

		mCommanderUnit->setAnimation(mCommanderUnit->mWalkName,true,false);
		for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
		{
			(*it)->setAnimation((*it)->mWalkName,true,false);
		}
		mReturnInitAni=true;

	}
	else
	{
		mCommanderUnit->setPositionOffset(CommanderVector.x,CommanderVector.z);
		
		int i=0;
		for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
		{
			(*it)->setPositionOffset(SoldierVector[i].x,SoldierVector[i].z);
			(*it)->mFormationPosition=i;
			i++;
		}
	}
}

void SquadGraphics::handleParticleSystemEvent( ParticleUniverse::ParticleSystem *particleSystem, ParticleUniverse::ParticleUniverseEvent &particleUniverseEvent )
{
	if (particleUniverseEvent.componentType==ParticleUniverse::CT_SYSTEM && particleUniverseEvent.eventType==ParticleUniverse::PU_EVT_NO_PARTICLES_LEFT)
	{
		mPUSystemEnd=true;
	}
}

bool SquadGraphics::isEffectOver( Object object )
{
	switch(object)
	{
	case Squad:
		{
			return mPUSystemEnd;
			break;
		}
	case Commander:
		{
			mCommanderUnit->isEffectOver();
			break;
		}
	case Soldier:
		{
			if (mSoldierUnits.size()!=0)
			{
				return mSoldierUnits.at(0)->isEffectOver();
			}
			else
			{
				return true;
			}
			break;
		}
	}

	return true;
}

void SquadGraphics::stopEffect( Object object )
{
	if (mPUSystem!=NULL)
	{
		switch(object)
		{
		case Squad:
			{
				mPUSystem->stop();
				break;
			}
		case Commander:
			{
				mCommanderUnit->stopEffect();
				break;
			}
		case Soldier:
			{
				for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
				{
					(*it)->stopEffect();
				}
				break;
			}
		}
	}
}

void SquadGraphics::setGrid(int x,int y)
{
	float wx,wy=0;
	Terrain::getSingletonPtr()->getWorldCoords(x,y,wx,wy);
	
	mNode->setPosition(wx,0,wy);

	mCommanderUnit->setPosition(wx,wy);
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->setPosition(wx,wy);
	}
}

void SquadGraphics::setScale( Ogre::Vector3 scale,bool isAnim )
{

}

void SquadGraphics::setDirection( Direction d,bool isAnim )
{
	mDirection=d;

	Ogre::Vector3 CommanderVector;
	Ogre::Vector3 SoldierVector[4];

	getFormationPosition(mFormation,d,CommanderVector,SoldierVector);

	Ogre::Quaternion q;

	switch(d)
	{
	case North:
		{
			q.FromAngleAxis(Ogre::Degree(180),Ogre::Vector3(0,1,0));
			break;
		}
	case South:
		{
			q.FromAngleAxis(Ogre::Degree(360),Ogre::Vector3(0,1,0));
			break;
		}
	case West:
		{
			q.FromAngleAxis(Ogre::Degree(270),Ogre::Vector3(0,1,0));
			break;
		}
	case East:
		{
			q.FromAngleAxis(Ogre::Degree(90),Ogre::Vector3(0,1,0));
			break;
		}
	}
	//x'=xcos-ysin y'=ycos+xsin
	mCommanderUnit->setPositionOffset(CommanderVector.x,CommanderVector.z);
	mCommanderUnit->mNode->setOrientation(q);
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->setPositionOffset(SoldierVector[(*it)->mFormationPosition].x,SoldierVector[(*it)->mFormationPosition].z);
		(*it)->mNode->setOrientation(q);
	}

	//if (isAnim)
	//{
	//	mNodeAnimation = mSceneMgr->createAnimation(mNode->getName()+"_Ani", 2);
	//	mNodeAnimation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	//	Ogre::NodeAnimationTrack* track = mNodeAnimation->createNodeTrack(1, mNode);

	//	Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(0);
	//	kf->setTranslate(mNode->getPosition());
	//	kf->setRotation(mNode->getOrientation());

	//	kf = track->createNodeKeyFrame(2);
	//	kf->setTranslate(mNode->getPosition());
	//	kf->setRotation(q);

	//	mNodeAnimationState = mSceneMgr->createAnimationState(mNode->getName()+"_Ani");
	//	
	//	setCheckUnitHeight(true);
	//	mNodeAnimationState->setLoop(false);
	//	mNodeAnimationState->setEnabled(true);

	//	mCommanderUnit->setAnimation(mCommanderUnit->mRotationName,true,false);
	//	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	//	{
	//		(*it)->setAnimation((*it)->mRotationName,true,false);
	//	}
	//	mReturnInitAni=true;
	//}
	//else
	//{
	//	mNode->setOrientation(q);
	//}
}

void SquadGraphics::setCheckUnitHeight( bool enable )
{
	mCommanderUnit->mIsCheckHeight=enable;
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->mIsCheckHeight=enable;
	}
}

void SquadGraphics::setWeaponMode( WeaponMode mode )
{
	UnitGrap::WeaponType type;
	if (mode==MainWepon)
	{
		type=UnitGrap::MainWepon;
	}
	else
	{
		type=UnitGrap::SecWepon;
	}

	mCommanderUnit->setWeapon(type,UnitGrap::RightHand);
	mCommanderUnit->setWeapon(UnitGrap::Shield,UnitGrap::LeftHand);
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->setWeapon(type,UnitGrap::RightHand);
		(*it)->setWeapon(UnitGrap::Shield,UnitGrap::LeftHand);
	}
}

void SquadGraphics::setDeath()
{
	if (mSoldierUnits.size()!=0)//还有士兵?
	{
		//随机选择
		int i;
		if (mSoldierUnits.size()!=1)
		{
			i=rand()%(mSoldierUnits.size()-1);
		}
		else
		{
			i=0;
		}

		mDeathUnit=mSoldierUnits.at(i);

		mReliefUnit=NULL;

		if (mDeathUnit->mFormationPosition<2)//如果是前排?
		{
			int findF;//确定士兵队伍里面还有没有替补?
			if (i==0)
			{
				findF=3;
			}
			else
			{
				findF=2;
			}
			
			for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
			{
				if ((*it)->mFormationPosition==findF)
				{
					mReliefUnit=(*it);
				}
			}

			if (mReliefUnit!=NULL)//如果找到了替补队员
			{
				mReliefUnit->mFormationPosition=mDeathUnit->mFormationPosition;
				mReliefUnit->mOffsetX=mDeathUnit->mOffsetX;
				mReliefUnit->mOffsetY=mDeathUnit->mOffsetY;
			}

		}

	}
	else
	{
		mDeathUnit=mCommanderUnit;
		mReliefUnit=NULL;
	}

	//执行死亡动画
	mDeathUnit->setAnimation(mDeathUnit->mDeathName,false,false);
	mDeathStep=playAni;

}

void SquadGraphics::doDeathStep()
{
	switch(mDeathStep)
	{
	case playAni:
		{
			//等待死亡动画结束
			if (mDeathUnit->mAniBlender->complete)
			{
				mDeathStep=changeMat;
				mDeathUnit->setFadeInOut(false);
			}

			break;
		}
	case changeMat:
		{
			if (mReliefUnit!=NULL)//需要替补?
			{
				mDeathStep=moveUnit;
			}
			else
			{
				mDeathStep=resetAni;//终止
			}
			break;
		}
	case moveUnit:
		{
			mReliefAniName=mReliefUnit->mAniBlender->getSource()->getAnimationName();
			mReliefAniLoop=mReliefUnit->mAniBlender->getSource()->getLoop();
			
			//设置节点动画
			mNodeAnimation = mSceneMgr->createAnimation(mNode->getName()+"_Ani", RELIEF_MOVE_TIME);
			mNodeAnimation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
			Ogre::NodeAnimationTrack* track = mNodeAnimation->createNodeTrack(1, mReliefUnit->mNode);

			Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(0);
			kf->setTranslate( mReliefUnit->mNode->getPosition());
			kf->setRotation( mReliefUnit->mNode->getOrientation());
			kf->setScale(mReliefUnit->mNode->getScale());

			kf = track->createNodeKeyFrame(RELIEF_MOVE_TIME);
			kf->setTranslate(mDeathUnit->mNode->getPosition());
			kf->setRotation(mReliefUnit->mNode->getOrientation());
			kf->setScale( mReliefUnit->mNode->getScale());

			mNodeAnimationState = mSceneMgr->createAnimationState(mNode->getName()+"_Ani");

			//setCheckUnitHeight(true);
			mNodeAnimationState->setLoop(false);
			mNodeAnimationState->setEnabled(true);

			mReturnInitAni=false;

			//设置跑步动画
			mReliefUnit->setAnimation(mReliefUnit->mWalkName,true,false);
			
			mDeathStep=resetAni;

			break;
		}
	case resetAni:
		{
			if (mNodeAnimation==NULL)
			{
				if(mReliefUnit!=NULL)
				{
					//回复原来动画
					mReliefUnit->setAnimation(mReliefAniName,mReliefAniLoop,false);
					mReliefUnit=NULL;
				}
				
				if (mDeathUnit->mAlpha==-1)
				{
					//移除单位
					for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
					{
						if ((*it)==mDeathUnit)
						{
							mSoldierUnits.erase(it);
							break;
						}
					}

					if (mDeathUnit==mCommanderUnit)
					{
						mCommanderUnit=NULL;
					}

					delete mDeathUnit;
					mDeathUnit=NULL;
				}
			}
			break;
		}
	}
}

void SquadGraphics::stopDeath()
{
	if (mReliefUnit!=NULL)
	{
		if (mNodeAnimation!=NULL)
		{
			stopTransform();
		}

		mReliefUnit->mNode->setPosition(mDeathUnit->mNode->getPosition());
	}

	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		if ((*it)==mDeathUnit)
		{
			mSoldierUnits.erase(it);
			break;
		}
	}

	delete mDeathUnit;
	mDeathUnit=NULL;
}

bool SquadGraphics::isDeathOver()
{
	if (mDeathUnit!=NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool SquadGraphics::isRecoverOver()
{
	bool over=true;
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		if((*it)->mAniBlender->getSource()->getAnimationName()==(*it)->mRecoverName)
		{
			over=false;
			break;
		}
	}
	return over;
}

void SquadGraphics::setRecover(int num)
{
	int size=mSoldierUnits.size();
	if (mSoldierUnits.size()+num<5)
	{
		for (int i=0;i<num;i++)
		{
			int findIndex=-1;
			
			for (int j=0;j<mSoldierUnits.size();j++)
			{
				bool flag=false;
				for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
				{
					if((*it)->mFormationPosition==j)
					{
						flag=true;
						break;
					}
				}

				if (!flag)//没有找到?
				{
					findIndex=j;
					break;
				}
			}

			UnitGrap* unit=createSoldier();
			unit->mFormationPosition=findIndex;
				
			Ogre::Vector3 CommanderVector;
			Ogre::Vector3 SoldierVector[4];

			getFormationPosition(mFormation,mDirection,CommanderVector,SoldierVector);
			unit->setPositionOffset(SoldierVector[unit->mFormationPosition].x,SoldierVector[unit->mFormationPosition].z);

			unit->setFadeInOut(true);
			unit->setAnimation(unit->mRecoverName,false,true);

		}
	}
}


void SquadGraphics::update( unsigned int deltaTime )
{
	if (mDeathUnit!=NULL)
	{
		doDeathStep();
	}
	
	if (mNodeAnimationState!=NULL)
	{
		if (mNodeAnimationState->getTimePosition() >= mNodeAnimationState->getLength())
		{
			stopTransform();
			if (mReturnInitAni)
			{
				setInitAnimation(Squad);
			}
		}
		else
		{
			mNodeAnimationState->addTime(deltaTime/1000.0f);
		}
	}

	if (mCommanderUnit!=NULL)
	{
		mCommanderUnit->update(deltaTime);
	}
	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->update(deltaTime);
	}
}

void SquadGraphics::setVisible(bool visible)
{
	mCommanderUnit->mNode->setVisible(visible);

	for (std::vector<UnitGrap*>::iterator it=mSoldierUnits.begin();it!=mSoldierUnits.end();it++)
	{
		(*it)->mNode->setVisible(visible);
	}
}

Direction SquadGraphics::getDirection()
{
	return mDirection;
}

