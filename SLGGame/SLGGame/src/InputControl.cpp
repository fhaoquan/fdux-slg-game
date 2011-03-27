#include "InputControl.h"

#include "GUISystem.h"
#include "CameraContral.h"

InputControl::InputControl(void)
{
	mCameraContral = NULL;
}

InputControl::~InputControl(void)
{
}

bool InputControl::keyPressed( const OIS::KeyEvent &arg )
{
	return mGUISystem->keyPressed(arg);
}

bool InputControl::keyReleased( const OIS::KeyEvent &arg )
{
	return mGUISystem->keyReleased(arg);
}

bool InputControl::mouseMoved( const OIS::MouseEvent &arg )
{
	if (!mGUISystem->mouseMoved(arg))
	{
		mCameraContral->moveCamera((arg.state.X.abs - 640 )/100.0f,(arg.state.Y.abs - 360) /100.0f);
	}
	return true;
}

bool InputControl::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (!mGUISystem->mousePressed(arg,id))
	{
		//mCameraContral->cameraQuery(arg.state.X.abs,arg.state.Y.abs);
	}
	return true;
}

bool InputControl::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (!mGUISystem->mouseReleased(arg,id))
	{

	}
	return true;
}

void InputControl::setCameraContral( CameraContral* camera )
{
	mCameraContral=camera;
}

void InputControl::setGUISystem( GUISystem* system )
{
	mGUISystem=system;
}