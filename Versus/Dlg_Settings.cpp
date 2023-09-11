// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Dlg_Settings.h"
#include <QIntValidator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "ui_Dlg_Settings.h"
#include "Versus.h"

QString GetServerAddress( const QString& ip, const QString & port )
{
	return QString( "%1:%2" ).arg( ip ).arg( port );
}

Dlg_Settings::Dlg_Settings( Versus* parent, const QString & ip, int webPort, int tcpPort, bool autoStart )
	: QDialog( parent, Qt::WindowFlags::enum_type::WindowCloseButtonHint )
	, ui(new Ui::Dlg_SettingsClass())
	, _base( parent )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi(this);

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	connect( ui->portScore, &QLineEdit::textChanged, this, [this](){ ui->serverAddress->setText( GetServerAddress( ui->serverIP->text(), ui->portScore->text() ) ); } );
	connect( ui->serverIP, &QLineEdit::textChanged, this, [this](){ ui->serverAddress->setText( GetServerAddress( ui->serverIP->text(), ui->portScore->text() ) ); } );

	ui->serverIP->setText( ip );
	ui->portScore->setText( QString::number( webPort ) );
	ui->portScore->setValidator( new QIntValidator( 1025, 65535, this ) );

	ui->portTcp->setText( QString::number( tcpPort ) );
	ui->portTcp->setValidator( new QIntValidator( 1025, 65535, this ) );

	ui->cbAutostart->setCheckState( autoStart ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );

	ui->serverAddress->setText( GetServerAddress( ip, QString::number( webPort ) ) );

	QPixmap bkgnd( ":/Versus/bg_settings.png" );
	//bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_Settings::~Dlg_Settings()
{
	delete ui;
}

QString Dlg_Settings::GetLocalIP() const
{
	return ui->serverIP->text();
}

int Dlg_Settings::GetWebServerPort() const
{
	return ui->portScore->text().toInt();
}

int Dlg_Settings::GetTcpSocketPort() const
{
	return ui->portTcp->text().toInt();
}

bool Dlg_Settings::GetAutoStart() const
{
	return ui->cbAutostart->checkState() == Qt::CheckState::Checked;
}

void Dlg_Settings::mousePressEvent( QMouseEvent* event )
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