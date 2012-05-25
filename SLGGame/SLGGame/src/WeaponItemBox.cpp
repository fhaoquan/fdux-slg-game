/*!
	@file
	@author		Albert Semenov
	@date		07/2008
*/
#include "Precompiled.h"
#include "WeaponItemBox.h"


WeaponItemBox::WeaponItemBox(MyGUI::Widget* _parent) :
	wraps::BaseItemBox<WeaponCellView>(_parent)
{
}

WeaponItemBox::~WeaponItemBox()
{
	MyGUI::ItemBox* box = getItemBox();
	size_t count = box->getItemCount();
	for (size_t pos = 0; pos < count; ++pos)
	{
		delete *box->getItemDataAt<WeaponItemData*>(pos);
	}
}
