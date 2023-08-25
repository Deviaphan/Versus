#pragma once

#include <QDialog>
#include <unordered_map>
#include <vector>
#include "HotkeyItemWidget.h"

namespace Ui
{
	class Dlg_EditHotkey;
};

class QMouseEvent;
class QKeySequenceEdit;

class HotkeyItemWidget;

enum TimerHotkeys
{
	TimerHotkey_Offset = 10000,
	TimerHotkey_Start = TimerHotkey_Offset,
	TimerHotkey_Pause,
	TimerHotkey_Stop
};

class Dlg_EditHotkey: public QDialog
{
	Q_OBJECT

public:
	Dlg_EditHotkey( QWidget* parent );
	~Dlg_EditHotkey();

	void SetNumHotkeys( int numHotkeys );

	void Set( int playerId, const QKeySequence& ks );
	QKeySequence Get( int playerId ) const;

	int GetNumHotkeys() const
	{
		return _numHotkeys;
	}

	void SetDecStep( double step );
	void SetIncStep( double step );
	double GetDecStep() const;
	double GetIncStep() const;

public slots:
	void accept() override;

protected:
	void mousePressEvent( QMouseEvent* event ) override;
	void keyPressEvent( QKeyEvent* e ) override;

private:
	Ui::Dlg_EditHotkey* ui;

	//QKeySequenceEdit* _hotDec[4];
	//QKeySequenceEdit* _hotInc[4];

	QKeySequenceEdit* _timer[3];

	std::vector< HotkeyItemWidget*> _list;

	std::unordered_map<int, QKeySequence> _scoreHotkeys;
	int _numHotkeys;
};
