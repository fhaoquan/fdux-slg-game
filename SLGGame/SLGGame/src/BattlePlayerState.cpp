#include "BattlePlayerState.h"

#include <boost/format.hpp>

#include "CommonFunction.h"

#include "InputControl.h"
#include "CameraContral.h"

#include "DataLibrary.h"
#include "StringTable.h"

#include "GUISystem.h"
#include "GUIBattle.h"
#include "GUIGameStateWindows.h"
#include "GUIMenuWindow.h"
#include "GUIInfoWindow.h"
#include "GUISquadWindows.h"
#include "GUICommandWindows.h"
#include "GUITargetWindow.h"
#include "GUIMapWindow.h"
#include "SquadGrapManager.h"

#include "Terrain.h"
#include "BattleSquadManager.h"
#include "BattleSquad.h"

#include "AreaGrap.h"

#include "CutSceneBuilder.h"
#include "cutscenediretor.h"
#include "StateManager.h"

//#define MAPCREATE

BattlePlayerState::BattlePlayerState()
{
	mGUIBattle = static_cast<GUIBattle *>(GUISystem::getSingleton().getScene(BattleScene));
	GUISystem::getSingletonPtr()->setFrameUpdateScene(BattleScene);
	mGUIState = static_cast<GUIGameStateWindows *>(mGUIBattle->getSubWindow("GameState"));
	//mGUIState->setBattleState(mMainState);
	mGUIState->setAllowNextTurn(true);
	mGUIMenu=static_cast<GUIMenuWindow *>(GUISystem::getSingleton().createScene(MenuWindowsScene));
	mGUIMenu->hideScene();
	mGUISquad = static_cast<GUISquadWindows *>(mGUIBattle->getSubWindow("SquadWindow"));
	mGUISquad->setSquad(NULL);
	mGUICommand = static_cast<GUICommandWindows *>(mGUIBattle->getSubWindow("CommandWindow"));
	mGUICommand->setSquad(NULL);
	mGUICommand->setPlayerState(this);

	GUIMapWindow* mMapWindow =  static_cast<GUIMapWindow *>(mGUIBattle->getSubWindow("MapWindow"));
	mMapWindow->updateAllPlayerPointState();

	GUIInfoWindow* infoWindow=(GUIInfoWindow*)mGUIBattle->getSubWindow("InfoWindow");
	infoWindow->setCaption("PlayerTurn.png",MyGUI::Colour::White);
	infoWindow->showScene("");

	Core::getSingleton().mInputControl->pushListener(this);
	mMouseX = 640;
	mMouseY = 360;

	mCameraContral = CameraContral::getSingletonPtr();

	mControlState = PLAYERCONTROL_NONE;

	mSelectSquad = NULL;
	mSelectSkillId = "";

	mRangeGrap = NULL;
	mTargetAreaGrap = NULL;
	mLastTargetX = -1;
	mLastTargetY = -1;	

	isPassLC=false;

	createSelectPlane();
}
BattlePlayerState::~BattlePlayerState()
{
	Core::getSingleton().mInputControl->popListener();
	mGUISquad->setSquad(NULL);
	mGUICommand->setSquad(NULL);
	mGUICommand->setPlayerState(NULL);
	hideSelectPlane();
	SquadGrapManager::getSingletonPtr()->clearBBState();
}

void BattlePlayerState::update(unsigned int deltaTime)
{
#ifndef MAPCREATE
	float dx = 0.0f,dy = 0.0f;
	float dt = (float)deltaTime / 5.0f;
	if(mMouseX < 20)
		dx = -dt;
	if(mMouseX > 1260)
		dx = dt;
	if(mMouseY < 20)
		dy = -dt;
	if(mMouseY > 680)
		dy = dt;
	mCameraContral->moveCamera(dx,dy);
#else
	if (isPassLC)
	{
		mCameraContral->moveCamera(1,0);
	}
	else
	{
		mCameraContral->moveCamera(0,0);
	}
#endif // MAPCREATE
}

void BattlePlayerState::reactiveState()
{
	switch(mControlState)
	{
	case PLAYERCONTROL_NONE:
		break;
	case PLAYERCONTROL_CHOOSESKILL:
		mGUISquad->setSquad(mSelectSquad);
		mGUICommand->setSquad(mSelectSquad);
		setSelectPlanePosition();
		mGUIState->setAllowNextTurn(true);
		break;
	case PLAYERCONTROL_CHOOSETARGET:
		mGUISquad->setSquad(mSelectSquad);
		break;
	case PLAYERCONTROL_TURNSQUAD:
		mGUICommand->setSquad(mSelectSquad);
		mControlState = PLAYERCONTROL_CHOOSESKILL;
		break;
	case PLAYERCONTROL_MENU:
		break;
	}
}

