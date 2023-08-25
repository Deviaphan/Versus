#pragma once

#include <vector>
#include <QString>

class ThemeInfo
{
public:
	struct Style
	{
		QString name;

		std::vector<QString> colors;
	};

public:
	QString dir;
	int currentStyle = 0;

	QString author;
	QString homepage;
	QString title;
	QString game;
	QString sourceSize;
	QString align;

	int numPlayers = 0;

	std::vector<Style> styles;
	int numStyles = 0;
};

void ReadInfo( const QString& themeFile, ThemeInfo& info );

