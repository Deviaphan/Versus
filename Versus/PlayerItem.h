#pragma once
#include <QString>
#include <vector>

class PlayerItem
{
public:
	QString player;
	QString playerLogo;

	QString team;
	QString teamLogo;

	QString scoreColor;
};

class TeamItem
{
public:
	QString team;
	QString teamLogo;
};

class GroupItem
{
public:
	QString player;
	QString team;
	QString score;
};

class Group
{
public:
	::std::vector<GroupItem> items;
	QString title;
};
