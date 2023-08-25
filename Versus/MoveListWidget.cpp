// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "MoveListWidget.h"
#include <QMouseEvent>
#include <QWindow>
#include <Windows.h>

MoveListWidget::MoveListWidget(QWidget *parent)
	: QListWidget(parent)
{}

MoveListWidget::~MoveListWidget()
{}

void MoveListWidget::mousePressEvent( QMouseEvent* event )
{
	if( event->buttons().testFlag( Qt::LeftButton ) )
	{
		HWND hWnd = ::GetAncestor( (HWND)(window()->windowHandle()->winId()), GA_ROOT );
		POINT pt;
		::GetCursorPos( &pt );
		::ReleaseCapture();
		::SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS( pt ) );
	}

	QListWidget::mousePressEvent( event );
}
