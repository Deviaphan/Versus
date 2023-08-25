// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "PlayerManager.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "SharedTools.h"

constexpr char DB_filename[] = "players.json";

constexpr char DB_playerNames[] = "db_players";
constexpr char DB_teamNames[] = "db_teams";
constexpr char DB_groupNames[] = "db_groups";


PlayerManager& PlayerManager::Instance()
{
	static PlayerManager pm;
	return pm;
}

std::vector<PlayerItem>& PlayerManager::PlayerDB()
{
	return Instance()._playerDB;
}

std::vector<TeamItem>& PlayerManager::TeamDB()
{
	return Instance()._teamDB;
}

::std::vector<Group>& PlayerManager::PlayerGroupDB()
{
	return Instance()._playerGroupDB;
}

void PlayerManager::SaveDB() const
{
	const QString fullFileName = QCoreApplication::applicationDirPath() + "/" + DB_filename;

	if( _playerDB.empty() && _teamDB.empty() )
	{
		if( QFile::exists( fullFileName ) )
		{
			QFile::remove( fullFileName );
		}

		return;
	}

	QJsonObject rootObj;

	{
		QJsonArray playerArray;
		for( const auto& pi : _playerDB )
		{
			if( pi.player.isEmpty() )
				continue;

			QJsonObject item;

			item.insert( "playerName", pi.player );
			item.insert( "teamName", pi.team );
			item.insert( "playerLogo", pi.playerLogo );
			item.insert( "teamLogo", pi.teamLogo );

			playerArray.push_back( item );
		}
		rootObj.insert( DB_playerNames, playerArray );
	}
	{
		QJsonArray teamArray;
		for( const auto& ti : _teamDB )
		{
			QJsonObject item;

			item.insert( "teamName", ti.team );
			item.insert( "teamLogo", ti.teamLogo );

			teamArray.push_back( item );
		}
		rootObj.insert( DB_teamNames, teamArray );
	}

	{
		QJsonArray groupArray;
		for( const auto& gi : _playerGroupDB )
		{
			QJsonObject playerGroup;

			playerGroup.insert( "title", gi.title );

			QJsonArray items;
			{
				for( auto& pi : gi.items )
				{
					QJsonObject item;

					item.insert( "player", pi.player );
					item.insert( "team", pi.team );
					item.insert( "score", pi.score );

					items.push_back( item );
				}
			}
			playerGroup.insert( "group", items );

			groupArray.push_back( playerGroup );
		}
		rootObj.insert( DB_groupNames, groupArray );
	}

	QFile outFile( fullFileName );
	outFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text );

	const QJsonDocument saveDoc( rootObj );
	outFile.write( saveDoc.toJson() );
}

void PlayerManager::LoadDB()
{
	const QString fullFileName = QCoreApplication::applicationDirPath() + "/" + DB_filename;

	QFile inFile( fullFileName );
	inFile.open( QIODevice::ReadOnly | QIODevice::Text );
	const QByteArray data = inFile.readAll();
	inFile.close();

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( data, &errorPtr );
	if( doc.isNull() )
	{
		return;
	}

	const QJsonObject rootObj = doc.object();

	_playerDB.clear();
	{
		const QJsonArray dataArray = rootObj[DB_playerNames].toArray();
		for( const QJsonValueConstRef value : dataArray )
		{
			if( !value.isObject() )
				continue;

			const QJsonObject p = value.toObject();

			PlayerItem pid;

			pid.player = ReadJsonValue( p, "playerName", "Player" );
			pid.team = ReadJsonValue( p, "teamName", "Team" );
			pid.playerLogo = ReadJsonValue( p, "playerLogo", "" );
			pid.teamLogo = ReadJsonValue( p, "teamLogo", "" );

			_playerDB.emplace_back( pid );
		}
	}

	_teamDB.clear();
	{
		const QJsonArray dataArray = rootObj[DB_teamNames].toArray();
		for( const QJsonValueConstRef value : dataArray )
		{
			if( !value.isObject() )
				continue;

			const QJsonObject p = value.toObject();

			TeamItem tid;

			tid.team = ReadJsonValue( p, "teamName", "Team" );
			tid.teamLogo = ReadJsonValue( p, "teamLogo", "" );

			_teamDB.emplace_back( tid );
		}
	}

	_playerGroupDB.clear();
	{
		const QJsonArray groupArray = rootObj[DB_groupNames].toArray();
		for( const QJsonValueConstRef value : groupArray )
		{
			if( !value.isObject() )
				continue;

			const QJsonObject playerGroup = value.toObject();

			auto& pg = _playerGroupDB.emplace_back();

			pg.title = ReadJsonValue( playerGroup, "title", "-/-" );

			const QJsonArray items = playerGroup["group"].toArray();
			for( const QJsonValueConstRef gValue : items )
			{
				const QJsonObject gItem = gValue.toObject();

				auto& item = pg.items.emplace_back();

				item.player = ReadJsonValue( gItem, "player" );
				item.team = ReadJsonValue( gItem, "team" );
				item.score = ReadJsonValue( gItem, "score" );
			}
		}
	}
}

void PlayerManager::AddPlayer( const PlayerItem& item )
{
	if( item.player.isEmpty() )
		return;

	for( auto& ii : _playerDB )
	{
		if( ii.player == item.player )
		{
			ii = item;
			return;
		}
	}

	_playerDB.emplace_back( item );
}

void PlayerManager::AddTeam( const PlayerItem& item )
{
	if( item.team.isEmpty() )
		return;

	for( auto& ii : _teamDB )
	{
		if( ii.team == item.team )
		{
			ii.teamLogo = item.teamLogo;
			return;
		}
	}

	TeamItem ti;
	ti.team = item.team;
	ti.teamLogo = item.teamLogo;
	_teamDB.emplace_back( ti );
}

void PlayerManager::AddTeam( const TeamItem& item )
{
	if( item.team.isEmpty() )
		return;

	for( auto& ii : _teamDB )
	{
		if( ii.team == item.team )
		{
			ii.teamLogo = item.teamLogo;
			return;
		}
	}

	_teamDB.emplace_back( item );
}

PlayerItem* PlayerManager::FindPlayerItem( const QString& player, const QString& team )
{
	for( auto& ii : _playerDB )
	{
		if( ii.player == player && ii.team == team )
		{
			return &ii;
		}
	}

	return nullptr;
}
