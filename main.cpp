#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlComponent>

#include "imageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickWindow view;
    QQmlEngine engine;
    engine.addImageProvider(QStringLiteral("resources"), new ImageProvider);

    QQmlComponent component(&engine, QStringLiteral("qrc:/test.qml"));
    QQuickItem *item = static_cast<QQuickItem *>(component.create());

    view.setWidth(item->width());
    view.setHeight(item->height());
    item->setParentItem(view.contentItem());

    QObject::connect(&view, &QQuickWindow::widthChanged,
                     [&]() { item->setWidth(view.width()); });
    QObject::connect(&view, &QQuickWindow::heightChanged,
                     [&]() { item->setHeight(view.height()); });

    view.show();

    return app.exec();
}
