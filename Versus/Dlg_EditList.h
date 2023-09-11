#pragma once

#include <QDialog>
#include <QtWidgets/QDialog>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Dlg_EditListClass;
};
QT_END_NAMESPACE

class QMouseEvent;

class Dlg_EditList: public QDialog
{
	Q_OBJECT

public:
	Dlg_EditList( QWidget* parent );
	~Dlg_EditList();

public:
	void UpdateList();
	int GetSelectedIndex() const;

public slots:
	void OnAddLine();
	void OnRemoveLine();

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private:
	Ui::Dlg_EditListClass* ui;

public:
	::std::vector<QString> items;
};
