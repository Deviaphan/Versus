// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Dlg_SetPlayerGroup.h"
#include <algorithm>
#include <QCollator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "PlayerGroupItem.h"
#include "PlayerManager.h"
#include "ui_Dlg_SetPlayerGroup.h"

Dlg_SetPlayerGroup::Dlg_SetPlayerGroup( QWidget* parent )
	: QDialog( parent )
	, ui( new Ui::Dlg_SetPlayerGroupClass() )
	, _currentPlayerSet( nullptr )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi( this );

	connect( ui->btnOK, &QPushButton::clicked, this, &Dlg_SetPlayerGroup::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	connect( ui->btnSaveGroup, &QPushButton::clicked, this, &Dlg_SetPlayerGroup::OnSaveGroup );
	
	QPixmap bkgnd( ":/Versus/bg_list.png" );
	bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_SetPlayerGroup::~Dlg_SetPlayerGroup()
{
	delete ui;
}

void Dlg_SetPlayerGroup::UpdateList()
{
	assert( _currentPlayerSet != nullptr );

	if( !_currentPlayerSet )
		return;

	QSignalBlocker blocker( ui->listItems );

	ui->listItems->clear();

	auto& pg = PlayerManager::PlayerGroupDB();

	QCollator collator;
	collator.setCaseSensitivity( Qt::CaseInsensitive );
	collator.setNumericMode( true );

	::std::sort( pg.begin(), pg.end(), [&collator]( const Group& lhs, const Group& rhs ) -> bool
	{
		return collator.compare( lhs.title, rhs.title ) < 0;
	} );

	for( auto & item : pg )
	{
		if( item.items.size() != _currentPlayerSet->size() )
			continue;

		auto pgi = new PlayerGroupItem( this );
		pgi->SetItem( &item );

		const auto wItem = new QListWidgetItem;
		wItem->setSizeHint( pgi->minimumSize() );

		ui->listItems->addItem( wItem );
		ui->listItems->setItemWidget( wItem, pgi );
	}
}

void Dlg_SetPlayerGroup::Init( const std::vector<PlayerItemData> & playerSet )
{
	_currentPlayerSet = &playerSet;

	std::vector<QString> names;
	for( const auto& p : *_currentPlayerSet )
	{
		if( !p.player.isEmpty() )
			names.emplace_back( p.player );
	}
	std::sort( names.begin(), names.end() );

	QString title;
	const size_t size = names.size();
	for( size_t index = 0; index < size; ++index )
	{
		title += names[index];
		if( index != (size - 1) )
		{
			title += "●";
		}
	}

	if( title.size() > 50 )
	{
		title.clear();

		for( size_t index = 0; index < size; ++index )
		{
			title += names[index].left( 3 );
			if( index != (size - 1) )
			{
				title += "●";
			}
		}
	}

	ui->groupTitle->setText( title );

	UpdateList();
}

void Dlg_SetPlayerGroup::accept()
{
	const QString title = ui->groupTitle->text();
	if( !title.isEmpty() )
	{
		auto& pg = PlayerManager::PlayerGroupDB();

		for( auto ii = pg.begin(); ii != pg.end(); ++ii )
		{
			if( ii->title == title && ii->items.size() == _currentPlayerSet->size() )
			{
				emit OnSaveGroup();
				break;
			}
		}
	}

	QDialog::accept();
}

void Dlg_SetPlayerGroup::OnSaveGroup()
{
	assert( _currentPlayerSet != nullptr );
	if( !_currentPlayerSet )
		return;

	const QString title = ui->groupTitle->text();
	if( title.isEmpty() )
		return;

	auto& pg = PlayerManager::PlayerGroupDB();

	for( auto ii = pg.begin(); ii != pg.end(); ++ii )
	{
		if( ii->title == title && ii->items.size() == _currentPlayerSet->size() )
		{
			pg.erase( ii );
			break;
		}
	}

	auto& group = pg.emplace_back();
	group.title = title;

	for( auto& item : *_currentPlayerSet )
	{
		auto& i = group.items.emplace_back();

		i.player = item.player;
		i.team = item.team;
		i.score = item.score;
	}

	ui->groupTitle->clear();

	UpdateList();
}

void Dlg_SetPlayerGroup::OnDeleteItem( const Group* item )
{
	auto& db = PlayerManager::PlayerGroupDB();
	size_t idx = 0;
	for( ; idx < db.size(); ++idx )
	{
		if( db[idx].title == item->title )
		{
			db.erase( db.begin() + idx );
			break;
		}
	}

	UpdateList();

	if( idx == db.size() )
		idx -= 1;

	ui->listItems->setCurrentRow( ((int)idx), QItemSelectionModel::SelectionFlag::Select );
}

void Dlg_SetPlayerGroup::SetCurrentItem( PlayerGroupItem* pli, const Group* item )
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

void Dlg_SetPlayerGroup::mousePressEvent( QMouseEvent* event )
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