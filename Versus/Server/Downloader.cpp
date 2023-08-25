// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Downloader.h"

Downloader::Downloader( QObject * parent ): QObject( parent )
{
	_manager = std::make_unique<QNetworkAccessManager>();
	// ... and connect the signal to the handler 
	connect( _manager.get(), &QNetworkAccessManager::finished, this, &Downloader::onResult );
}

void Downloader::GetData( const QUrl & url )
{
	QNetworkRequest request;
	request.setUrl( url );
	_manager->get( request );
}

void Downloader::onResult( QNetworkReply * reply )
{
	if( reply->error() )
	{
		// We inform about it and show the error information
		qDebug() << "ERROR";
		qDebug() << reply->errorString();
	}
	else
	{
		_data = reply->readAll();

		qDebug() << "Downloading is completed";
		emit onReady(); // Sends a signal to the completion of the receipt of the file
	}
}