void BattlePlayerState::backFromMenu()
{
	mControlState = PLAYERCONTROL_NONE;
}

bool BattlePlayerState::keyPressed(const OIS::KeyEvent &arg)
{
#ifdef MAPCREATE
	switch(arg.key)
	{
	case OIS::KC_W:
		{
			mCameraContral->mTranslateVector.z=-1;
			break;
		}
	case OIS::KC_S:
		{
			mCameraContral->mTranslateVector.z=1;
			break;
		}
	case OIS::KC_A:
		{
			mCameraContral->mTranslateVector.x=-1;
			break;
		}
	case OIS::KC_D:
		{
			mCameraContral->mTranslateVector.x=1;
			break;
		}
	case OIS::KC_Q:
		{
			mCameraContral->mTranslateVector.y=1;
			break;
		}
	case OIS::KC_E:
		{
			mCameraContral->mTranslateVector.y=-1;
			break;
		}
	case OIS::KC_LCONTROL:
		{
			isPassLC=true;
			break;
		}
	case OIS::KC_R:
		{
			mCameraContral->print();
			break;
		}
	}
#endif


	if (arg.key==OIS::KC_ESCAPE )
	{
		switch(mControlState)
		{
		case PLAYERCONTROL_NONE:
			mGUIMenu->setAllowSave(true);
			mGUIMenu->setNotifyState(this);
			mGUIMenu->showScene("PlayerState");
			mControlState = PLAYERCONTROL_MENU;
			break;
		case PLAYERCONTROL_CHOOSESKILL:
			mSelectSquad = NULL;
			mGUISquad->setSquad(NULL);
			mGUICommand->setSquad(NULL);
			mControlState = PLAYERCONTROL_NONE;
			break;
		case PLAYERCONTROL_CHOOSETARGET:
			if(mRangeGrap)
			{
				delete mRangeGrap;
				mRangeGrap = NULL;
			}
			if(mTargetAreaGrap)
			{
				delete mTargetAreaGrap;
				mTargetAreaGrap = NULL;
			}
			mGUISquad->setSquad(mSelectSquad);
			mGUICommand->setSquad(mSelectSquad);
			mControlState = PLAYERCONTROL_CHOOSESKILL;
			break;
		case PLAYERCONTROL_TURNSQUAD:
			mGUICommand->setSquad(mSelectSquad);
			mControlState = PLAYERCONTROL_CHOOSESKILL;
			break;
		case PLAYERCONTROL_MENU:
			mGUIMenu->hideScene();
			mControlState = PLAYERCONTROL_NONE;
			break;
		}
		return true;
	}
	return false;
}

bool BattlePlayerState::keyReleased(const OIS::KeyEvent &arg)
{
#ifdef MAPCREATE
	switch(arg.key)
	{
	case OIS::KC_W:
		{
			mCameraContral->mTranslateVector.z=0;
			break;
		}
	case OIS::KC_S:
		{
			mCameraContral->mTranslateVector.z=0;
			break;
		}
	case OIS::KC_A:
		{
			mCameraContral->mTranslateVector.x=0;
			break;
		}
	case OIS::KC_D:
		{
			mCameraContral->mTranslateVector.x=0;
			break;
		}
	case OIS::KC_Q:
		{
			mCameraContral->mTranslateVector.y=0;
			break;
		}
	case OIS::KC_E:
		{
			mCameraContral->mTranslateVector.y=0;
			break;
		}
	case OIS::KC_LCONTROL:
		{
			isPassLC=false;
		}
	}
#endif
	return false;
}

