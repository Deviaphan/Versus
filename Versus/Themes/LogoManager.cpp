// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "LogoManager.h"
#include "Dlg_SelectLogo.h"

QString LogoManager::lastPlayerLogoPack;
QString LogoManager::lastTeamLogoPack;

LogoManager& LogoManager::Instance()
{
	static LogoManager lm;
	return lm;
}

QString LogoManager::FindLogo( QWidget * parent, bool playerLogo )
{
	QString & lastFolder =  playerLogo ? lastPlayerLogoPack : lastTeamLogoPack;

	Dlg_SelectLogo dlg(parent);
	dlg.Init( lastFolder);

	if( dlg.exec() == QDialog::Rejected )
		return "";

	QString fileName;
	dlg.GetFileName( lastFolder, fileName );
	
	return "versus_logo/" +lastFolder + "/" + fileName;
}
