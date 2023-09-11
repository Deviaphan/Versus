#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dlg_SettingsClass; };
QT_END_NAMESPACE

class QMouseEvent;
class Versus;

class Dlg_Settings : public QDialog
{
	Q_OBJECT

public:
	Dlg_Settings( Versus * parent, const QString & ip, int webPort, int tcpPort, bool autoStart );
	~Dlg_Settings();

public:
	QString GetLocalIP() const;
	int GetWebServerPort() const;
	int GetTcpSocketPort() const;
	bool GetAutoStart() const;

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private:
	Ui::Dlg_SettingsClass *ui;

	Versus* _base;

};
