#pragma once
#include <QJsonObject>
#include <vector>
#include "PlayerItem.h"

class PlayerManager
{
private:
	PlayerManager() = default;
public:
	~PlayerManager() = default;

public:
	static PlayerManager& Instance();

	static ::std::vector<PlayerItem>& PlayerDB();

	static ::std::vector<TeamItem>& TeamDB();

	static ::std::vector<Group>& PlayerGroupDB();

public:
	void SaveDB() const;

	void LoadDB();

	void AddPlayer( const PlayerItem& item );
	void AddTeam( const PlayerItem& item );
	void AddTeam( const TeamItem& item );

	PlayerItem* FindPlayerItem( const QString& player, const QString& team );

private:
	::std::vector<PlayerItem> _playerDB; // база данных игроков
	::std::vector<TeamItem> _teamDB; // база данных команд

	::std::vector<Group> _playerGroupDB; // база данных сохранённых групп игроков для быстрого переключения
};
