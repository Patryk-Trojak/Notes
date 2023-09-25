#ifndef NOTECONTENTEDIT_H
#define NOTECONTENTEDIT_H
#include "ImageDownloader.h"
#include <QTextEdit>

class NoteContentEdit : public QTextEdit
{
    Q_OBJECT

  public:
    NoteContentEdit(QWidget *parent = nullptr);
    QVariant loadResource(int type, const QUrl &name);

    using ResourceSaver = std::function<int(const QVariant &, QTextDocument::ResourceType)>;
    using ResourceLoader = std::function<QVariant(const QUrl &, QTextDocument::ResourceType)>;

    void setResourceSaver(const ResourceSaver &resourceSaver);
    void setResourceLoader(const ResourceLoader &resourceLoader);

  protected:
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);
    ResourceSaver resourceSaver;
    ResourceLoader resourceLoader;

  private:
    ImageDownloader urlDownloader;
    void insertImage(const QVariant &imageData);
    void onImageDownloaded(const QImage &image, const QUrl &sourceUrl);
    void onImageDownloadedFailed(const QUrl &sourceUrl);
};

#endif // NOTECONTENTEDIT_H