bool BattlePlayerState::mouseMoved(const OIS::MouseEvent &arg)
{
	mMouseX = arg.state.X.abs;
	mMouseY = arg.state.Y.abs;

	if(mX < mMouseX)
		mCameraContral->mDx = -0.01;
	else if(mX > mMouseX)
		mCameraContral->mDx = 0.01;
	else
		mCameraContral->mDx = 0.00;

	if(mY < mMouseY)
		mCameraContral->mDy = -0.01;
	else if(mY > mMouseY)
		mCameraContral->mDy = 0.01;
	else
		mCameraContral->mDy = 0.00;

	mX = mMouseX;
	mY = mMouseY;

	mGUIBattle->SceneInputEvent(arg.state.X.abs,arg.state.Y.abs);
	if(mControlState == PLAYERCONTROL_CHOOSETARGET)
	{
		int GX,GY;
		Terrain::getSingletonPtr()->coordinateToGrid(arg.state.X.abs,arg.state.Y.abs,GX,GY);
		if(GX != mLastTargetX || GY != mLastTargetY)
		{
			if(mRangeGrap->inArea(GX, GY))
			{
				std::vector<int> pathlist;
				if(mSelectSkillId == "move")
				{
					std::vector<BattleSquadManager::MoveNode> path = BattleSquadManager::getSingleton().getMovePath(mSelectSquad, GX, GY);
					for(unsigned int n = 0; n < path.size(); n++)
					{
						pathlist.push_back(path[n].x);
						pathlist.push_back(path[n].y);
					}
				}
				else
				{
					std::vector<BattleSquadManager::SkillNode> path = 
						BattleSquadManager::getSingleton().getSkillTargetArea(mSelectSquad, mSelectSkillId, GX, GY);
					for(unsigned int n = 0; n < path.size(); n++)
					{
						pathlist.push_back(path[n].x);
						pathlist.push_back(path[n].y);
					}
				}
				if(mTargetAreaGrap)
				{
					mTargetAreaGrap->changeArea(pathlist);
				}
				else
				{
					mTargetAreaGrap = new AreaGrap(pathlist, "CUBE_YELLOW", 0.6f);
				}
			}
			else if(mTargetAreaGrap)
			{
				delete mTargetAreaGrap;
				mTargetAreaGrap = NULL;
			}
			mLastTargetX = GX;
			mLastTargetY = GY;
		}
	}
	return false;
}

void BattlePlayerState::createSelectPlane()
{
	Ogre::Entity* planeEnt=NULL;

	if (!Core::getSingletonPtr()->mSceneMgr->hasEntity("SelectSquadPlane"))
	{
		Ogre::Plane plane;
		plane.normal = Ogre::Vector3::UNIT_Y;
		plane.d = 0;

		Ogre::MeshManager::getSingleton().createPlane("Myplane",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			24,24,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);
		planeEnt=Core::getSingletonPtr()->mSceneMgr->createEntity( "SelectSquadPlane", "Myplane" );
		planeEnt->setVisibilityFlags(VISMASK_TRANSPARENT);

		planeEnt->setMaterialName("SelectSquadPlane");
		planeNode=Core::getSingletonPtr()->mSceneMgr->getRootSceneNode()->createChildSceneNode("SelectSquadPlaneNode");

		planeNode->attachObject(planeEnt);
		planeNode->setVisible(false);
	}
	else
	{
		planeNode=Core::getSingletonPtr()->mSceneMgr->getSceneNode("SelectSquadPlaneNode");
		planeNode->setVisible(false);
	}
}

void BattlePlayerState::setSelectPlanePosition( int GX,int GY,float height )
{
	float wx,wy;
	Terrain::getSingletonPtr()->getWorldCoords(GX,GY,wx,wy);
	planeNode->setPosition(wx,Terrain::getSingletonPtr()->getHeight(wx,wy)+height,wy);
	planeNode->setVisible(true);
}

void BattlePlayerState::setSelectPlanePosition()
{	
	setSelectPlanePosition(mSelectSquad->getGridX(),mSelectSquad->getGridY(),1.0);
}

void BattlePlayerState::hideSelectPlane()
{
	planeNode->setVisible(false);
}

