#pragma once
#include "cutscene.h"

#include <string>

#include "squaddefine.h"

class ChangeStateCutScene : public CutScene
{
public:
	ChangeStateCutScene(int nextstate, std::string arg);
	virtual ~ChangeStateCutScene(void);

	void startCutScence();
	bool endCutScene();
	void skipCutScene();
	void updateCutScene(unsigned int deltaTime);

	int mNextState;
	std::string mArg;

};