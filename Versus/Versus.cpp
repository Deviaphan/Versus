// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "stdafx.h"

#include "Versus.h"
#include <algorithm>
#include <chrono>
#include <QFile>
#include <QtWidgets>
#include <QWebSocket>
#include <set>
#include "CalcWindowPosition.h"
#include "Dlg_EditList.h"
#include "Dlg_SetPlayerGroup.h"
#include "Dlg_Settings.h"
#include "PlayerManager.h"
#include "SharedTools.h"
#include "WaitCursor.h"
#include "CheckNew/Dlg_CheckUpdate.h"
#include "Hotkey/Dlg_EditHotkey.h"
#include "Themes/Dlg_SelectTheme.h"
#include "Themes/LogoManager.h"


void SendToSocket( QList<QWebSocket*>& webSockets, const QString& json )
{
	for( auto ii = webSockets.begin(); ii != webSockets.end(); )
	{
		if( (*ii)->isValid() )
		{
			(*ii)->sendTextMessage( json );
			++ii;
		}
		else
		{
			(*ii)->close();
			ii = webSockets.erase( ii );
		}
	}
}


Versus::Versus( QWidget* parent )
	: QDialog( parent, Qt::WindowFlags::enum_type::WindowCloseButtonHint | Qt::WindowFlags::enum_type::WindowMinimizeButtonHint )
	, _socketServer( "", QWebSocketServer::SslMode::NonSecureMode )
	, _numHotkeys( 4 )
	, _decStep( -1.0 )
	, _incStep( 1.0 )
	, _localIP("localhost")
	, _webServerPort( 9090 )
	, _tcpSocketPort( 9091 )
	, _autoStart( false )
	, _timerGroupIndex( 0 )
	, _timerId( 0 )
	, _totalTime( 0 )
	, _currentTime( 0 )
	, _timerState( TimerEvents::Stop )
	, _stickyEdges( true )
{
	ui.setupUi( this );

	{
		connect( ui.btnSettings, &QPushButton::clicked, this, &Versus::OnEditSettings );
		connect( ui.btnScoreServer, &QPushButton::clicked, this, &Versus::OnStartStopServer );
		connect( &_socketServer, &QWebSocketServer::newConnection, this, [this]{ this->AddSockets(); } );

		connect( ui.btnUpdate, &QPushButton::clicked, this, &Versus::OnSendScoreboardData );
	}

	{
		connect( ui.btnSelectTheme, &QPushButton::clicked, this, &Versus::OnSelectTheme );

		connect( ui.styleIndex, &QComboBox::currentIndexChanged, this, [this]
		{
			VS_TRY;
		const int idx = ui.styleIndex->currentIndex();
		if( idx >= 0 )
		{
			this->_themeInfo.currentStyle = idx;
			for( int i = 0; i < (int)_currentSet.size(); ++i )
			{
				_currentSet.at( i ).scoreColor = _themeInfo.styles[idx].colors.at( i );
				_currentSet.at( i ).UpdateItem();
			}

			OnSendScoreboardData();
		}
		VS_CATCH;
		} );
	}

	{
		_timerGroupIndex = ui.tabTimers->currentIndex();

		ui.timeCountdown->setTime( QTime( 0, 0, 30 ) );
		ui.timeStopwatch->setTime( QTime( 0, 0, 30 ) );

		ui.btnTimerStart->setDisabled( true );
		ui.btnTimerPause->setDisabled( true );
		ui.btnTimerStop->setDisabled( true );

		connect( ui.btnTimerStart, &QPushButton::clicked, this, &Versus::OnTimerStart );
		connect( ui.btnTimerPause, &QPushButton::clicked, this, &Versus::OnTimerPause );
		connect( ui.btnTimerStop, &QPushButton::clicked, this, &Versus::OnTimerStop );
	}

	{
		connect( ui.btnEditTournament, &QPushButton::clicked, this, &Versus::OnEditTournamentTitle );
		connect( ui.btnEditStage, &QPushButton::clicked, this, &Versus::OnEditStage );
		connect( ui.btnEditBestOf, &QPushButton::clicked, this, &Versus::OnEditBestOf );

		connect( ui.tournamentTitle, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [this]( int index ){ui.tournamentTitle->setCurrentIndex( index ); } );
		connect( ui.bestOf, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [this]( int index ){ui.bestOf->setCurrentIndex( index ); } );
		connect( ui.stageTitle, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [this]( int index ){ui.stageTitle->setCurrentIndex( index ); } );
	}

	{
		connect( ui.btnHotkeys, &QPushButton::clicked, this, &Versus::OnEditHotkey );

		connect( ui.pushPlayerGroup, &QPushButton::clicked, this, &Versus::OnSetPlayerGroup );

		connect( ui.pushSortSwap, &QPushButton::clicked, this, &Versus::OnSortSwap );
		connect( ui.pushSortUp, &QPushButton::clicked, this, &Versus::OnSortUp );
		connect( ui.pushSortDown, &QPushButton::clicked, this, &Versus::OnSortDown );
		connect( ui.pushReset, &QPushButton::clicked, this, &Versus::OnResetScores );
	}

	//qApp->installEventFilter( this );
}

Versus::~Versus()
{
	VS_TRY;

	StopServer();

	SaveSettings();

	VS_CATCH;
}