bool BattlePlayerState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	int GX,GY;
	Terrain::getSingletonPtr()->coordinateToGrid(arg.state.X.abs,arg.state.Y.abs,GX,GY);
	if(id == OIS::MB_Left)
	{
		switch(mControlState)
		{
		case PLAYERCONTROL_NONE:
			{
				BattleSquad* squad = BattleSquadManager::getSingleton().getBattleSquadAt(GX,GY,0,true);
				if(squad)
				{
					if(squad ->getTeam() == 1)
					{
						mSelectSquad = squad;
						mGUISquad->setSquad(mSelectSquad);
						mGUICommand->setSquad(mSelectSquad);
						mControlState = PLAYERCONTROL_CHOOSESKILL;

						setSelectPlanePosition();
						float wx;
						float wy;
						Terrain::getSingletonPtr()->getWorldCoords(mSelectSquad->getGridX(),mSelectSquad->getGridY(),wx,wy);
						CameraContral::getSingleton().moveCameraTo(wx,wy);
					}
				}
			}
			break;
		case PLAYERCONTROL_CHOOSESKILL:
			{
				BattleSquad* squad = BattleSquadManager::getSingleton().getBattleSquadAt(GX,GY,0,true);
				if(squad)
				{
					if(squad!= mSelectSquad && squad ->getTeam() == 1)
					{
						mSelectSquad = squad;
						mGUISquad->setSquad(mSelectSquad);
						mGUICommand->setSquad(mSelectSquad);

						setSelectPlanePosition();
						float wx;
						float wy;
						Terrain::getSingletonPtr()->getWorldCoords(mSelectSquad->getGridX(),mSelectSquad->getGridY(),wx,wy);
						CameraContral::getSingleton().moveCameraTo(wx,wy);
					}
				}
			}
			break;
		case PLAYERCONTROL_CHOOSETARGET:
			{
				BattleSquadManager* battlesquadmanager = BattleSquadManager::getSingletonPtr();
				int GX,GY;
				Terrain::getSingletonPtr()->coordinateToGrid(arg.state.X.abs,arg.state.Y.abs,GX,GY);
				if(mRangeGrap->inArea(GX, GY))
				{
					if(mSelectSkillId == "move")
					{
						std::vector<int> pathlist;
						std::vector<BattleSquadManager::MoveNode> path = battlesquadmanager->getMovePath(mSelectSquad, GX, GY);
						std::vector<BattleSquadManager::MoveNode>::reverse_iterator rite;
						for(rite = path.rbegin(); rite != path.rend(); rite++)
						{
							pathlist.push_back((*rite).x);
							pathlist.push_back((*rite).y);
						}
						unsigned int evt = 0;
						unsigned intstopindex = 0;
						battlesquadmanager->moveSquad(mSelectSquad, pathlist, intstopindex, evt);
						if(evt == MOVEEVENT_SPOT)
						{

						}
					}
					else
					{
						unsigned int evt = 0;
						battlesquadmanager->useSkill(mSelectSquad, mSelectSkillId, GX, GY, evt);
						if(evt == MOVEEVENT_SPOT)
						{

						}
					}
					if(CutSceneBuilder::getSingleton().hasCutScenes())
					{
						CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
						mMainState->PushState(cutscenedirector);
						mGUICommand->setSquad(NULL);
						mGUIState->setAllowNextTurn(false);
					}
	 				else
	 				{
	 					mGUICommand->setSquad(mSelectSquad);
						mGUISquad->setSquad(mSelectSquad);
	 				}
					if(mRangeGrap)
					{
						delete mRangeGrap;
						mRangeGrap = NULL;
					}
					if(mTargetAreaGrap)
					{
						delete mTargetAreaGrap;
						mTargetAreaGrap = NULL;
					}
					hideSelectPlane();
					mControlState = PLAYERCONTROL_CHOOSESKILL;
				}
			}
			break;
		case PLAYERCONTROL_TURNSQUAD:
			{
				int GX,GY;
				Terrain::getSingletonPtr()->coordinateToGrid(arg.state.X.abs,arg.state.Y.abs,GX,GY);
				int d = GetDirection(mSelectSquad->getGridX(), mSelectSquad->getGridY(), GX, GY);
				BattleSquadManager::getSingleton().setDirection(mSelectSquad, d);
				if(CutSceneBuilder::getSingleton().hasCutScenes())
				{
					CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
					mMainState->PushState(cutscenedirector);
				}
			}
			break;
		case PLAYERCONTROL_MENU:
			break;
		}
		return true;
	}
	else if(id == OIS::MB_Right)
	{
		switch(mControlState)
		{
		case PLAYERCONTROL_CHOOSESKILL:
			hideSelectPlane();
			mSelectSquad = NULL;
			mGUISquad->setSquad(NULL);
			mGUICommand->setSquad(NULL);
			mControlState = PLAYERCONTROL_NONE;
			break;
		case PLAYERCONTROL_CHOOSETARGET:
			if(mRangeGrap)
			{
				delete mRangeGrap;
				mRangeGrap = NULL;
			}
			if(mTargetAreaGrap)
			{
				delete mTargetAreaGrap;
				mTargetAreaGrap = NULL;
			}
			mGUISquad->setSquad(mSelectSquad);
			mGUICommand->setSquad(mSelectSquad);
			mControlState = PLAYERCONTROL_CHOOSESKILL;
			break;
		case PLAYERCONTROL_TURNSQUAD:
			mGUICommand->setSquad(mSelectSquad);
			mControlState = PLAYERCONTROL_CHOOSESKILL;
			break;
		}
		return true;
	}
	return false;
}
bool BattlePlayerState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return false;
}

