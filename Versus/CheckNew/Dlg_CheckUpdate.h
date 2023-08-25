#pragma once

#include <QDialog>

#include "Version.h"

namespace Ui { class Dlg_CheckUpdate; };

class Dlg_CheckUpdate : public QDialog
{
	Q_OBJECT

public:
	Dlg_CheckUpdate(QWidget *parent = Q_NULLPTR);
	~Dlg_CheckUpdate();
	
	void SetVer( const QString& oldVer, const QString& newVer );
	void SetNotes( const QVector<QString>& notes );

private:
	Ui::Dlg_CheckUpdate *ui;
};
