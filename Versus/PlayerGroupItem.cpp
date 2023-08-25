// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "PlayerGroupItem.h"

#include <QTime>
#include "Dlg_SetPlayerGroup.h"
#include "PlayerItem.h"

PlayerGroupItem::PlayerGroupItem( Dlg_SetPlayerGroup* parent )
	: QWidget(parent)
	, ui( new Ui::PlayerGroupItemClass() )
	, _parent( parent )
	, _item( nullptr )
	, _lastClickTime( 0 )
{
	ui->setupUi(this);

	ui->pushSelect->setIconSize( QSize( 40, 40 ) );
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );

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

	connect( ui->btnDelete, &QPushButton::clicked, this, [this]{ emit _parent->OnDeleteItem( _item ); } );
}

PlayerGroupItem::~PlayerGroupItem()
{
	delete ui;
}

void PlayerGroupItem::SetItem( const Group* item )
{
	_item = item;

	ui->groupName->setText( item->title );
}

void PlayerGroupItem::TurnOff()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_off_40.png" ) ) );
}

void PlayerGroupItem::TurnOn()
{
	ui->pushSelect->setIcon( QIcon( QPixmap( ":/Versus/radio_on_40.png" ) ) );
}
