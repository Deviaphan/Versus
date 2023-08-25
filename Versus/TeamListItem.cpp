// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "TeamListItem.h"
#include <QTime>
#include "Dlg_EditTeams.h"
#include "PlayerItem.h"
#include "SharedTools.h"
#include "ui_TeamListItem.h"
#include "Themes/LogoManager.h"

TeamListItem::TeamListItem( Dlg_EditTeams* parent )
	: QWidget( parent )
	, ui( new Ui::TeamListItemClass() )
	, _parent( parent )
	, _item( nullptr )
	, _lastClickTime( 0 )
{
	ui->setupUi(this);

	connect( ui->logoTeam, &QPushButton::clicked, this, &TeamListItem::OnEditTeamlogo );

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

	connect( ui->teamName, &QLineEdit::textChanged, this, [this]( const QString& s )
	{
		this->_item->team = s;
	}
	);

	connect( ui->btnDelete, &QPushButton::clicked, this, [this]{ emit _parent->OnDeleteItem( _item ); } );
}

TeamListItem::~TeamListItem()
{
	delete ui;
}

void TeamListItem::UpdateListItem()
{
	ui->teamName->setText( _item->team );

	ui->logoTeam->setIconSize( QSize( 60, 60 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( _item->teamLogo ) ) ) );

	ui->pushSelect->setIconSize( QSize( 40, 40 ) );
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );
}

void TeamListItem::TurnOff()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );
}

void TeamListItem::TurnOn()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_on_40.png" ) ) );
}

void TeamListItem::OnEditTeamlogo()
{
	const QString logoPath = LogoManager::Instance().FindLogo( _parent->_versus, false );
	if( !logoPath.isEmpty() )
	{
		SetTeamLogo( logoPath );
	}
}

void TeamListItem::SetTeamLogo( const QString& logoPath )
{
	ui->logoTeam->setIconSize( QSize( 30, 30 ) );
	ui->logoTeam->setIcon( QIcon( QPixmap( GetLogoFullPath( logoPath ) ) ) );

	_item->teamLogo = GetLogoPath( logoPath );
}