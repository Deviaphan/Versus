#pragma once

#include <QWidget>
#include "ui_PlayerGroupItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PlayerGroupItemClass; };
QT_END_NAMESPACE

class Group;
class Dlg_SetPlayerGroup;

class PlayerGroupItem : public QWidget
{
	Q_OBJECT

public:
	PlayerGroupItem( Dlg_SetPlayerGroup *parent);
	~PlayerGroupItem();

public:
	void SetItem( const Group* item );
	void TurnOff();
	void TurnOn();

private:
	Ui::PlayerGroupItemClass *ui;
	Dlg_SetPlayerGroup* _parent;
	const Group* _item;
	int _lastClickTime;
};
