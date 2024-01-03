// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "SharedTools.h"
#include <algorithm>
#include <QComboBox>
#include <QFile>
#include <QFileInfo>
#include <QtWidgets>
#include "Themes/Dlg_SelectLogo.h"

const QString g_defaultLogo( "versus_logo/.default/grey.jpg" );

QString GetSettingsFile()
{
	return QCoreApplication::applicationDirPath() + "/settings.json";
}

QString GetLanguageFile()
{
	return QCoreApplication::applicationDirPath() + "/lang/langs.json";
}

void ReadStringArray( const QString& fileName, std::set<QString>& items )
{
	QFile inFile( fileName );
	inFile.open( QIODevice::ReadOnly | QIODevice::Text );
	QByteArray data = inFile.readAll();
	inFile.close();

	QJsonParseError errorPtr;
	QJsonDocument doc = QJsonDocument::fromJson( data, &errorPtr );
	if( doc.isNull() )
	{
		return;
	}

	QJsonObject rootObj = doc.object();
	QJsonArray ptsArray = rootObj.value( "items" ).toArray();
	for( const QJsonValueRef val : ptsArray )
	{
		if( val.isString() )
		{
			items.emplace( val.toString() );
		}
	}
}

void WriteStringArray( const QString& fileName, const std::set<QString>& items )
{
	if( items.empty() )
	{
		if( QFile::exists( fileName ) )
		{
			QFile::remove( fileName );
		}

		return;
	}

	QJsonObject rootObj;

	QJsonArray itemArray;

	for( const auto& item : items )
	{
		itemArray.append( item );
	}

	rootObj.insert( "items", itemArray );

	QFile outFile( fileName );
	outFile.open( QIODevice::WriteOnly | QIODevice::Text );

	QJsonDocument saveDoc( rootObj );
	outFile.write( saveDoc.toJson() );
}

void Add( QJsonArray& obj, const QString& key, const QString& value )
{
	QJsonObject v;
	v.insert( "id", key );
	v.insert( "v", value );
	obj.push_back( v );
}

QString ReadJsonValue( const QJsonObject& rootObj, const char* key, const QString& defaultValue )
{
	const QJsonValue value = rootObj.value( key );
	if( value.isString() )
		return value.toString();

	return defaultValue;
}

QString GetDefaultColor( int index )
{
	static const QString defColor[4] = {"#a6d4ff", "#ffacad", "#53ea92", "#ffff7f"};

	if( index < 4 )
		return defColor[index];

	return "#ffffff";
}

QString GetLogoPath( const QString& logoPath )
{
	static const QString baseDir = QCoreApplication::applicationDirPath();

	const QString logoFile = baseDir + "/" + logoPath;

	const QFileInfo fileInfo( logoFile );
	if( !fileInfo.exists() || !fileInfo.isFile() )
	{
		return g_defaultLogo;
	}

	return logoPath;
}

QString GetLogoFullPath( const QString& logoPath )
{
	static const QString baseDir = QCoreApplication::applicationDirPath();

	return baseDir + "/" + GetLogoPath( logoPath );
}

int RgbToGray( const QColor& color )
{
	return (color.red() * 11 + color.green() * 16 + color.blue() * 5) / 32;
}


void ChangeBgColor( QWidget* widget, const QString& color )
{
	QPalette palette = widget->palette();
	const QColor color1( color );
	QColor color2;
	if( RgbToGray( color1 ) > 100 )
		color2.setRgb( 0, 0, 0 );
	else
		color2.setRgb( 255, 255, 255 );

	palette.setColor( QPalette::Base, color1 );
	palette.setColor( QPalette::Text, color2 );

	widget->setPalette( palette );
}

void LoadLanguages( ::std::set<LangData>& langs )
{
	QFile inFile( GetLanguageFile() );
	inFile.open( QIODevice::ReadOnly | QIODevice::Text );
	const QByteArray byteData = inFile.readAll();
	inFile.close();

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( byteData, &errorPtr );
	if( doc.isNull() )
		return;

	QJsonObject rootObj = doc.object();

	if( !rootObj["langs"].isArray() )
		return;

	QJsonArray itemArray = rootObj["langs"].toArray();

	for( const QJsonValueRef val : itemArray )
	{
		if( val.isObject() )
		{
			auto obj = val.toObject();

			LangData data;
			data.langTitle = obj["title"].toString();
			data.langFile = obj["qm"].toString();

			const QString pngFile = QCoreApplication::applicationDirPath() + "/lang/" + obj["png"].toString();
			data.icon = QIcon( pngFile );

			langs.insert( data );
		}
	}
}

