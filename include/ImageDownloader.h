#ifndef URLDOWNLOADER_H
#define URLDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QObject>

class ImageDownloader : public QObject
{
    Q_OBJECT
  public:
    explicit ImageDownloader(QObject *parent = nullptr);
    void tryDownloadImage(const QUrl &url);

  signals:
    void downloadFinishedSuccessfully(const QImage &image, const QUrl &sourceUrl);
    void downloadFailed(const QUrl &sourceUrl);

  private:
    QNetworkAccessManager networkAccessManager;
    void onDownloadFinished(QNetworkReply *reply);
};

#endif
