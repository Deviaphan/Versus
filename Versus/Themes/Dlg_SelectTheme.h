#pragma once

#include <QDialog>
#include "ThemeInfo.h"
#include <vector>

class QMouseEvent;

namespace Ui { class Dlg_SelectTheme; };

class Dlg_SelectTheme : public QDialog
{
	Q_OBJECT

public:
	Dlg_SelectTheme(QWidget *parent = Q_NULLPTR);
	~Dlg_SelectTheme();
	void Init( const QString& uid );

protected:
	void mousePressEvent( QMouseEvent* event ) override;

public:
	ThemeInfo GetThemeInfo() const;

private:
	Ui::Dlg_SelectTheme * ui;

private slots:
	void OnChangeTheme();

private:
	void FindThemes();
	void ReloadPreview();

private:
	std::vector<ThemeInfo> _themes;

	QString _lastUsed;

};
