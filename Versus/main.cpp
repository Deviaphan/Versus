// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "stdafx.h"
#include "Versus.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	a.setWindowIcon( QIcon( ":/Versus/WndIcon" ) );

    Versus w;
    w.Init();
    w.show();

    return a.exec();
}