void Versus::Init()
{
	VS_TRY;

	WaitCursor wait;

	{
		const QString bgFile = QCoreApplication::applicationDirPath() + "/background.png";

		if( QFile::exists( bgFile ) )
		{
			QPixmap bkgnd( bgFile );
			bkgnd = bkgnd.scaled( this->size(), Qt::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation );
			QPalette palette;
			palette.setBrush( QPalette::Window, bkgnd );
			this->setPalette( palette );
		}
		else
		{
			QPixmap bkgnd( ":/Versus/Background" );
			QPalette palette;
			palette.setBrush( QPalette::Window, bkgnd );
			this->setPalette( palette );
		}

		LoadBestOfList( ui.bestOf );
		LoadGroupList( ui.stageTitle );

		connect( &_downloader, &Downloader::onReady, this, &Versus::CheckVersion );

		const QUrl updateUrl( "http://maagames.ru/files/versus/version.json" );
		_downloader.GetData( updateUrl );
	}

	ui.btnScoreServer->setIconSize( QSize( 48, 48 ) );
	ui.btnScoreServer->setIcon( QIcon( QPixmap( ":/Versus/start.png" ) ) );

	ui.btnUpdate->setIconSize( QSize( 48, 48 ) );
	ui.btnUpdate->setIcon( QIcon( QPixmap( ":/Versus/update-data.png" ) ) );

	LoadSettings();

	if( _themeInfo.title.isEmpty() )
	{
		emit OnSelectTheme();
	}
	else
	{
		LoadTheme();
	}

	if( _autoStart )
	{
		emit OnStartStopServer();
	}

	VS_CATCH;
}

QString Versus::GetSettingsFile() const
{
	return QCoreApplication::applicationDirPath() + "/settings.json";
}

void Versus::EditScore( int id, double step )
{
	VS_TRY;

	const int index = std::abs( id ) - 1;

	if( index >= (int)_currentSet.size() || index < 0 )
		return;

	_currentSet.at( index ).widget->EditScores( step );

	VS_CATCH;
}

void Versus::RegisterScoreHotkey( int id, const QKeySequence& keySeq )
{
	if( keySeq.isEmpty() )
		return;

	HotkeyMap::iterator ii = _globalShortcuts.insert( id, std::make_shared< QxtGlobalShortcut>( keySeq ) );
	if( ii != _globalShortcuts.end() )
	{
		QObject::connect( ii.value().get(), &QxtGlobalShortcut::activated, this,
			[this, id]
			{
				this->EditScore( id, (id > 0) ? this->GetIncStep() : this->GetDecStep() );
			} );
	}
}
void Versus::UnregisterScoreHotkey( const QxtGlobalShortcut * shortcut )
{
	if( shortcut )
	{
		QObject::disconnect( shortcut, &QxtGlobalShortcut::activated, nullptr, nullptr );
	}
}

void Versus::LoadSettings()
{
	VS_TRY;

	QFile inFile( GetSettingsFile() );
	inFile.open( QIODevice::ReadOnly | QIODevice::Text );
	const QByteArray byteData = inFile.readAll();
	inFile.close();

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( byteData, &errorPtr );
	if( doc.isNull() )
	{
		LoadTournamentList( nullptr, ui.tournamentTitle );

		return;
	}

	QJsonObject rootObj = doc.object();

	const QString themeDir = ReadJsonValue( rootObj, "themeDir" );
	const int themeStyleIndex = ReadJsonValue( rootObj, "themeStyleIndex", "0" ).toInt();

	const QString localIP = ReadJsonValue( rootObj, "localIP" );
	if( !localIP.isEmpty() )
	{
		_localIP = localIP;
	}

	const QString port = ReadJsonValue( rootObj, "port" );
	if( !port.isEmpty() )
	{
		_webServerPort = port.toInt();
	}	

	const QString portTcp = ReadJsonValue( rootObj, "portTcp" );
	if( !portTcp.isEmpty() )
	{
		_tcpSocketPort = portTcp.toInt();
	}

	const QString autoStart = ReadJsonValue( rootObj, "autoStart", "0" );
	if( !autoStart.isEmpty() )
	{
		_autoStart = autoStart.toInt() != 0;
	}

	const QString stickyEdges = ReadJsonValue( rootObj, "stickyEdges", "1" );
	if( !stickyEdges.isEmpty() )
	{
		_stickyEdges = stickyEdges.toInt() != 0;
	}
	
	LoadTournamentList( &rootObj, ui.tournamentTitle );

	ui.tournamentTitle->setCurrentText( ReadJsonValue( rootObj, "lastTitle", ui.tournamentTitle->currentText() ) );
	ui.stageTitle->setCurrentText( ReadJsonValue( rootObj, "lastGroup", ui.stageTitle->currentText() ) );
	ui.bestOf->setCurrentText( ReadJsonValue( rootObj, "lastBo3", ui.bestOf->currentText() ) );

	LogoManager::lastPlayerLogoPack = ReadJsonValue( rootObj, "playerLogoPack", LogoManager::lastPlayerLogoPack );
	LogoManager::lastTeamLogoPack = ReadJsonValue( rootObj, "teamLogoPack", LogoManager::lastTeamLogoPack );

	_decStep = ReadJsonValue( rootObj, "decStep", "-1" ).toDouble();
	_incStep = ReadJsonValue( rootObj, "incStep", "1" ).toDouble();

	_numHotkeys = ReadJsonValue( rootObj, "numHotkeys", "4" ).toInt();

	QJsonArray dataArray = rootObj["hotkeys"].toArray();
	for( QJsonValueRef value : dataArray )
	{
		QJsonObject item = value.toObject();
		int id = item["id"].toInt( 0 );
		if( id == 0 )
			continue;

		QString hk = item["hk"].toString();

		QKeySequence keySeq( hk, QKeySequence::PortableText );
		RegisterScoreHotkey( id, keySeq );
	}

	if( !themeDir.isEmpty() )
	{
		ReadInfo( themeDir + "/theme.json", _themeInfo );
		_themeInfo.dir = themeDir;
		_themeInfo.currentStyle = (std::min)(themeStyleIndex, _themeInfo.numStyles - 1);
	}

	{
		const QString curTab = ReadJsonValue( rootObj, "timerIndex", "2" );
		ui.tabTimers->setCurrentIndex( curTab.toInt() );

		const QString countdown = ReadJsonValue( rootObj, "timerCountdown", "00:00:30" );
		ui.timeCountdown->setTime( QTime::fromString( countdown, "HH:mm:ss" ) );

		const QString stopwatch = ReadJsonValue( rootObj, "timerStopwatch", "00:00:30" );
		ui.timeStopwatch->setTime( QTime::fromString( stopwatch, "HH:mm:ss" ) );
	}

	// загрузка группы игроков
	{
		QJsonArray playerArray = rootObj["defaultPlayersGroup"].toArray();
		for( QJsonValueRef value : playerArray )
		{
			QJsonObject p = value.toObject();

			PlayerItemData pid;

			pid.player = ReadJsonValue( p, "playerName", "Player" );
			pid.team = ReadJsonValue( p, "teamName", "Team" );
			pid.playerLogo = ReadJsonValue( p, "playerLogo", "" );
			pid.teamLogo = ReadJsonValue( p, "teamLogo", "" );
			pid.score = ReadJsonValue( p, "score", "0" );

			_currentSet.emplace_back( pid );
		}
	}

	constexpr int NO_POS = -1000000;
	const int posX = rootObj["posX"].toInt( NO_POS );
	const int posY = rootObj["posY"].toInt( NO_POS );
	int windowWidth = rootObj["windowWidth"].toInt( NO_POS );
	int windowHeight = rootObj["windowHeight"].toInt( NO_POS );

	if( windowWidth == NO_POS || windowHeight == NO_POS )
	{
		const QSize thisRect = this->size();
		windowWidth = thisRect.width();
		windowHeight = thisRect.height();
	}

	this->resize( windowWidth, windowHeight );

	if( posX != NO_POS && posY != NO_POS )
	{
		CalcWindowPosition( posX, posY, windowWidth, windowHeight, this );
	}

	PlayerManager::Instance().LoadDB();

	VS_CATCH;
}

