#pragma once

#include <QDialog>
#include "PlayerItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dlg_EditTeamsClass; };
QT_END_NAMESPACE

class QMouseEvent;

class TeamListItem;

class Dlg_EditTeams : public QDialog
{
	Q_OBJECT

public:
	Dlg_EditTeams(QWidget *parent, QWidget* versus );
	~Dlg_EditTeams() override;

public slots:
	void OnEditTeamlogo();
	void OnAddNewTeam();

public:
	void UpdateList();
	void OnDeleteItem( const TeamItem* item );
	void SetCurrentItem( TeamListItem* pli, const TeamItem* item );

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private:
	void SetTeamLogo( const QString& logoPath );

private:
	Ui::Dlg_EditTeamsClass *ui;
	TeamListItem* _selectedItem;

	QString _teamLogo;

public:
	QWidget* _versus;
	TeamItem _selectedItemData;
};
