#pragma once
#include "PlayerItem.h"
#include "PlayerItemWidget.h"

class PlayerItemData: public PlayerItem
{
public:
	void UpdateItem()
	{
		if( widget )
		{
			widget->SetTitle( itemTitle );
			widget->WriteItem();
		}
	}

public:
	PlayerItemWidget* widget = Q_NULLPTR;
	QString itemTitle;
	QString score = "0";
};