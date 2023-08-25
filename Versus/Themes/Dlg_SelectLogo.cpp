// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Dlg_SelectLogo.h"
#include <QDir>
#include <QDirIterator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "ui_Dlg_SelectLogo.h"

Dlg_SelectLogo::Dlg_SelectLogo(QWidget *parent)
	: QDialog( parent, Qt::WindowFlags::enum_type::WindowCloseButtonHint )
	, ui( new Ui::Dlg_SelectLogo )
	, _lastUsedLogoPack( ".default" )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi(this);

	connect( ui->logoPack, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &Dlg_SelectLogo::OnChangeCombo );

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	connect( ui->logoList, &QListWidget::doubleClicked, this, &QDialog::accept );

	QPixmap bkgnd( ":/Versus/bg_logo.png" );
	//bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );
}

Dlg_SelectLogo::~Dlg_SelectLogo()
{
	delete ui;
}

void Dlg_SelectLogo::Init( const QString& pack )
{
	if( !pack.isEmpty() )
		_lastUsedLogoPack = pack;

	InitLogoPacks();
}

void Dlg_SelectLogo::OnChangeCombo( int /*idx*/ )
{
	UpdateLogoList();
}

void Dlg_SelectLogo::InitLogoPacks()
{
	try
	{
		ui->logoPack->clear();

		int curIndex = 0;
		int idx = 0;

		const QString themeDir = QCoreApplication::applicationDirPath() + "/versus_logo/";

		QDirIterator it( themeDir, QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot );
		while( it.hasNext() )
		{
			it.next();
			ui->logoPack->addItem( it.fileName() );

			if( it.fileName().compare( _lastUsedLogoPack, Qt::CaseInsensitive ) == 0 )
				curIndex = idx;

			++idx;
		}

		ui->logoPack->setCurrentIndex( curIndex );
	}
	catch( ... )
	{
	}
}

void Dlg_SelectLogo::UpdateLogoList()
{
	ui->logoList->clear();
	_filename.clear();

	if( ui->logoPack->count() <= 0 )
		return;

	const QString packName = ui->logoPack->currentText();

	const QString baseDir = QCoreApplication::applicationDirPath();
	const QString logoDir = baseDir + "/versus_logo/" + packName + "/";

	QDir dir( logoDir );
	QFileInfoList stringList = dir.entryInfoList( QStringList{"*.jpg", "*.jpeg", "*.png", "*.gif", "*.svg"}, QDir::Filter::Files, QDir::SortFlag::IgnoreCase );

	ui->logoList->setDragDropMode( QAbstractItemView::NoDragDrop );
	for( const auto& s : stringList )
	{
		const QString icon = logoDir + s.fileName();
		ui->logoList->addItem( new QListWidgetItem( QPixmap( icon ), s.baseName() ) );
		_filename.emplace_back( s.fileName() );
	}

	ui->logoList->setCurrentRow( 0 );
}

void Dlg_SelectLogo::GetFileName( QString& packName, QString& imageName ) const
{
	const int idx = ui->logoList->currentRow();
	if( idx >= 0 && idx < (int)_filename.size() )
	{
		packName = ui->logoPack->currentText();

		imageName = _filename[idx];
	}
}

void Dlg_SelectLogo::mousePressEvent( QMouseEvent* event )
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