void Versus::SaveSettings()
{
	VS_TRY;

	WaitCursor wait;

	UpdatePlayerDB();

	QJsonObject rootObj;

	const QPoint pos = this->pos();
	rootObj.insert( "posX", pos.x() );
	rootObj.insert( "posY", pos.y() );

	const QSize thisRect = this->size();
	rootObj.insert( "windowWidth", thisRect.width() );
	rootObj.insert( "windowHeight", thisRect.height() );

	rootObj.insert( "themeDir", _themeInfo.dir );
	rootObj.insert( "themeStyleIndex", QString::number( _themeInfo.currentStyle ) );

	rootObj.insert( "localIP", _localIP );
	rootObj.insert( "port", QString::number( _webServerPort ) );
	rootObj.insert( "portTcp", QString::number( _tcpSocketPort ) );
	rootObj.insert( "autoStart", QString::number( (int)_autoStart ) );
	rootObj.insert( "stickyEdges", QString::number( (int)_stickyEdges ) );

	SaveTournamentList( rootObj, ui.tournamentTitle );
	rootObj.insert( "lastTitle", ui.tournamentTitle->currentText() );
	rootObj.insert( "lastGroup", ui.stageTitle->currentText() );
	rootObj.insert( "lastBo3", ui.bestOf->currentText() );

	rootObj.insert( "playerLogoPack", LogoManager::lastPlayerLogoPack );
	rootObj.insert( "teamLogoPack", LogoManager::lastTeamLogoPack );

	// таймеры
	{
		const int curTab = ui.tabTimers->currentIndex();
		rootObj.insert( "timerIndex", QString::number( curTab ) );

		const QString countdown = ui.timeCountdown->time().toString( "HH:mm:ss" );
		rootObj.insert( "timerCountdown", countdown );

		const QString stopwatch = ui.timeStopwatch->time().toString( "HH:mm:ss" );
		rootObj.insert( "timerStopwatch", stopwatch );
	}

	// сохранение группы игроков
	{
		QJsonArray playerArray;
		for( const auto& pid : _currentSet )
		{
			QJsonObject p;

			p.insert( "playerName", pid.player );
			p.insert( "teamName", pid.team );
			p.insert( "playerLogo", pid.playerLogo );
			p.insert( "teamLogo", pid.teamLogo );
			p.insert( "score", pid.score );

			playerArray.push_back( p );
		}

		rootObj.insert( "defaultPlayersGroup", playerArray );
	}

	rootObj.insert( "decStep", QString::number( _decStep ) );
	rootObj.insert( "incStep", QString::number( _incStep ) );

	if( !_globalShortcuts.empty() )
	{
		rootObj.insert( "numHotkeys", QString::number( _numHotkeys ) );

		QJsonArray hotkeys;

		for( HotkeyMap::const_iterator iiHotkey = _globalShortcuts.begin(); iiHotkey != _globalShortcuts.end(); ++iiHotkey )
		{
			QJsonObject v;
			v.insert( "id", iiHotkey.key() );
			v.insert( "hk", iiHotkey.value()->shortcut().toString() );
			hotkeys.push_back( v );
		}
		rootObj.insert( "hotkeys", hotkeys );
	}

	QFile outFile( GetSettingsFile() );
	outFile.open( QIODevice::WriteOnly | QIODevice::Text );

	const QJsonDocument saveDoc( rootObj );
	outFile.write( saveDoc.toJson() );

	PlayerManager::Instance().SaveDB();

	SaveGroupList( ui.stageTitle );
	SaveBestOfList( ui.bestOf );

	VS_CATCH;
}


void Versus::OnSelectTheme()
{
	Dlg_SelectTheme dlg( this );
	dlg.Init( _themeInfo.dir );

	if( dlg.exec() == QDialog::Rejected )
		return;

	_themeInfo = dlg.GetThemeInfo();

	LoadTheme();
}

