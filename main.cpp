#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "form.h"
#include <QQmlContext>
#include <QQmlComponent>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    Form form;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("form", &form);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);
    form.setEngine(&engine);

    return app.exec();
}
