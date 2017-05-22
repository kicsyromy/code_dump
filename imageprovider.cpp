#include "imageprovider.h"

#include <QImage>
#include <QByteArray>
#include <QBuffer>

ImageProvider::ImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ImageProvider::requestImage(const QString &, QSize *, const QSize &)
{
    QImage image(QStringLiteral("resources/image.png"));
    QByteArray imageData;
    QBuffer temp(&imageData);
    temp.open(QIODevice::WriteOnly);
    image.save(&temp, "PNG");
    uchar *buffer = new uchar[imageData.size()];
    memcpy(buffer, imageData.data(), imageData.size());
    return QImage::fromData(buffer, imageData.size(), "PNG");
}