void Versus::LoadTheme()
{
	VS_TRY;

	ui.themeName->setText( _themeInfo.title );
	ui.widgetSize->setText( _themeInfo.sourceSize );

	QSignalBlocker block( ui.styleIndex );

	ui.styleIndex->clear();
	if( _themeInfo.styles.empty() )
	{
		ui.styleIndex->addItem( "-/-" );
	}
	else
	{
		for( const auto& item : _themeInfo.styles )
		{
			ui.styleIndex->addItem( item.name );
		}
	}
	ui.styleIndex->setCurrentIndex( _themeInfo.currentStyle );

	_currentSet.resize( _themeInfo.numPlayers );

	UpdatePlayerSet();

	if( _server )
	{
		const QString json = R"({"updateView":1})";
		SendToSocket( _webSockets, json );
	}

	VS_CATCH;
}

void Versus::UpdatePlayerColors()
{
	VS_TRY;

	for( int i = 0; i < _themeInfo.numPlayers; ++i )
	{
		_currentSet.at( i ).scoreColor = _themeInfo.styles.at( _themeInfo.currentStyle ).colors.at( i );
	}

	VS_CATCH;
}

void Versus::ReadItems() const
{
	for( auto& pid : _currentSet )
	{
		pid.widget->ReadItem();
	}
}

void Versus::UpdatePlayerSet()
{
	QSignalBlocker blocker( ui.playerListBox );

	UpdatePlayerColors();

	ui.playerListBox->clear();

	int index = 1;
	for( auto& itemData : _currentSet )
	{
		itemData.itemTitle = QString( "Player %1" ).arg( index );
		++index;

		itemData.widget = new PlayerItemWidget( this );
		itemData.widget->SetItemData( &itemData );
		itemData.UpdateItem();

		const auto item1 = new QListWidgetItem;
		item1->setSizeHint( itemData.widget->minimumSize() );

		ui.playerListBox->addItem( item1 );
		ui.playerListBox->setItemWidget( item1, itemData.widget );
	}
}

void Versus::OnSendScoreboardData()
{
	VS_TRY;

	UpdatePlayerDB();

	if( !_server )
		return;

	QJsonObject obj;
	obj.insert( "updateView", (int)0 );

	QJsonArray byteData;

	Add( byteData, "themeStyleIndex", QString::number( _themeInfo.currentStyle ) );

	Add( byteData, "title", ui.tournamentTitle->currentText() );
	Add( byteData, "group", ui.stageTitle->currentText() );
	Add( byteData, "bo3", ui.bestOf->currentText() );

	for( int i = 0; i < (int)_currentSet.size(); ++i )
	{
		const PlayerItemData& item = _currentSet.at( i );

		Add( byteData, QString( "p%1name" ).arg( i + 1 ), item.player );
		Add( byteData, QString( "p%1score" ).arg( i + 1 ), item.score );
		Add( byteData, QString( "p%1logo" ).arg( i + 1 ), item.playerLogo );
		Add( byteData, QString( "t%1name" ).arg( i + 1 ), item.team );
		Add( byteData, QString( "t%1logo" ).arg( i + 1 ), item.teamLogo );

		Add( byteData, QString( "p%1color" ).arg( i + 1 ), item.scoreColor );
	}

	obj.insert( "data", byteData );

	const QJsonDocument doc( obj );

	SendToSocket( _webSockets, doc.toJson() );

	VS_CATCH;
}


void Versus::mousePressEvent( QMouseEvent* event )
{
	if( event->buttons().testFlag( Qt::LeftButton ) )
	{
		//_mpos = event->pos();
		HWND hWnd = ::GetAncestor( (HWND)(window()->windowHandle()->winId()), GA_ROOT );
		POINT pt;
		::GetCursorPos( &pt );
		::ReleaseCapture();
		::SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS( pt ) );
	}

	QDialog::mousePressEvent( event );
}

class Skip
{
public:
	Skip( bool& v )
		:_value( v )
	{
		_value = true;
	}

	~Skip()
	{
		_value = false;
	}

private:
	bool & _value;
};

void Versus::moveEvent( QMoveEvent* /*event*/ )
{
	if( !_stickyEdges )
		return;

	static bool skip_pass = false;

	if( skip_pass )
	{
		return;
	}

	Skip skipGuard( skip_pass );

	VS_TRY;

	const auto rect = this->frameGeometry();
	const int w = rect.width();
	const int h = rect.height();

	const int windowTopBorder = rect.y();
	const int windowBottomBorder = windowTopBorder + h;
	const int windowLeftBorder = rect.x();
	const int windowRightBorder = windowLeftBorder + w;

	bool eventComplete = false;
	int posX = windowLeftBorder;
	int posY = windowTopBorder;
	
	const QScreen* ii = QApplication::screenAt( QPoint{ windowLeftBorder + w / 2, windowTopBorder + h / 2 } );
	if( ii )
	{
		constexpr int magnetDistance = 16;

		const QRect sr = ii->availableGeometry();

		const int screenTopBorder = sr.top();
		const int screenBottomBorder = sr.bottom();
		const int screenLeftBorder = sr.left();
		const int screenRightBorder = sr.right();

		if( abs( windowRightBorder - screenRightBorder ) < magnetDistance )
		{
			if( (windowRightBorder - screenRightBorder) != 0 )
			{
				if( posX != screenRightBorder - w )
				{
					posX = screenRightBorder - w;
					eventComplete = true;
				}
			}
		}
		else if( abs( windowLeftBorder - screenLeftBorder ) < magnetDistance )
		{
			if( posX != screenLeftBorder )
			{
				if( (windowLeftBorder - screenLeftBorder) != 0 )
				{
					posX = screenLeftBorder;
					eventComplete = true;
				}
			}
		}

		if( abs( windowBottomBorder - screenBottomBorder ) < magnetDistance )
		{
			if( posY != screenBottomBorder - h )
			{
				if( (windowBottomBorder - screenBottomBorder) != 0 )
				{
					posY = screenBottomBorder - h;
					eventComplete = true;
				}
			}
		}
		else if( abs( windowTopBorder - screenTopBorder ) < magnetDistance )
		{
			if( posY != screenTopBorder )
			{
				if( (windowTopBorder - screenTopBorder) != 0 )
				{
					posY = screenTopBorder;
					eventComplete = true;
				}
			}
		}
	}

	if( eventComplete )
	{
		move( posX, posY );
	}

	VS_CATCH;
}

