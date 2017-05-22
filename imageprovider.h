#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();

public:
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) Q_DECL_OVERRIDE;
};

#endif // IMAGEPROVIDER_H
