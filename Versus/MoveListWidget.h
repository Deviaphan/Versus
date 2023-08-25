#pragma once

#include <QListWidget>

class MoveListWidget  : public QListWidget
{
	Q_OBJECT

public:
	MoveListWidget(QWidget *parent);
	~MoveListWidget();

	void mousePressEvent( QMouseEvent* event ) override;

};
