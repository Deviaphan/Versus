// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Dlg_CheckUpdate.h"
#include "ui_Dlg_CheckUpdate.h"

Dlg_CheckUpdate::Dlg_CheckUpdate( QWidget* parent )
	: QDialog( parent, Qt::WindowFlags::enum_type::WindowCloseButtonHint )
	, ui( new Ui::Dlg_CheckUpdate )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi( this );

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );

	QPixmap bkgnd( ":/Versus/bg_update.png" );
	//bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_CheckUpdate::~Dlg_CheckUpdate()
{
	delete ui;
}

void Dlg_CheckUpdate::SetVer( const QString& oldVer, const QString& newVer )
{
	ui->editOld->setText( oldVer );
	ui->editNew->setText( newVer );
}

void Dlg_CheckUpdate::SetNotes( const QVector<QString>& notes )
{
	QString rn;

	for( const QString& s : notes )
	{
		rn += " - " + s + "\r\n";
	}

	ui->releaseNotes->setPlainText( rn );
}
