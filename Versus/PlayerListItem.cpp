// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "PlayerListItem.h"
#include <QTime>
#include "Dlg_EditPlayers.h"
#include "Dlg_EditTeams.h"
#include "PlayerManager.h"
#include "SharedTools.h"
#include "ui_PlayerListItem.h"
#include "Themes/LogoManager.h"
#include "Versus.h"

PlayerListItem::PlayerListItem( Dlg_EditPlayers* parent )
	: QWidget( parent )
	, ui( new Ui::PlayerListItemClass() )
	, _parent( parent )
	, _item( nullptr )
	, _lastClickTime( 0 )
{
	ui->setupUi( this );

	connect( ui->logoPlayer, &QPushButton::clicked, this, &PlayerListItem::OnEditPlayerlogo );
	connect( ui->logoTeam, &QPushButton::clicked, this, &PlayerListItem::OnEditTeamlogo );

	connect( ui->pushSelect, &QPushButton::clicked, this, [this]()
	{
		const QTime time = QTime::currentTime();
		const int curTime = time.msecsSinceStartOfDay();

		if( (curTime - _lastClickTime) < 500 )
		{
			emit _parent->accept();
		}
		else
		{
			_lastClickTime = curTime;
			_parent->SetCurrentItem( this, _item );
		}
	} );

	connect( ui->editName, &QLineEdit::textChanged, this, [this]( const QString& s )
	{
		this->_item->player = s;
	}
	);

	connect( ui->editTeam, &QLineEdit::textChanged, this, [this]( const QString& s )
	{
		this->_item->team = s;
		this->PerhapsUpdateTeamLogo( s );
	}
	);

	connect( ui->favTeams, &QPushButton::clicked, this, &PlayerListItem::OnEditTeam );

	connect( ui->btnDelete, &QPushButton::clicked, this, [this]{ emit _parent->OnDeleteItem( _item ); } );
}

PlayerListItem::~PlayerListItem()
{
	delete ui;
}

void PlayerListItem::UpdateListItem()
{
	ui->editName->setText( _item->player );
	ui->editTeam->setText( _item->team );

	ui->logoPlayer->setIconSize( QSize( 60, 60 ) );
	ui->logoPlayer->setIcon( QIcon( QPixmap( GetLogoFullPath( _item->playerLogo ) ) ) );


	ui->logoTeam->setIconSize( QSize( 30, 30 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( _item->teamLogo ) ) ) );

	ui->pushSelect->setIconSize( QSize( 40, 40 ) );
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );
}

void PlayerListItem::TurnOff()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );
}

void PlayerListItem::TurnOn()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_on_40.png" ) ) );
}

void PlayerListItem::OnEditPlayerlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _parent->_versus, true );
	if( !logoPath.isEmpty() )
	{
		SetPlayerLogo( logoPath );
	}
}

void PlayerListItem::OnEditTeamlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _parent->_versus, false );
	if( !logoPath.isEmpty() )
	{
		SetTeamLogo( logoPath );
	}
}

void PlayerListItem::SetPlayerLogo( const QString& logoPath )
{
	ui->logoPlayer->setIconSize( QSize( 60, 60 ) );
	ui->logoPlayer->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_item->playerLogo = GetLogoPath( logoPath );
}

void PlayerListItem::SetTeamLogo( const QString& logoPath )
{
	ui->logoTeam->setIconSize( QSize( 30, 30 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_item->teamLogo = GetLogoPath( logoPath );
}

void PlayerListItem::OnEditTeam()
{
	Dlg_EditTeams dlg( _parent, _parent->_versus );
	if( dlg.exec() == QDialog::Rejected )
	{
		bool clearTeam = true;

		const auto& db = PlayerManager::TeamDB();
		for( auto& item : db )
		{
			if( item.team == _item->team )
			{
				clearTeam = false;
				break;
			}
		}

		if( clearTeam )
		{
			_item->team.clear();
			_item->teamLogo.clear();
		}
	}
	else
	{
		_item->team = dlg._selectedItemData.team;
		_item->teamLogo = dlg._selectedItemData.teamLogo;
	}

	ui->editTeam->setText( _item->team );

	SetTeamLogo( _item->teamLogo );
}

void PlayerListItem::PerhapsUpdateTeamLogo( const QString& teamName )
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
