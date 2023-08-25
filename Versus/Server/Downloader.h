#pragma once
#include <qobject.h>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <memory>

class Downloader: public QObject
{
	Q_OBJECT
public:
	explicit Downloader( QObject * parent = 0 );

	const QByteArray & GetResult() const
	{
		return _data;
	}

signals:
	void onReady();

public slots:
	void GetData( const QUrl & url );
	void onResult( QNetworkReply * reply );

private:
	std::unique_ptr<QNetworkAccessManager> _manager;

	QByteArray _data;
};

