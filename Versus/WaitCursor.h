#pragma once

#include <QtWidgets/QApplication>

class WaitCursor
{
public:
	WaitCursor()
	{
		QApplication::setOverrideCursor( Qt::WaitCursor );
	}

	WaitCursor( const WaitCursor& ) = delete;
	WaitCursor( const WaitCursor&& ) = delete;
	WaitCursor& operator=( const WaitCursor& ) = delete;
	WaitCursor& operator=( const WaitCursor&& ) = delete;

	~WaitCursor()
	{
		QApplication::restoreOverrideCursor();
	}
};
