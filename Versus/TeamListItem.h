#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TeamListItemClass; };
QT_END_NAMESPACE

class TeamItem;
class Dlg_EditTeams;

class TeamListItem : public QWidget
{
	Q_OBJECT

public:
	TeamListItem( Dlg_EditTeams* parent = nullptr );
	~TeamListItem() override;

public:
	void SetItem( TeamItem* item )
	{
		_item = item;
	}

	void TurnOff();
	void TurnOn();

	void UpdateListItem();

public slots:
	void OnEditTeamlogo();

protected:
	void SetTeamLogo( const QString& logoPath );

private:
	Ui::TeamListItemClass *ui;
	Dlg_EditTeams* _parent;

	TeamItem* _item;
	int _lastClickTime;
};
