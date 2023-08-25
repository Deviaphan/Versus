#pragma once
#include <set>
#include <vector>
#include "QString"
#include <QColor>
#include "PlayerItem.h"

class QWidget;
class QPushButton;
class QJsonArray;
class QComboBox;
class QJsonObject;

extern const QString g_editTitle;


void ReadStringArray( const QString& fileName, std::set<QString>& items );
void WriteStringArray( const QString& fileName, const std::set<QString>& items );

void Add( QJsonArray& obj, const QString& key, const QString& value );

QString ReadJsonValue( const QJsonObject& rootObj, const char* key, const QString& defaultValue = QString() );

QString GetDefaultColor( int index );

void SaveTournamentList( QJsonObject & root, QComboBox * itemList );
void LoadTournamentList( QJsonObject * root, QComboBox* itemList );

void SaveGroupList( QComboBox * itemList );
void LoadGroupList( QComboBox* itemList );

void SaveBestOfList( QComboBox * itemList );
void LoadBestOfList( QComboBox* itemList );

QString GetLogoPath( const QString& logoPath );

QString GetLogoFullPath( const QString& logoPath );

int RgbToGray( const QColor& color );

void ChangeBgColor( QWidget* widget, const QString& color );