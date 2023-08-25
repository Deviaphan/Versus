// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Dlg_EditTeams.h"
#include <QCollator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "PlayerManager.h"
#include "SharedTools.h"
#include "TeamListItem.h"
#include "ui_Dlg_EditTeams.h"
#include "Themes/LogoManager.h"

Dlg_EditTeams::Dlg_EditTeams( QWidget* parent, QWidget* versus )
	: QDialog( parent )
	, _versus( versus )
	, ui( new Ui::Dlg_EditTeamsClass() )
	, _selectedItem( nullptr )
{
	setStyleSheet( _versus->styleSheet() );

	ui->setupUi( this );

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	{
		connect( ui->logoTeam, &QPushButton::clicked, this, &Dlg_EditTeams::OnEditTeamlogo );
		connect( ui->btnAddTeam, &QPushButton::clicked, this, &Dlg_EditTeams::OnAddNewTeam );

		SetTeamLogo( "" );
	}

	QPixmap bkgnd( ":/Versus/bg_list.png" );
	bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );

	UpdateList();
}

Dlg_EditTeams::~Dlg_EditTeams()
{
	delete ui;
}

void Dlg_EditTeams::mousePressEvent( QMouseEvent* event )
{
	if( event->buttons().testFlag( Qt::LeftButton ) )
	{
		HWND hWnd = ::GetAncestor( (HWND)(window()->windowHandle()->winId()), GA_ROOT );
		POINT pt;
		::GetCursorPos( &pt );
		::ReleaseCapture();
		::SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS( pt ) );
	}
}

void Dlg_EditTeams::UpdateList()
{
	QSignalBlocker blocker( ui->listItems );

	ui->listItems->clear();

	QCollator collator;
	collator.setCaseSensitivity( Qt::CaseInsensitive );
	collator.setNumericMode( true );

	::std::sort( PlayerManager::TeamDB().begin(), PlayerManager::TeamDB().end(), [&collator]( const TeamItem& lhs, const TeamItem& rhs ) -> bool
	{
		return collator.compare( lhs.team, rhs.team ) < 0;
	} );

	for( auto& teamData : PlayerManager::TeamDB() )
	{
		auto item = new TeamListItem( this );
		item->SetItem( &teamData );
		item->UpdateListItem();

		const auto wItem = new QListWidgetItem;
		wItem->setSizeHint( item->minimumSize() );

		ui->listItems->addItem( wItem );
		ui->listItems->setItemWidget( wItem, item );
	}
}

void Dlg_EditTeams::SetCurrentItem( TeamListItem* pli, const TeamItem* item )
{
	if( _selectedItem )
		_selectedItem->TurnOff();

	_selectedItem = nullptr;

	if( pli && item )
	{
		_selectedItem = pli;
		_selectedItemData = *item;

		_selectedItem->TurnOn();
	}
}

void Dlg_EditTeams::OnDeleteItem( const TeamItem* item )
{
	auto& db = PlayerManager::TeamDB();
	size_t idx = 0;
	for( ; idx < db.size(); ++idx )
	{
		if( db[idx].team == item->team )
		{
			db.erase( db.begin() + idx );
			break;
		}
	}

	UpdateList();

	ui->listItems->setCurrentRow( ((int)idx) - 1, QItemSelectionModel::SelectionFlag::Select );
}

void Dlg_EditTeams::OnEditTeamlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _versus, false );
	if( !logoPath.isEmpty() )
	{
		SetTeamLogo( logoPath );
	}
}

void Dlg_EditTeams::SetTeamLogo( const QString& logoPath )
{
	ui->logoTeam->setIconSize( QSize( 60, 60 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_teamLogo = GetLogoPath( logoPath );
}

void Dlg_EditTeams::OnAddNewTeam()
{
	TeamItem item;
	item.team = ui->teamName->text();
	item.teamLogo = _teamLogo;

	if( item.team.isEmpty() )
		return;

	PlayerManager::Instance().AddTeam( item );

	UpdateList();

	const auto& db = PlayerManager::TeamDB();
	size_t idx = 0;
	for( ; idx < db.size(); ++idx )
	{
		if( db[idx].team == item.team )
		{
			break;
		}
	}

	ui->listItems->setCurrentRow( ((int)idx), QItemSelectionModel::SelectionFlag::Select );
}
