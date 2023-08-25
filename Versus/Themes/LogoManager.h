#pragma once
#include "QString"

class QWidget;

class LogoManager
{
private:
	LogoManager() = default;
	~LogoManager() = default;

public:
	LogoManager( const LogoManager& ) = delete;
	LogoManager( const LogoManager&& ) = delete;
	LogoManager& operator = ( const LogoManager& ) = delete;
	LogoManager& operator = ( const LogoManager&& ) = delete;

public:
	static LogoManager & Instance();

public:

	static QString FindLogo( QWidget * parent, bool playerLogo );

public:
	static QString lastPlayerLogoPack;
	static QString lastTeamLogoPack;

};