void Versus::keyPressEvent( QKeyEvent* e )
{
	if( e->key() != Qt::Key_Escape )
	{
		QDialog::keyPressEvent( e );
	}
}

void Versus::OnStartStopServer()
{
	if( _server == nullptr )
	{
		StartServer();
	}
	else
	{
		StopServer();
	}
}

void Versus::StartServer()
{
	VS_TRY;

	WaitCursor wait;

	const int port = GetHTMLPort();
	const int tcpPort = GetTCPPort();
	const QString & ip = GetLocalIP();

	_server = std::make_unique<QHttpServer>( this );

	_server->route( "/", [this]()
	{
		const QString indexFile = this->_themeInfo.dir + "/index.html";
		return QHttpServerResponse::fromFile( indexFile );
	} );

	_server->route( "/controls.html", [this]()
	{
		QString htmlPath = QCoreApplication::applicationDirPath();
		htmlPath += "/themes/controls.html";

		QFile inFile( htmlPath );
		inFile.open( QIODevice::ReadOnly | QIODevice::Text );
		const QByteArray byteData = inFile.readAll();
		inFile.close();

		QString playersData;
		
		const QString tableTemplate = R"!(<table>
		<tr><td class='__PXLOGO__ photo'></td><th class='__PXNAME__ Name' style='background-color:__PLAYERCOLOR__;'>__PLAYER_NAME__</th><td class='__TXLOGO__ photo'></td></tr>
		<tr><td class="BtnDec" onclick='OnSendScore( "__PXSCORE__", "-")'>&nbsp;</td><td class="__PXSCORE__ Score">__PLAYER_SCORE__</td><td class="BtnInc" onclick='OnSendScore( "__PXSCORE__", "+")'>&nbsp;</td></tr>
		</table>)!";

		for( int i = 0; i < (int)_currentSet.size(); ++i )
		{
			const PlayerItemData& item = _currentSet.at( i );

			const QString pXname = QString( "p%1name" ).arg( i + 1 );
			const QString pXscore = QString( "p%1score" ).arg( i + 1 );
			const QString pXlogo = QString( "p%1logo" ).arg( i + 1 );
			//const QString tXname = QString( "t%1name" ).arg( i + 1 );
			const QString tXlogo = QString( "t%1logo" ).arg( i + 1 );
			const QString pXColor = QString( "p%1color" ).arg( i + 1 );

			QString tt = tableTemplate;
			
			tt.replace( "__PXLOGO__", pXlogo );
			tt.replace( "__PXNAME__", pXname );
			tt.replace( "__TXLOGO__", tXlogo );
			tt.replace( "__PLAYERCOLOR__", item.scoreColor );
			tt.replace( "__PLAYER_NAME__", item.player );
			tt.replace( "__PLAYER_SCORE__", item.score );
			tt.replace( "__PXSCORE__", pXscore );

			playersData += tt;
		}

		QString htmlData( byteData );

		htmlData.replace( "__PLAYERS__", playersData );

		return QHttpServerResponse( htmlData );
	} );

	_server->route( "/default_send.js", [this]()
	{
		QString jsPath = QCoreApplication::applicationDirPath();
		jsPath += "/themes/default_send.js";
		return QHttpServerResponse::fromFile( jsPath );
	} );

	_server->route( "/default_timer.js", [this]()
	{
		QString jsPath = QCoreApplication::applicationDirPath();
		jsPath += "/themes/default_timer.js";
		return QHttpServerResponse::fromFile( jsPath );
	} );

	_server->route( "/updateData.js", [tcpPort, ip]()
	{
		QString jsPath = QCoreApplication::applicationDirPath();
		jsPath += "/themes/updateData.js";

		QFile inFile( jsPath );
		inFile.open( QIODevice::ReadOnly | QIODevice::Text );
		const QByteArray byteData = inFile.readAll();
		inFile.close();

		QString jsData( byteData );
		jsData.replace( "%LOCALHOST%", ip );
		jsData.replace( "%PORT%", QString::number( tcpPort ) );

		return QHttpServerResponse( jsData );
	} );

	_server->route( "/updateDataAdvanced.js", [tcpPort, ip]()
	{
		QString jsPath = QCoreApplication::applicationDirPath();
		jsPath += "/themes/updateDataAdvanced.js";

		QFile inFile( jsPath );
		inFile.open( QIODevice::ReadOnly | QIODevice::Text );
		const QByteArray byteData = inFile.readAll();
		inFile.close();

		QString jsData( byteData );
		jsData.replace( "%LOCALHOST%", ip );
		jsData.replace( "%PORT%", QString::number( tcpPort ) );

		return QHttpServerResponse( jsData );
	} );

	_server->route( "/versus_logo/<arg>", []( const QUrl& url )
	{
		const QString logoPath = QCoreApplication::applicationDirPath() + "/versus_logo/" + url.path();
		const QDir logoDir( logoPath );

		return QHttpServerResponse::fromFile( logoDir.absolutePath() );
	} );

	_server->route( "/<arg>", [this]( const QUrl& url ) 
	{
		const QString rootPath = this->_themeInfo.dir + "/" + url.path();
		const QDir rootDir( rootPath );

		return QHttpServerResponse::fromFile( rootDir.absolutePath() );
	} );

	_server->listen( QHostAddress::Any, port );

	_socketServer.listen( QHostAddress( "ws://localhost" ), tcpPort );

	ui.btnScoreServer->setIcon( QIcon( QPixmap( ":/Versus/stop.png" ) ) );
	ui.btnScoreServer->setText( "Stop server" );

	emit OnSendScoreboardData();

	ui.btnTimerStart->setDisabled( false );
	ui.btnTimerPause->setDisabled( true );
	ui.btnTimerStop->setDisabled( true );

	VS_CATCH;
}

