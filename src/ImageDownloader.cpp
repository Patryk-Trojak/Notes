#include "ImageDownloader.h"
#include <QImage>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

ImageDownloader::ImageDownloader(QObject *parent) : QObject{parent}
{
    QObject::connect(&networkAccessManager, &QNetworkAccessManager::finished, this,
                     &ImageDownloader::onDownloadFinished);
}

void ImageDownloader::tryDownloadImage(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    networkAccessManager.get(request);
}

void ImageDownloader::onDownloadFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QImage image;
    image.loadFromData(reply->readAll());
    if (image.isNull())
        emit this->downloadFailed(reply->url());
    else
        emit this->downloadFinishedSuccessfully(image, reply->url());
}
