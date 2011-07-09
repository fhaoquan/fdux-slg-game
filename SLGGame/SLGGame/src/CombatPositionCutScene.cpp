#include "CombatPositionCutScene.h"

#include "SquadGraphics.h"
#include "SquadGrapManager.h"

CombatPositionCutScene::CombatPositionCutScene(unsigned int id,Direction direction,bool isBack):mDirection(direction),mIsBack(isBack)
{
	mSquadGraphics=SquadGrapManager::getSingletonPtr()->getSquad(id);
}

CombatPositionCutScene::~CombatPositionCutScene(void)
{
}

bool CombatPositionCutScene::endCutScene()
{
	return mSquadGraphics->isTransformOver();
}

void CombatPositionCutScene::skipCutScene()
{

}

void CombatPositionCutScene::updateCutScene( unsigned int deltaTime )
{

}

void CombatPositionCutScene::startCutScence()
{
	if (!mIsBack)
	{
		mSquadGraphics->combatPosition(mDirection);
	}
	else
	{
		mSquadGraphics->idlePosition();
	}
}
