#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class PlayerListItemClass;
};
QT_END_NAMESPACE

class PlayerItem;
class Dlg_EditPlayers;

class PlayerListItem: public QWidget
{
	Q_OBJECT

public:
	PlayerListItem( Dlg_EditPlayers* parent );
	~PlayerListItem() override;

	void SetItem( PlayerItem* item )
	{
		_item = item;
	}

	void UpdateListItem();

	void TurnOff();
	void TurnOn();

public slots:
	void OnEditPlayerlogo();
	void OnEditTeamlogo();

	void OnEditTeam();

protected:
	void SetPlayerLogo( const QString& logoPath );
	void SetTeamLogo( const QString& logoPath );
	void PerhapsUpdateTeamLogo( const QString& teamName );

private:
	Ui::PlayerListItemClass* ui;
	Dlg_EditPlayers* _parent;
	PlayerItem* _item;

	int _lastClickTime;
};
