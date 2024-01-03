#pragma once

#include "ui_Versus.h"

#include <QList>
#include <QMap>
#include <QWebSocketServer>
#include <QtHttpServer/qhttpserver.h>
#include <QtWidgets/QDialog>
#include <QTranslator>

#include "PlayerItemData.h"
#include "SharedTools.h"
#include "Hotkey/qxtglobalshortcut.h"
#include "Server/Downloader.h"
#include "Themes/ThemeInfo.h"

class QComboBox;

void SendToSocket( QList<QWebSocket*>& webSockets, const QString& json );


class Versus : public QDialog
{
	Q_OBJECT

	enum class TimerEvents
	{
		None = 0,
		Start = 1,
		Pause = 2,
		Resume = 3,
		Stop = 4,
		Expired = 5
	};

public:
	explicit Versus(QWidget *parent = Q_NULLPTR);
    ~Versus() override;

	void InitUI();
	void Init();

public slots:
    void OnSelectTheme();
    void OnSendScoreboardData();
	void OnSendTimerData();

	void OnStartStopServer();
	void AddSockets();

	void OnEditHotkey();

	void OnEditSettings();

	void OnResetScores();
	void OnSortUp();
	void OnSortDown();
	void OnSortSwap();

	void OnSetPlayerGroup();

	void OnTimerStart();
	void OnTimerPause();
	void OnTimerStop();

	void OnEditTournamentTitle();
	void OnEditStage();
	void OnEditBestOf();

	void OnCopyServerToClipboard();

public:
	void ProcessReceivedMessage( const QString& message );

	void UpdatePlayerDB() const;
	void SendTimerEvent( TimerEvents e, int totalTime );

	double GetIncStep() const;
	double GetDecStep() const;

protected:
	void mousePressEvent( QMouseEvent* event ) override;
	void moveEvent( QMoveEvent* event ) override;
	void keyPressEvent( QKeyEvent* e ) override;
	void timerEvent( QTimerEvent* event ) override;

protected:
	int GetHTMLPort() const;
	int GetTCPPort() const;
	const QString& GetLocalIP() const;

	void CheckVersion();

	void StartServer();
	void StopServer();


	void UpdatePlayerSet();
	void ReadItems() const;

protected:
	void EditScore( int id, double step );

    void LoadSettings();
    void SaveSettings();

    void LoadTheme();

	void UpdatePlayerColors();

	void RegisterScoreHotkey( int id, const QKeySequence& keySeq );
	void UnregisterScoreHotkey( const QxtGlobalShortcut* shortcut );

private:
    Ui::VersusClass ui{};

protected:
	QTranslator _translator;

	::std::unique_ptr<QHttpServer> _server;

	Downloader _downloader;

	QWebSocketServer _socketServer;
	QList<QWebSocket*> _webSockets;


	ThemeInfo _themeInfo;

	using HotkeyMap = QMap<int, std::shared_ptr<QxtGlobalShortcut> >;
	HotkeyMap _globalShortcuts;
	int _numHotkeys;
	double _decStep;
	double _incStep;

	QString _localIP;
	int _webServerPort;
	int _tcpSocketPort;

	bool _autoStart;

	std::vector<PlayerItemData> _currentSet = {};

	int _timerId;
	int _totalTime;
	int _currentTime;
	TimerEvents _timerState;

	int _usedTimerIndex;
	QString _countdown;
	QString _stopwatch;

	bool _stickyEdges;

	QString _lastTitle;
	QString _lastSub1;
	QString _lastSub2;

	QString _selectedLanguage;
	::std::set<LangData> _langs;
};

