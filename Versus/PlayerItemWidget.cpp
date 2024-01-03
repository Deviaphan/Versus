// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "PlayerItemWidget.h"

#include <QFileInfo>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "Dlg_EditPlayers.h"
#include "Dlg_EditTeams.h"
#include "PlayerItemData.h"
#include "PlayerManager.h"
#include "QLineEdit"
#include "ui_PlayerItemWidget.h"
#include "Versus.h"
#include "Themes/LogoManager.h"

PlayerItemWidget::PlayerItemWidget( Versus* parent )
	: QWidget( parent, Qt::WindowFlags() )
	, ui( new Ui::PlayerItemWidgetClass() )
	, _itemData( nullptr )
	, _versus( parent )
{
	ui->setupUi( this );

	connect( ui->editName, &QLineEdit::textChanged, this, [this]( const QString& s ){ this->PerhapsUpdatePlayerLogo( s ); } );
	connect( ui->editTeam, &QLineEdit::textChanged, this, [this]( const QString& s ){ this->PerhapsUpdateTeamLogo( s ); } );

	connect( ui->logoPlayer, &QPushButton::clicked, this, &PlayerItemWidget::OnEditPlayerlogo );
	connect( ui->logoTeam, &QPushButton::clicked, this, &PlayerItemWidget::OnEditTeamlogo );

	connect( ui->editScore, &QLineEdit::textChanged, this, [this]( const QString& s )
	{
		const int length = (int)s.length();
		const int textHeight = (std::max)(40 - length * 5, 9);
		QFont font = this->ui->editScore->font();
		font.setPointSize( textHeight );
		this->ui->editScore->setFont( font );
	}
	);

	connect( ui->decScore, &QPushButton::clicked, this, [this]{	this->Dec();	} );
	connect( ui->incScore, &QPushButton::clicked, this, [this]{	this->Inc();	} );

	connect( ui->favPlayers, &QPushButton::clicked, this, &PlayerItemWidget::OnEditPlayer );
	connect( ui->favTeams, &QPushButton::clicked, this, &PlayerItemWidget::OnEditTeam );

}

PlayerItemWidget::~PlayerItemWidget()
{
	delete ui;
}

void PlayerItemWidget::SetTitle( const QString & title )
{
	ui->labelPlayerIndex->setText( title );
}


void PlayerItemWidget::SetItemData( PlayerItemData* itemData )
{
	_itemData = itemData;

	WriteItem();
}

void PlayerItemWidget::WriteItem()
{
	ui->editName->setText( _itemData->player );
	ui->editTeam->setText( _itemData->team );
	ui->editScore->setText( _itemData->score );

	SetPlayerLogo( _itemData->playerLogo );
	SetTeamLogo( _itemData->teamLogo );

	ChangeBgColor( ui->editScore, _itemData->scoreColor );
}

void PlayerItemWidget::ReadItem()
{
	_itemData->player = ui->editName->text();
	_itemData->team  = ui->editTeam->text();
	_itemData->score = ui->editScore->text();
}

void PlayerItemWidget::Inc()
{
	EditScores( _versus->GetIncStep() );
}

void PlayerItemWidget::Dec()
{
	EditScores( _versus->GetDecStep() );
}

void PlayerItemWidget::EditScores( double step )
{
	const double pScore = ui->editScore->text().toDouble() + step;
	ui->editScore->setText( QString::number( pScore ) );
	emit _versus->OnSendScoreboardData();
}

void PlayerItemWidget::SetPlayerLogo( const QString& logoPath )
{
	ui->logoPlayer->setIconSize( QSize( 60, 60 ) );
	ui->logoPlayer->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_itemData->playerLogo = GetLogoPath( logoPath );
}

void PlayerItemWidget::SetTeamLogo( const QString& logoPath )
{
	ui->logoTeam->setIconSize( QSize( 30, 30 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_itemData->teamLogo = GetLogoPath( logoPath );
}

void PlayerItemWidget::OnEditPlayerlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _versus, true );
	if( !logoPath.isEmpty() )
	{
		SetPlayerLogo( logoPath );

		emit _versus->OnSendScoreboardData();
	}
}

void PlayerItemWidget::OnEditTeamlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _versus, false );
	if( !logoPath.isEmpty() )
	{
		SetTeamLogo( logoPath );

		emit _versus->OnSendScoreboardData();
	}
}

void PlayerItemWidget::OnEditPlayer()
{
	_versus->UpdatePlayerDB();

	Dlg_EditPlayers dlg( this, _versus );
	if( dlg.exec() == QDialog::Rejected )
	{
		bool clearPlayer = true;
		const auto & db = PlayerManager::PlayerDB();
		for( auto& item : db )
		{
			if( item.player == _itemData->player )
			{
				clearPlayer = false;
				break;
			}
		}

		if( clearPlayer )
		{
			_itemData->player.clear();
			_itemData->playerLogo .clear();
			_itemData->team.clear();
			_itemData->teamLogo.clear();
		}
	}
	else
	{
		_itemData->player = dlg._selectedItemData.player;
		_itemData->playerLogo = dlg._selectedItemData.playerLogo;
		_itemData->team = dlg._selectedItemData.team;
		_itemData->teamLogo = dlg._selectedItemData.teamLogo;
	}

	WriteItem();

	PlayerManager::Instance().AddPlayer( *_itemData );
	PlayerManager::Instance().AddTeam( *_itemData );

}

void PlayerItemWidget::OnEditTeam()
{
	_versus->UpdatePlayerDB();

	Dlg_EditTeams dlg( this, _versus );
	if( dlg.exec() == QDialog::Rejected )
	{
		bool clearTeam = true;

		const auto& db = PlayerManager::TeamDB();
		for( auto& item : db )
		{
			if( item.team == _itemData->team )
			{
				clearTeam = false;
				break;
			}
		}

		if( clearTeam )
		{
			_itemData->team.clear();
			_itemData->teamLogo.clear();
		}
	}
	else
	{
		_itemData->team = dlg._selectedItemData.team;
		_itemData->teamLogo = dlg._selectedItemData.teamLogo;
	}

	WriteItem();

}

void PlayerItemWidget::PerhapsUpdatePlayerLogo( const QString& playerName )
{
	const auto& db = PlayerManager::PlayerDB();
	for( const auto& item : db )
	{
		if( item.player == playerName )
		{
			_itemData->player = item.player;
			_itemData->team = item.team;
			_itemData->playerLogo = item.playerLogo;
			_itemData->teamLogo = item.teamLogo;

			WriteItem();

			return;
		}
	}
}

void PlayerItemWidget::PerhapsUpdateTeamLogo( const QString& teamName )
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