void Versus::StopServer()
{
	VS_TRY;

	WaitCursor wait;

	OnTimerStop();

	for( QWebSocket* s : _webSockets )
	{
		QObject::disconnect( s, &QWebSocket::textMessageReceived, nullptr, nullptr );

		s->close();
	}
	_webSockets.clear();

	if( _server )
	{
		_server.reset();
	}

	_socketServer.close();

	ui.btnScoreServer->setIcon( QIcon( QPixmap( ":/Versus/start.png" ) ) );
	ui.btnScoreServer->setText( "Start server" );

	ui.btnTimerStart->setDisabled( true );
	ui.btnTimerPause->setDisabled( true );
	ui.btnTimerStop->setDisabled( true );

	VS_CATCH;
}

void Versus::OnEditTournamentTitle()
{
	VS_TRY;

	Dlg_EditList dlg( this );

	for( int i = 0; i < ui.tournamentTitle->count(); ++i )
	{
		dlg.items.emplace_back( ui.tournamentTitle->itemText( i ) );
	}

	dlg.UpdateList();

	dlg.exec();

	QSignalBlocker blocker( ui.tournamentTitle );

	ui.tournamentTitle->clear();

	for( const auto& item : dlg.items )
	{
		ui.tournamentTitle->addItem( item );
	}

	ui.tournamentTitle->setCurrentIndex( dlg.GetSelectedIndex() );

	VS_CATCH;
}

void Versus::OnEditStage()
{
	VS_TRY;

	Dlg_EditList dlg( this );

	for( int i = 0; i < ui.stageTitle->count(); ++i )
	{
		dlg.items.emplace_back( ui.stageTitle->itemText( i ) );
	}

	dlg.UpdateList();

	dlg.exec();

	QSignalBlocker blocker( ui.stageTitle );

	ui.stageTitle->clear();

	for( const auto& item : dlg.items )
	{
		ui.stageTitle->addItem( item );
	}

	ui.stageTitle->setCurrentIndex( dlg.GetSelectedIndex() );

	VS_CATCH;
}

void Versus::OnEditBestOf()
{
	VS_TRY;

	Dlg_EditList dlg( this );

	for( int i = 0; i < ui.bestOf->count(); ++i )
	{
		dlg.items.emplace_back( ui.bestOf->itemText( i ) );
	}

	dlg.UpdateList();

	dlg.exec();

	QSignalBlocker blocker( ui.bestOf );

	ui.bestOf->clear();

	for( const auto& item : dlg.items )
	{
		ui.bestOf->addItem( item );
	}

	ui.bestOf->setCurrentIndex( dlg.GetSelectedIndex() );

	VS_CATCH;
}

void Versus::CheckVersion()
{
	VS_TRY;

	const QByteArray& result = _downloader.GetResult();

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( result, &errorPtr );
	if( doc.isNull() )
	{
		return;
	}

	const QJsonObject rootObj = doc.object();

	const QString newVer = ReadJsonValue( rootObj, "version" );


	if( newVer == CURRENT_VERSION )
		return;

	QVector<QString> releaseNodes;

	if( rootObj["notes"].isArray() )
	{
		QJsonArray dataArray = rootObj["notes"].toArray();
		for( QJsonValueRef value : dataArray )
		{
			if( value.isString() )
			{
				releaseNodes.emplace_back( value.toString() );
			}
		}
	}

	Dlg_CheckUpdate dlg( this );
	dlg.SetVer( CURRENT_VERSION, newVer );
	dlg.SetNotes( releaseNodes );

	dlg.exec();

	VS_CATCH;
}

void Versus::ProcessReceivedMessage( const QString& message )
{
	VS_TRY;

	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson( message.toUtf8(), &errorPtr );
	if( doc.isNull() || errorPtr.error != QJsonParseError::NoError )
	{
		return;
	}

	const QJsonObject rootObj = doc.object();

	if( !ReadJsonValue( rootObj, "updateMe" ).isEmpty() )
	{
		emit OnSendScoreboardData();
		return;
	}

	if( !ReadJsonValue( rootObj, "reset" ).isEmpty() )
	{
		emit OnResetScores();
		return;
	}
	if( !ReadJsonValue( rootObj, "sortasc" ).isEmpty() )
	{
		emit OnSortUp();
		return;
	}
	if( !ReadJsonValue( rootObj, "sortdesc" ).isEmpty() )
	{
		emit OnSortDown();
		return;
	}
	if( !ReadJsonValue( rootObj, "swap" ).isEmpty() )
	{
		emit OnSortSwap();
		return;
	}

	if( !ReadJsonValue( rootObj, "timerStart" ).isEmpty() )
	{
		emit OnTimerStart();
		return;
	}
	if( !ReadJsonValue( rootObj, "timerStop" ).isEmpty() )
	{
		emit OnTimerStop();
		return;
	}
	if( !ReadJsonValue( rootObj, "timerPause" ).isEmpty() )
	{
		emit OnTimerPause();
		return;
	}

	for( int i = 0; i < (int)_currentSet.size(); ++i )
	{
		const QString playerId = QString( "p%1score" ).arg( i + 1 );
		const QString p = ReadJsonValue( rootObj, playerId.toLatin1().data() );
		if( !p.isEmpty() )
		{
			double step = 0.0;

			if( p == "+" )
			{
				step = this->GetIncStep();
			}
			else if( p == "-" )
			{
				step = this->GetDecStep();
			}
			else
			{
				step = p.toDouble();
				if( step == 0.0 )
					step = this->GetIncStep();
			}

			EditScore( i + 1, step );

			return;
		}
	}

	VS_CATCH;
}

