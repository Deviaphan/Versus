#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PlayerItemWidgetClass; };
QT_END_NAMESPACE

class PlayerItemData;
class Versus;
class QMouseEvent;

class PlayerItemWidget : public QWidget
{
	Q_OBJECT

public:
	PlayerItemWidget( Versus* parent );
	~PlayerItemWidget() override;

public:
	void SetItemData( PlayerItemData * itemData );
	void SetTitle( const QString & title );

	void WriteItem();
	void ReadItem();
	
	void Inc();
	void Dec();
	void EditScores( double step );

protected slots:
	void OnEditPlayerlogo();
	void OnEditTeamlogo();

	void OnEditPlayer();
	void OnEditTeam();

private:
	void SetPlayerLogo( const QString & logoPath );
	void SetTeamLogo( const QString & logoPath );

	void PerhapsUpdatePlayerLogo( const QString& playerName );
	void PerhapsUpdateTeamLogo( const QString& teamName );

private:
	Ui::PlayerItemWidgetClass* ui;

	PlayerItemData* _itemData;

	Versus* _versus;
};
