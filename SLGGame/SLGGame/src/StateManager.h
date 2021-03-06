#pragma once

#include <string>

#include "GameState.h"

#include "iisingleton.hpp"

using namespace izayoi;

//状态管理器,用于管理各种状态
class StateManager:public IISingleton<StateManager>
{
public:
	StateManager(void);
	~StateManager(void);
	
	enum StateType//状态枚举
	{
		None,
		Menu,
		AVG,
		Battle,
		Supply
	};

	//改变主状态
	void changeState(std::string arg,StateType type);
	
	//添加附加状态
	void addAffixationState(std::string arg,StateType type);
	
	//移除附加状态
	void removeAffixationState();

	//存在附加状态?
	bool hasAffixationState();
	
	//帧更新
	void StateUpdate(unsigned int deltaTime);

	//文件保存处理
	void saveState(std::string file);

	//文件载入处理
	void loadState(std::string file);

	StateType curState() { return mCurState; }

	StateType nextState() {return mNextState; }

private:
	GameState* CreateState(StateType type);

	GameState* mWillChangeState;
	GameState* mBaseState;
	GameState* mAffixationState;

	StateType  mCurState;
	StateType  mNextState;
	std::string mNextStateArg;
};
