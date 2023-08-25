#pragma once

#include <QDialog>
#include <vector>


class QMouseEvent;

namespace Ui {
	class Dlg_SelectLogo;
};

class Dlg_SelectLogo : public QDialog
{
	Q_OBJECT

public:
	Dlg_SelectLogo(QWidget *parent = Q_NULLPTR);
	~Dlg_SelectLogo() override;

	void GetFileName( QString & packName, QString & imageName ) const;

	void Init( const QString& pack );

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private slots:
	void OnChangeCombo( int );

private:
	void InitLogoPacks();
	void UpdateLogoList();

private:
	Ui::Dlg_SelectLogo * ui;
	std::vector<QString> _filename;

	QString _lastUsedLogoPack;
};