void Versus::AddSockets()
{
	VS_TRY;

	if( _socketServer.hasPendingConnections() )
	{
		while( QWebSocket* s = _socketServer.nextPendingConnection() )
		{
			QObject::connect( s, &QWebSocket::textMessageReceived, this, &Versus::ProcessReceivedMessage );

			_webSockets.push_back( s );
		}
	}

	VS_CATCH;
}

void Versus::OnEditHotkey()
{
	VS_TRY;

	Dlg_EditHotkey dlg( this );

	{
		dlg.SetDecStep( _decStep );
		dlg.SetIncStep( _incStep );

		for( HotkeyMap::const_iterator ii = _globalShortcuts.begin(); ii != _globalShortcuts.end(); ++ii )
		{
			UnregisterScoreHotkey( ii.value().get() );

			dlg.Set( ii.key(), ii.value()->shortcut() );
		}
		_globalShortcuts.clear();

		dlg.SetNumHotkeys( _numHotkeys );
	}

	dlg.exec();

	{
		_decStep = dlg.GetDecStep();
		_incStep = dlg.GetIncStep();

		constexpr int idsTimer[] = { TimerHotkey_Start, TimerHotkey_Pause, TimerHotkey_Stop };
		for( int id : idsTimer )
		{
			QKeySequence keySeq = dlg.Get( id );
			if( !keySeq.isEmpty() )
			{
				auto ii = _globalShortcuts.insert( id, std::make_shared< QxtGlobalShortcut>( keySeq ) );
				if( ii != _globalShortcuts.end() )
				{
					switch( id )
					{
						case TimerHotkey_Start:
							QObject::connect( ii.value().get(), &QxtGlobalShortcut::activated, this, &Versus::OnTimerStart );
							break;
						case TimerHotkey_Pause:
							QObject::connect( ii.value().get(), &QxtGlobalShortcut::activated, this, &Versus::OnTimerPause );
							break;
						case TimerHotkey_Stop:
							QObject::connect( ii.value().get(), &QxtGlobalShortcut::activated, this, &Versus::OnTimerStop );
							break;
						default:
							break;
					}
				}
			}
		}

		_numHotkeys = dlg.GetNumHotkeys();
		for( int i = 0; i < _numHotkeys; ++i )
		{
			const int idValues[2] = { i + 1, -i - 1 };
			for( int id : idValues )
			{
				QKeySequence keySeq = dlg.Get( id );
				RegisterScoreHotkey( id, keySeq );
			}
		}
	}

	VS_CATCH;
}

void Versus::OnEditSettings()
{
	VS_TRY;

	Dlg_Settings dlg( this, _localIP, _webServerPort, _tcpSocketPort, _autoStart );

	if( dlg.exec() == QDialog::Rejected )
		return;

	_localIP = dlg.GetLocalIP();
	_webServerPort = dlg.GetWebServerPort();
	_tcpSocketPort = dlg.GetTcpSocketPort();
	_autoStart = dlg.GetAutoStart();

	VS_CATCH;
}

int Versus::GetHTMLPort() const
{
	return _webServerPort;
}

int Versus::GetTCPPort() const
{
	return _tcpSocketPort;
}

const QString& Versus::GetLocalIP() const
{
	return _localIP;
}

void Versus::OnResetScores()
{
	VS_TRY;

	WaitCursor wait;

	for( PlayerItemData& pid : _currentSet )
	{
		pid.score = "0";
	}

	UpdatePlayerSet();

	emit OnSendScoreboardData();

	VS_CATCH;
}

void Versus::OnSortUp()
{
	VS_TRY;

	WaitCursor wait;

	ReadItems();

	::std::stable_sort( _currentSet.begin(), _currentSet.end(), []( const auto& lhs, const auto& rhs )->bool
	{
		return lhs.score.toDouble() < rhs.score.toDouble();
	} );

	UpdatePlayerSet();

	emit OnSendScoreboardData();

	VS_CATCH;
}

void Versus::OnSortDown()
{
	VS_TRY;

	WaitCursor wait;

	ReadItems();

	::std::stable_sort( _currentSet.begin(), _currentSet.end(), []( const auto& lhs, const auto& rhs )->bool
	{
		return lhs.score.toDouble() > rhs.score.toDouble();
	} );

	UpdatePlayerSet();

	emit OnSendScoreboardData();

	VS_CATCH;
}

void Versus::OnSortSwap()
{
	VS_TRY;

	WaitCursor wait;

	ReadItems();

	std::vector<PlayerItemData> reverseSet;
	reverseSet.reserve( _currentSet.size() );

	reverseSet.insert( reverseSet.end(), _currentSet.rbegin(), _currentSet.rend() );

	reverseSet.swap( _currentSet );

	UpdatePlayerSet();

	emit OnSendScoreboardData();

	VS_CATCH;
}

void Versus::UpdatePlayerDB() const
{
	VS_TRY;

	ReadItems();

	for( const auto& item : _currentSet )
	{
		PlayerManager::Instance().AddPlayer( item );
		PlayerManager::Instance().AddTeam( item );
	}

	VS_CATCH;
}

void Versus::OnSetPlayerGroup()
{
	VS_TRY;

	Dlg_SetPlayerGroup dlg( this );

	dlg.Init( _currentSet );

	if( dlg.exec() == QDialog::Rejected )
		return;

	auto& item = dlg._selectedItemData;

	QSignalBlocker blocker( ui.playerListBox );

	ui.playerListBox->clear();

	_currentSet.clear();

	for( const auto& ii : item.items )
	{
		auto& p = _currentSet.emplace_back();

		auto playerItem = PlayerManager::Instance().FindPlayerItem( ii.player, ii.team );
		if( playerItem )
		{
			p.player = playerItem->player;
			p.team = playerItem->team;
			p.playerLogo = playerItem->playerLogo;
			p.teamLogo = playerItem->teamLogo;
			p.score = ii.score;
		}
		else
		{
			p.player = ii.player;
			p.team = ii.team;
		}

		p.score = ii.score;
	}

	UpdatePlayerSet();

	VS_CATCH;
}

