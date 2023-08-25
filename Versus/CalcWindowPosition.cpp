// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "CalcWindowPosition.h"
#include <QWidget>
#include <QApplication>
#include <QScreen>

void CalcWindowPosition( int posX, int posY, int windowWidth, int windowHeight, QWidget * wnd )
{
	QRect desktopRect;
	auto screens = QApplication::screens();
	for( auto& ii : screens )
	{
		QRect sr = ii->availableGeometry();
		if( sr.contains( posX, posY ) )
		{
			desktopRect = sr;
			break;
		}
	}

	if( desktopRect.width() <= 0 )
	{
		posX = 0;
		posY = 0;
	}
	else
	{
		if( posX < desktopRect.left() )
			posX = desktopRect.left();
		if( posY < desktopRect.top() )
			posY = desktopRect.top();

		if( posX > ( desktopRect.right() - windowWidth ) )
			posX = (desktopRect.right() - windowWidth);
		if( posY > ( desktopRect.bottom() - windowHeight ) )
			posY = (desktopRect.bottom() - windowHeight);
	}

	wnd->move( posX, posY );
}
