// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Dlg_EditList.h"
#include <algorithm>
#include <QCollator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "ui_Dlg_EditList.h"


Dlg_EditList::Dlg_EditList( QWidget* parent )
	: QDialog( parent )
	, ui( new Ui::Dlg_EditListClass() )
{
	ui->setupUi( this );

	connect( ui->btnAddNew, &QPushButton::clicked, this, &Dlg_EditList::OnAddLine );
	connect( ui->btnRemove, &QPushButton::clicked, this, &Dlg_EditList::OnRemoveLine );
	connect( ui->btnClose, &QPushButton::clicked, this, &QDialog::reject );

	QPixmap bkgnd( ":/Versus/bg_list.png" );
	bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_EditList::~Dlg_EditList()
{
	delete ui;
}

void Dlg_EditList::mousePressEvent( QMouseEvent* event )
{
	if( event->buttons().testFlag( Qt::LeftButton ) )
	{
		HWND hWnd = ::GetAncestor( (HWND)(window()->windowHandle()->winId()), GA_ROOT );
		POINT pt;
		::GetCursorPos( &pt );
		::ReleaseCapture();
		::SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS( pt ) );
	}

	QDialog::mousePressEvent( event );
}

void Dlg_EditList::UpdateList()
{
	QSignalBlocker blocker( ui->listItems );

	ui->listItems->clear();

	for( auto& s : items )
	{
		ui->listItems->addItem( s );
	}
}

void Dlg_EditList::OnAddLine()
{
	const QString line = ui->lineEdit->text();
	if( line.isEmpty() )
		return;

	items.emplace_back( line );

	QCollator collator;
	collator.setCaseSensitivity( Qt::CaseInsensitive );
	collator.setNumericMode( true );

	std::sort( items.begin(), items.end(), collator  );

	UpdateList();
}

void Dlg_EditList::OnRemoveLine()
{
	const int index = ui->listItems->currentRow();
	if( index < 0 )
		return;

	QListWidgetItem* item = ui->listItems->takeItem( index );
	if( item )
	{
		delete item;

		items.erase( items.begin() + index );
	}
}