void Versus::timerEvent( QTimerEvent* event )
{
	if( event->timerId() == _timerId )
	{
		OnSendTimerData();
	}
}

void SendCurrentTime( QList<QWebSocket*>& webSockets, int currentTime )
{
	VS_TRY;

	QJsonObject obj;
	obj.insert( "updateView", (int)0 );

	QJsonArray byteData;

	Add( byteData, "timer_hms", QString::number( currentTime ) );

	obj.insert( "data", byteData );

	const QJsonDocument doc( obj );
	SendToSocket( webSockets, doc.toJson() );

	VS_CATCH;
}

void Versus::OnSendTimerData()
{
	VS_TRY;

	if( !_server )
		return;

	switch( _timerGroupIndex )
	{
		case 0: // countdown
		{
			--_currentTime;
			if( _currentTime == 0 )
			{
				_timerState = TimerEvents::Expired;
				SendTimerEvent( TimerEvents::Expired, -1 );

				OnTimerStop();
				return;
			}
			break;
		}
		case 1: // stopwatch
		{
			++_currentTime;
			if( _currentTime == _totalTime )
			{
				_timerState = TimerEvents::Expired;
				SendTimerEvent( TimerEvents::Expired, -1 );

				OnTimerStop();
				return;
			}
			break;
		}
		case 2:
		{
			if( _timerId )
			{
				const QDateTime current = QDateTime::currentDateTime();
				const QTime time = current.time();
				_currentTime = time.hour() * 60 * 60 + time.minute() * 60 + time.second();
			}
			else
			{
				_currentTime = 0;
			}
			break;
		}
		default:
		{
			return;
		}
	}

	SendCurrentTime( _webSockets, _currentTime );

	VS_CATCH;
}

void Versus::SendTimerEvent( TimerEvents e, int totalTime )
{
	VS_TRY;

	if( !_server )
		return;

	QJsonObject obj;
	obj.insert( "updateView", (int)0 );

	QJsonArray byteData;

	if( totalTime >= 0 )
	{
		Add( byteData, "timer_total", QString::number( totalTime ) );
	}

	Add( byteData, "timer_event", QString::number( (int)e ) );


	obj.insert( "data", byteData );

	const QJsonDocument doc( obj );
	SendToSocket( _webSockets, doc.toJson() );

	VS_CATCH;
}

void Versus::OnTimerStart()
{
	VS_TRY;

	if( _timerId == 0 )
	{
		_timerGroupIndex = ui.tabTimers->currentIndex();

		_timerId = startTimer( std::chrono::seconds( 1 ), Qt::VeryCoarseTimer );

		switch( _timerGroupIndex )
		{
			case 0: // countdown
			{
				if( _timerState != TimerEvents::Pause )
				{
					const QTime time = ui.timeCountdown->time();
					_totalTime = time.hour() * 3600 + time.minute() * 60 + time.second();
					_currentTime = _totalTime;
				}
				break;
			}
			case 1: // stopwatch
			{
				if( _timerState != TimerEvents::Pause )
				{
					const QTime time = ui.timeStopwatch->time();
					_totalTime = time.hour() * 3600 + time.minute() * 60 + time.second();
					_currentTime = 0;
				}
				break;
			}
			case 2: // system clock
			{
				_totalTime = 24 * 60 * 60; // sec per day

				const QDateTime current = QDateTime::currentDateTime();
				const QTime time = current.time();
				_currentTime = time.hour() * 60 * 60 + time.minute() * 60 + time.second();

				break;
			}
			default:
			{
				break;
			}
		}

		if( _timerState == TimerEvents::Pause )
		{
			SendTimerEvent( TimerEvents::Resume, -1 );
		}
		else
		{
			SendTimerEvent( TimerEvents::Start, _totalTime );
		}

		_timerState = TimerEvents::Start;

		SendCurrentTime( _webSockets, _currentTime );
	}

	ui.btnTimerStart->setDisabled( true );
	ui.btnTimerPause->setDisabled( false );
	ui.btnTimerStop->setDisabled( false );

	VS_CATCH;
}

void Versus::OnTimerPause()
{
	VS_TRY;

	if( _timerId )
	{
		killTimer( _timerId );
		_timerId = 0;

		_timerState = TimerEvents::Pause;

		SendTimerEvent( TimerEvents::Pause, -1 );
		SendCurrentTime( _webSockets, _currentTime );

		ui.btnTimerStart->setDisabled( false );
		ui.btnTimerPause->setDisabled( true );
		ui.btnTimerStop->setDisabled( false );
	}

	VS_CATCH;
}

void Versus::OnTimerStop()
{
	VS_TRY;

	if( _timerId )
	{
		killTimer( _timerId );
		_timerId = 0;
	}

	if( _timerState != TimerEvents::Expired )
	{
		switch( _timerGroupIndex )
		{
			case 0: // countdown
			{
				_currentTime = _totalTime;
				break;
			}
			case 1: // stopwatch
			{
				_currentTime = 0;
				break;
			}
			case 2: // system clock
			{
				break;
			}
			default:
			{
				break;
			}
		}
	}

	_timerState = TimerEvents::Stop;

	SendCurrentTime( _webSockets, _currentTime );
	SendTimerEvent( TimerEvents::Stop, -1 );

	ui.btnTimerStart->setDisabled( false );
	ui.btnTimerPause->setDisabled( true );
	ui.btnTimerStop->setDisabled( true );

	VS_CATCH;
}

double Versus::GetIncStep() const
{
	return _incStep;
}

double Versus::GetDecStep() const
{
	return _decStep;
}