void SaveTournamentList( QJsonObject& root, QComboBox* itemList )
{
	try
	{
		QJsonArray itemArray;

		const int count = itemList->count();
		for( int idx = 0; idx < count; ++idx )
		{
			const QString text = itemList->itemText( idx );
			itemArray.append( text );
		}

		root.insert( "tournaments", itemArray );
	}
	catch( ... )
	{
	}
}

void LoadTournamentList( QComboBox* itemList )
{
	try
	{
		QSignalBlocker blocker( itemList );

		QFile inFile( GetSettingsFile() );
		inFile.open( QIODevice::ReadOnly | QIODevice::Text );
		const QByteArray byteData = inFile.readAll();
		inFile.close();

		QJsonParseError errorPtr;
		const QJsonDocument doc = QJsonDocument::fromJson( byteData, &errorPtr );
		if( !doc.isNull() )
		{
			QJsonObject rootObj = doc.object();

			QJsonArray itemArray;
			if( rootObj["tournaments"].isArray() )
			{
				itemArray = rootObj["tournaments"].toArray();
			}

			if( !itemArray.empty() )
			{
				for( const QJsonValueRef val : itemArray )
				{
					if( val.isString() )
					{
						itemList->addItem( val.toString() );
					}
				}
			}
		}

		if( itemList->count() == 0 )
		{
			itemList->addItem( "Versus Ⓡevolution" );
			itemList->setCurrentText( "Versus Ⓡevolution" );
			itemList->setCurrentIndex( 0 );
		}
	}
	catch( ... )
	{
	}
}

void SaveListSubtitle1( QComboBox* itemList )
{
	try
	{
		std::set<QString> items;
		const QString groupPath = QCoreApplication::applicationDirPath() + "/subtitle1.json";
		ReadStringArray( groupPath, items );

		const QString groupPathUser = QCoreApplication::applicationDirPath() + "/subtitle1-user.json";

		std::set<QString> userItems;
		const int count = itemList->count();
		for( int idx = 0; idx < count; ++idx )
		{
			const QString text = itemList->itemText( idx );
			if( items.find( text ) == items.end() )
			{
				userItems.insert( text );
			}
		}

		WriteStringArray( groupPathUser, userItems );
	}
	catch( ... )
	{
	}
}

void LoadListSubtitle1( QComboBox* itemList )
{
	try
	{
		QSignalBlocker blocker( itemList );

		std::set<QString> items;
		const QString groupPath = QCoreApplication::applicationDirPath() + "/subtitle1.json";
		const QString groupPathUser = QCoreApplication::applicationDirPath() + "/subtitle1-user.json";
		ReadStringArray( groupPath, items );
		ReadStringArray( groupPathUser, items );
		if( !items.empty() )
		{
			for( const QString& item : items )
			{
				itemList->addItem( item );
			}

			itemList->setCurrentIndex( 0 );
		}
	}
	catch( ... )
	{
	}
}

void SaveListSubtitle2( QComboBox* itemList )
{
	try
	{
		std::set<QString> items;
		const QString boPath = QCoreApplication::applicationDirPath() + "/subtitle2.json";
		ReadStringArray( boPath, items );

		const QString boPathUser = QCoreApplication::applicationDirPath() + "/subtitle2-user.json";

		std::set<QString> userItems;
		const int count = itemList->count();
		for( int idx = 0; idx < count; ++idx )
		{
			const QString text = itemList->itemText( idx );
			if( items.find( text ) == items.end() )
			{
				userItems.insert( text );
			}
		}

		WriteStringArray( boPathUser, userItems );
	}
	catch( ... )
	{
	}
}



void LoadListSubtitle2( QComboBox* itemList )
{
	try
	{
		QSignalBlocker blocker( itemList );

		std::set<QString> items;
		const QString boPath = QCoreApplication::applicationDirPath() + "/subtitle2.json";
		const QString boPathUser = QCoreApplication::applicationDirPath() + "/subtitle2-user.json";
		ReadStringArray( boPath, items );
		ReadStringArray( boPathUser, items );
		if( !items.empty() )
		{
			for( const QString& item : items )
			{
				itemList->addItem( item );
			}

			itemList->setCurrentIndex( 0 );
		}
	}
	catch( ... )
	{
	}
}
