// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Dlg_EditHotkey.h"
#include "ui_Dlg_EditHotkey.h"
#include <Windows.h>
#include <QWindow>
#include <QMouseEvent>

Dlg_EditHotkey::Dlg_EditHotkey( QWidget* parent )
	: QDialog( parent, Qt::WindowFlags::enum_type::WindowTitleHint )
	, ui( new Ui::Dlg_EditHotkey )
	, _timer{}
	, _numHotkeys( 0 )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi( this );

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );

	_timer[0] = ui->seqStart;
	_timer[1] = ui->seqPause;
	_timer[2] = ui->seqStop;

	connect( ui->btnClearStart, &QPushButton::clicked, this, [this]{_timer[0]->clear(); } );
	connect( ui->btnClearPause, &QPushButton::clicked, this, [this]{_timer[1]->clear(); } );
	connect( ui->btnClearStop, &QPushButton::clicked, this, [this]{_timer[2]->clear(); } );

	connect( ui->btnSetNumHotkeys, &QPushButton::clicked, this, [this]
	{
		const int num = (std::min)((std::max)(1, ui->numHotkeys->text().toInt()), 100);
		this->SetNumHotkeys( num );
	} );


	QPixmap bkgnd( ":/Versus/bg_hotkey.png" );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_EditHotkey::~Dlg_EditHotkey()
{
	delete ui;
}

void Dlg_EditHotkey::mousePressEvent( QMouseEvent* event )
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

void Dlg_EditHotkey::keyPressEvent( QKeyEvent* e )
{
	if( e->key() != Qt::Key_Escape )
	{
		QDialog::keyPressEvent( e );
	}
}

void Dlg_EditHotkey::accept()
{
	for( int i = 0; i < (int)_list.size(); ++i )
	{
		const int idInc = i + 1;
		const int idDec = -i - 1;

		_scoreHotkeys[idInc] = _list[i]->GetInc();
		_scoreHotkeys[idDec] = _list[i]->GetDec();
	}

	QDialog::accept();
}

void Dlg_EditHotkey::Set( int playerId, const QKeySequence& ks )
{
	if( playerId >= TimerHotkey_Offset )
	{
		_timer[playerId - TimerHotkey_Offset]->setKeySequence( ks );
		return;
	}

	_scoreHotkeys[playerId] = ks;
}

QKeySequence Dlg_EditHotkey::Get( int playerId ) const
{
	if( playerId >= TimerHotkey_Offset )
	{
		return _timer[playerId - TimerHotkey_Offset]->keySequence();
	}

	auto ii = _scoreHotkeys.find( playerId );
	if( ii != _scoreHotkeys.end() )
	{
		return ii->second;
	}

	return {};
}

void Dlg_EditHotkey::SetNumHotkeys( int numHotkeys )
{
	_numHotkeys = numHotkeys;

	ui->numHotkeys->setText( QString::number( numHotkeys ) );

	QSignalBlocker blocker( ui->listHotkeys );

	_list.clear();
	ui->listHotkeys->clear();

	for( int i = 0; i < numHotkeys; ++i )
	{
		const int idInc = i + 1;
		const int idDec = -i - 1;

		auto item = new HotkeyItemWidget( this, _scoreHotkeys[idInc], _scoreHotkeys[idDec] );

		item->SetTitle( i );

		const auto wItem = new QListWidgetItem;
		wItem->setSizeHint( item->minimumSize() );

		ui->listHotkeys->addItem( wItem );
		ui->listHotkeys->setItemWidget( wItem, item );

		_list.push_back( item );
	}

}

void Dlg_EditHotkey::SetDecStep( double step )
{
	ui->decStep->setText( QString::number( step ) );
}

void Dlg_EditHotkey::SetIncStep( double step )
{
	ui->incStep->setText( QString::number( step ) );
}

double Dlg_EditHotkey::GetDecStep() const
{
	return ui->decStep->text().toDouble();
}

double Dlg_EditHotkey::GetIncStep() const
{
	return ui->incStep->text().toDouble();
}
