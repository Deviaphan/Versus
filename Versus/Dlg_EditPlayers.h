#pragma once

#include <QDialog>
#include "PlayerItem.h"

class QMouseEvent;
namespace Ui { class Dlg_EditPlayers; };

class PlayerListItem;
class Versus;

class Dlg_EditPlayers : public QDialog
{
	Q_OBJECT

public:
	Dlg_EditPlayers( QWidget* parent, Versus* versus );
	~Dlg_EditPlayers() override;

	void UpdateList();

	void SetCurrentItem( PlayerListItem* pli, const PlayerItem* item );
	
public slots:
	void OnEditPlayerlogo();
	void OnEditTeamlogo();
	void OnAddNewPlayer();
	void OnEditTeam();

	void OnSortByName();
	void OnSortByTeam();

	void OnDeleteItem( const PlayerItem * item );

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private:
	void SetPlayerLogo( const QString& logoPath );
	void SetTeamLogo( const QString& logoPath );
	void PerhapsUpdateTeamLogo( const QString& teamName );

private:
	Ui::Dlg_EditPlayers *ui;
	PlayerListItem* _selectedItem;

	QString _playerLogo;
	QString _teamLogo;

	bool _sortByName;

public:
	Versus* _versus;
	PlayerItem _selectedItemData;
};