void BattlePlayerState::useSkill(std::string skillid)
{
	BattleSquadManager* battlesquadmanager = BattleSquadManager::getSingletonPtr();
	if(skillid == "move")
	{
		if(mSelectSquad->canMove())
		{
			std::map<int, BattleSquadManager::MoveNode> movenodes = battlesquadmanager->getMoveArea(mSelectSquad);
	
			std::vector<int> movearealist;
			std::map<int, BattleSquadManager::MoveNode>::iterator ite = movenodes.begin();
			for(; ite != movenodes.end(); ite++)
			{
				movearealist.push_back(ite->second.x);
				movearealist.push_back(ite->second.y);
			}
			if(mRangeGrap)
				delete mRangeGrap;
			mRangeGrap = new AreaGrap(movearealist, "CUBE_BLUE");
			if(mTargetAreaGrap)
			{
				delete mTargetAreaGrap;
				mTargetAreaGrap = NULL;
			}
			mLastTargetX = -1;
			mLastTargetY = -1;	
			mSelectSkillId = "move";
			mGUICommand->setSquad(NULL);
			mControlState = PLAYERCONTROL_CHOOSETARGET;
		}
	}
	else if(skillid == "turn")
	{
		mGUICommand->setSquad(NULL);
		mControlState = PLAYERCONTROL_TURNSQUAD;
	}
	else if(skillid == "looseformation")
	{
		BattleSquadManager::getSingleton().changeFormation(mSelectSquad, Loose, true);
		if(CutSceneBuilder::getSingleton().hasCutScenes())
		{
			CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
			mMainState->PushState(cutscenedirector);
			mGUICommand->setSquad(NULL);
		}
	}
	else if(skillid == "lineformation")
	{
		BattleSquadManager::getSingleton().changeFormation(mSelectSquad, Line, true);
		if(CutSceneBuilder::getSingleton().hasCutScenes())
		{
			CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
			mMainState->PushState(cutscenedirector);
			mGUICommand->setSquad(NULL);
		}
	}
	else if(skillid == "circularformation")
	{
		BattleSquadManager::getSingleton().changeFormation(mSelectSquad, Circular, true);
		if(CutSceneBuilder::getSingleton().hasCutScenes())
		{
			CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
			mMainState->PushState(cutscenedirector);
			mGUICommand->setSquad(NULL);
		}
	}
	else
	{
// 		DataLibrary* datalib = DataLibrary::getSingletonPtr();
// 		int skilltype;
// 		bool re = datalib->getData(str(boost::format("StaticData/SkillData/%1%/Type")%skillid),skilltype);
// 		if(re)
// 		{
// 			if(skilltype == SKILLTARGETTYPE_TARGETSELF)
// 			{
// 				mSelectSquad->useSkillOn(mSelectSquad, skillid);
// 				if(CutSceneBuilder::getSingleton().hasCutScenes())
// 				{
// 					CutSceneDirector* cutscenedirector = new CutSceneDirector(CutSceneBuilder::getSingleton().getCutScenes());
// 					mMainState->PushState(cutscenedirector);
// 					mGUICommand->setSquad(NULL);
// 				}
// 				else
// 				{
// 					mGUICommand->setSquad(mSelectSquad);
// 				}
// 			}
// 			else
// 			{
				std::vector<BattleSquadManager::SkillNode> skillnodes = battlesquadmanager->getSkillArea(mSelectSquad, skillid);

				std::vector<int> skillarealist;
				std::vector<BattleSquadManager::SkillNode>::iterator ite = skillnodes.begin();
				for(; ite != skillnodes.end(); ite++)
				{
					if((*ite).validTarget)
					{
						skillarealist.push_back((*ite).x);
						skillarealist.push_back((*ite).y);
					}
				}

				if(mRangeGrap)
					delete mRangeGrap;
				mRangeGrap = new AreaGrap(skillarealist, "CUBE_RED");
				if(mTargetAreaGrap)
				{
					delete mTargetAreaGrap;
					mTargetAreaGrap = NULL;
				}
				mLastTargetX = -1;
				mLastTargetY = -1;	
				mSelectSkillId = skillid;
				mGUICommand->setSquad(NULL);
				mControlState = PLAYERCONTROL_CHOOSETARGET;
//			}
//		}
	}
	mGUISquad->setSquad(mSelectSquad);
}