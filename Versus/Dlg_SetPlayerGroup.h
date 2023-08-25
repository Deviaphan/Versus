#pragma once

#include <QDialog>
#include "PlayerItemData.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Dlg_SetPlayerGroupClass;
};
QT_END_NAMESPACE

class QMouseEvent;
class PlayerGroupItem;

class Dlg_SetPlayerGroup: public QDialog
{
	Q_OBJECT

public:
	Dlg_SetPlayerGroup( QWidget* parent );
	~Dlg_SetPlayerGroup();

public slots:
	void accept() override;
	void OnSaveGroup();
	void OnDeleteItem( const Group* item );

public:
	void Init( const std::vector<PlayerItemData>& playerSet );
	void UpdateList();
	void SetCurrentItem( PlayerGroupItem* pli, const Group* item );

protected:
	void mousePressEvent( QMouseEvent* event ) override;

private:
	Ui::Dlg_SetPlayerGroupClass* ui;
	PlayerGroupItem* _selectedItem;

	const std::vector<PlayerItemData>* _currentPlayerSet;

public:
	Group _selectedItemData;
};
