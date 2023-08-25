// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "ThemeInfo.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "../SharedTools.h"

void ReadInfo( const QString& themeFile, ThemeInfo& info )
{
	QFile inFile( themeFile );
	inFile.open( QIODevice::ReadOnly | QIODevice::Text );
	const QByteArray data = inFile.readAll();
	inFile.close();

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( data, &errorPtr );
	if( doc.isNull() )
	{
		return;
	}

	QJsonObject rootObj = doc.object();

	info.author = ReadJsonValue( rootObj, "author" );
	info.homepage = ReadJsonValue( rootObj, "homepage" );
	info.title  = ReadJsonValue( rootObj, "title");
	info.game = ReadJsonValue( rootObj, "game", "any game" );
	info.sourceSize = ReadJsonValue( rootObj, "size", "any size" );
	info.align = ReadJsonValue( rootObj, "align" );

	info.numPlayers = ReadJsonValue( rootObj, "numPlayers", "2" ).toInt();
	if( info.numPlayers < 1 )
		info.numPlayers = 1;

	info.styles.clear();

	const QJsonValue styles = rootObj.value( "styles" );
	if( styles.isArray() )
	{
		QJsonArray stylesArray = styles.toArray();
		for( const QJsonValueRef val : stylesArray )
		{
			QJsonObject obj = val.toObject();

			ThemeInfo::Style tis;
			tis.name = ReadJsonValue( obj, "name", "" );

			int index = 0;
			const QJsonValue colorValue = obj.value( "colors" );
			if( colorValue.isArray() )
			{
				QJsonArray colorArray = colorValue.toArray();
				for( auto c : colorArray )
				{
					tis.colors.emplace_back( c.toString() );
					++index;
				}
			}

			for( ; index < info.numPlayers; ++index )
			{
				tis.colors.emplace_back( GetDefaultColor(index) );
			}

			info.styles.emplace_back( tis );
		}
	}
	else
	{
		ThemeInfo::Style tis;		
		for( int index = 0; index < info.numPlayers; ++index )
		{
			tis.colors.emplace_back( GetDefaultColor( index ) );
		}
		info.styles.emplace_back( tis );
	}

	info.numStyles = static_cast<int>(info.styles.size());
	if( info.numStyles < 1 )
		info.numStyles = 1;
}