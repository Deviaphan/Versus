// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Dlg_EditPlayers.h"
#include <algorithm>
#include <QCollator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "Dlg_EditTeams.h"
#include "PlayerListItem.h"
#include "PlayerManager.h"
#include "SharedTools.h"
#include "ui_Dlg_EditPlayers.h"
#include "Versus.h"
#include "Themes/LogoManager.h"

Dlg_EditPlayers::Dlg_EditPlayers( QWidget* parent, Versus* versus )
	: QDialog(parent)
	, _selectedItem( nullptr )
	, _versus( versus )
	, _sortByName( true )
{
	ui = new Ui::Dlg_EditPlayers();

	setStyleSheet( _versus->styleSheet() );

	ui->setupUi(this);

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	{
		connect( ui->logoPlayer, &QPushButton::clicked, this, &Dlg_EditPlayers::OnEditPlayerlogo );
		connect( ui->logoTeam, &QPushButton::clicked, this, &Dlg_EditPlayers::OnEditTeamlogo );

		connect( ui->teamName, &QLineEdit::textChanged, this, [this]( const QString& s ){ this->PerhapsUpdateTeamLogo( s ); } );
		connect( ui->favTeams, &QPushButton::clicked, this, &Dlg_EditPlayers::OnEditTeam );

		connect( ui->btnAddPlayer, &QPushButton::clicked, this, &Dlg_EditPlayers::OnAddNewPlayer );

		SetPlayerLogo( "" );
		SetTeamLogo( "" );
	}

	connect( ui->btnSortName, &QPushButton::clicked, this, &Dlg_EditPlayers::OnSortByName );
	connect( ui->btnSortTeam, &QPushButton::clicked, this, &Dlg_EditPlayers::OnSortByTeam );

	QPixmap bkgnd( ":/Versus/bg_list.png" );
	bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );

	UpdateList();
}

Dlg_EditPlayers::~Dlg_EditPlayers()
{
	delete ui;
}

void Dlg_EditPlayers::mousePressEvent( QMouseEvent* event )
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

void Dlg_EditPlayers::UpdateList()
{
	QSignalBlocker blocker( ui->listItems );

	ui->listItems->clear();

	for( auto& dbItem : PlayerManager::PlayerDB() )
	{
		auto item = new PlayerListItem( this );
		item->SetItem( &dbItem );
		item->UpdateListItem();

		const auto wItem = new QListWidgetItem;
		wItem->setSizeHint( item->minimumSize() );

		ui->listItems->addItem( wItem );
		ui->listItems->setItemWidget( wItem, item );
	}
}

void Dlg_EditPlayers::OnAddNewPlayer()
{
	PlayerItem item;
	item.player = ui->playerName->text();
	item.team = ui->teamName->text();
	item.playerLogo = _playerLogo;
	item.teamLogo = _teamLogo;

	if( item.player.isEmpty() )
		return;

	PlayerManager::Instance().AddPlayer( item );
	PlayerManager::Instance().AddTeam( item );

	if( _sortByName )
		OnSortByName();
	else
		OnSortByTeam();

	const auto & db = PlayerManager::PlayerDB();
	size_t idx = 0;
	for( ; idx < db.size(); ++idx )
	{
		if( db[idx].player == item.player )
		{
			if( db[idx].team == item.team )
			{
				break;
			}
		}
	}

	ui->listItems->setCurrentRow( ((int)idx), QItemSelectionModel::SelectionFlag::Select );
}

void Dlg_EditPlayers::OnSortByName()
{
	_sortByName = true;

	QCollator collator;
	collator.setCaseSensitivity( Qt::CaseInsensitive );
	collator.setNumericMode( true );

	::std::sort( PlayerManager::PlayerDB().begin(), PlayerManager::PlayerDB().end(), [&collator]( const PlayerItem& lhs, const PlayerItem& rhs ) -> bool
	{
		return collator.compare( lhs.player, rhs.player ) < 0;
	} );

	UpdateList();
}

void Dlg_EditPlayers::OnSortByTeam()
{
	_sortByName = false;

	QCollator collator;
	collator.setCaseSensitivity( Qt::CaseInsensitive );
	collator.setNumericMode( true );

	::std::sort( PlayerManager::PlayerDB().begin(), PlayerManager::PlayerDB().end(), [&collator]( const PlayerItem& lhs, const PlayerItem& rhs ) -> bool
	{
		if( collator.compare( lhs.team, rhs.team ) < 0 )
			return true;

		if( lhs.team == rhs.team )
			return collator.compare( lhs.player, rhs.player ) < 0;

		return false;			
	} );

	UpdateList();
}

void Dlg_EditPlayers::SetCurrentItem( PlayerListItem* pli, const PlayerItem* item )
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

void Dlg_EditPlayers::OnDeleteItem( const PlayerItem * item )
{
	auto & db = PlayerManager::PlayerDB();
	size_t idx = 0;
	for( ; idx < db.size(); ++idx )
	{
		if( db[idx].player == item->player )
		{
			if( db[idx].team == item->team )
			{
				db.erase( db.begin() + idx );
				break;
			}
		}
	}

	UpdateList();

	if( idx == db.size() )
		idx -= 1;

	ui->listItems->setCurrentRow( ((int)idx), QItemSelectionModel::SelectionFlag::Select );
}

void Dlg_EditPlayers::SetPlayerLogo( const QString& logoPath )
{
	ui->logoPlayer->setIconSize( QSize( 60, 60 ) );
	ui->logoPlayer->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_playerLogo = GetLogoPath( logoPath );
}

void Dlg_EditPlayers::SetTeamLogo( const QString& logoPath )
{
	ui->logoTeam->setIconSize( QSize( 30, 30 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_teamLogo = GetLogoPath( logoPath );
}

void Dlg_EditPlayers::OnEditPlayerlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _versus, true );
	if( !logoPath.isEmpty() )
	{
		SetPlayerLogo( logoPath );
	}
}

void Dlg_EditPlayers::OnEditTeamlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _versus, false );
	if( !logoPath.isEmpty() )
	{
		SetTeamLogo( logoPath );
	}
}

void Dlg_EditPlayers::PerhapsUpdateTeamLogo( const QString& teamName )
{
	const auto& db = PlayerManager::TeamDB();
	for( const auto& item : db )
	{
		if( item.team == teamName )
		{
			SetTeamLogo( item.teamLogo );

			return;
		}
	}
}

void Dlg_EditPlayers::OnEditTeam()
{
	_versus->UpdatePlayerDB();

	Dlg_EditTeams dlg( this, _versus );
	if( dlg.exec() == QDialog::Rejected )
	{
		const QString teamName = ui->teamName->text();

		bool clearTeam = true;

		const auto& db = PlayerManager::TeamDB();
		for( auto& item : db )
		{
			if( item.team == teamName )
			{
				clearTeam = false;
				break;
			}
		}

		if( clearTeam )
		{
			ui->teamName->clear();
			SetTeamLogo( "" );
		}
	}
	else
	{
		ui->teamName->setText( dlg._selectedItemData.team );
		SetTeamLogo( dlg._selectedItemData.teamLogo );
	}
}