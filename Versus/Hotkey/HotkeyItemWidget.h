#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class HotkeyItemWidgetClass; };
QT_END_NAMESPACE

class QKeySequence;

class HotkeyItemWidget : public QWidget
{
	Q_OBJECT

public:
	HotkeyItemWidget( QWidget* parent, const QKeySequence& inc, const QKeySequence& dec );
	~HotkeyItemWidget();

public:
	void SetTitle( int index );

	QKeySequence GetInc() const;
	QKeySequence GetDec() const;

private:
	Ui::HotkeyItemWidgetClass *ui;
};
