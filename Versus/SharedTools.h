#pragma once
#include <set>
#include <set>
#include <QString>
#include <QIcon>
#include <QColor>
#include "PlayerItem.h"

class QWidget;
class QPushButton;
class QJsonArray;
class QComboBox;
class QJsonObject;

extern const QString g_editTitle;

QString GetSettingsFile();
QString GetLanguageFile();

void ReadStringArray( const QString& fileName, std::set<QString>& items );
void WriteStringArray( const QString& fileName, const std::set<QString>& items );

void Add( QJsonArray& obj, const QString& key, const QString& value );

QString ReadJsonValue( const QJsonObject& rootObj, const char* key, const QString& defaultValue = QString() );

QString GetDefaultColor( int index );

struct LangData
{
	QString langTitle;
	QString langFile;
	QIcon icon;

	LangData() = default;
	explicit LangData( const QString& title )
		: langTitle( title )
	{
	}

	bool operator< ( const LangData& data ) const
	{
		return langTitle < data.langTitle;
	}
};
void LoadLanguages( ::std::set<LangData> & langs );

void SaveTournamentList( QJsonObject & root, QComboBox * itemList );
void LoadTournamentList( QComboBox* itemList );

void SaveListSubtitle1( QComboBox * itemList );
void LoadListSubtitle1( QComboBox* itemList );

void SaveListSubtitle2( QComboBox * itemList );
void LoadListSubtitle2( QComboBox* itemList );

QString GetLogoPath( const QString& logoPath );

QString GetLogoFullPath( const QString& logoPath );

int RgbToGray( const QColor& color );

void ChangeBgColor( QWidget* widget, const QString& color );