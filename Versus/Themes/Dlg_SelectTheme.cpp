// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Dlg_SelectTheme.h"
#include <QCollator>
#include <QDirIterator>
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>
#include "ui_Dlg_SelectTheme.h"

Dlg_SelectTheme::Dlg_SelectTheme(QWidget *parent)
	: QDialog(parent, Qt::WindowFlags::enum_type::WindowCloseButtonHint )
	, ui( new Ui::Dlg_SelectTheme )
{
	setStyleSheet( parent->styleSheet() );

	ui->setupUi(this);

	connect( ui->btnOK, &QPushButton::clicked, this, &QDialog::accept );
	connect( ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject );

	connect( ui->themeNames, &QListWidget::itemSelectionChanged, this, &Dlg_SelectTheme::OnChangeTheme );
}

Dlg_SelectTheme::~Dlg_SelectTheme()
{
	delete ui;
}

void Dlg_SelectTheme::Init( const QString& uid )
{
	_lastUsed = uid;

	QPixmap bkgnd( ":/Versus/bg_theme.png" );
	//bkgnd = bkgnd.scaled( this->size(), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation );
	QPalette palette;
	palette.setBrush( QPalette::Window, bkgnd );
	this->setPalette( palette );

	FindThemes();

	int idx = 0;
	for( int curIdx = 0; curIdx < (int)_themes.size(); ++curIdx )
	{
		const auto& info = _themes[curIdx];
		ui->themeNames->addItem( info.title );

		if( info.dir == _lastUsed )
			idx = curIdx;
	}

	ui->themeNames->setCurrentRow( idx );
}

void Dlg_SelectTheme::FindThemes()
{
	try
	{
		const QString themeDir = QCoreApplication::applicationDirPath() + "/themes/";

		QDirIterator it( themeDir, QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot );
		while( it.hasNext() )
		{
			const QString dir = it.next();
			const QString themeFile = dir + "/theme.json";

			ThemeInfo info;
			info.dir = dir;
			ReadInfo( themeFile, info );

			if( !info.title.isEmpty() )
				_themes.emplace_back( info );
		}

		QCollator collator;
		collator.setCaseSensitivity( Qt::CaseInsensitive );
		collator.setNumericMode( true );

		std::sort(
			_themes.begin(),
			_themes.end(),
			[&collator]( const ThemeInfo& lhs, const ThemeInfo& rhs )
					{
						return collator.compare( lhs.title, rhs.title ) < 0;
					}
		);
	}
	catch( ... )
	{
	}
}

void Dlg_SelectTheme::ReloadPreview()
{
	int idx = ui->themeNames->currentRow();
	if( idx < 0 )
	{
		ui->themePreview->setText( "no image" );
		return;
	}

	const auto & info = _themes.at( idx );

	const QString themeImage = info.dir + "/theme.png";
	QPixmap pm( themeImage );
	if( pm.isNull() )
	{
		ui->themePreview->setText( "no image" );
	}
	else
	{
		ui->themePreview->setPixmap( pm );
	}
}

void Dlg_SelectTheme::OnChangeTheme()
{
	ui->themeProps->clear();

	int idx = ui->themeNames->currentRow();
	if( idx < 0 )
	{
		return;
	}

	ReloadPreview();

	const auto& info = _themes.at( idx );

	QString text;
	text += tr("Game:\t") + info.game + "\n";
	text += tr("Source size:\t") + info.sourceSize + "\n";
	text += tr("Align:\t") + info.align + "\n";
	text += "\n";
	text += tr("Title:\t") + info.title + "\n";
	text += tr("Author:\t") + info.author + "\n";
	text += tr("Homepage:\t") + info.homepage + "\n";

	ui->themeProps->setPlainText( text );
}

ThemeInfo Dlg_SelectTheme::GetThemeInfo() const
{
	int idx = ui->themeNames->currentRow();
	if( idx < 0 )
		return ThemeInfo();

	return _themes.at( idx );
}

void Dlg_SelectTheme::mousePressEvent( QMouseEvent* event )
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