#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "exutils.h"
#include "ecuseedkeydll.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("mbseedkey");
    app.setOrganizationDomain("mbseedkey");

    qmlRegisterType<ExUTILS>("xplatforms.mbseedkey.exutils", 1, 0, "ExUTILS");
    qmlRegisterType<ECUSeedKeyDLL>("xplatforms.mbseedkey.ecuseedkeydll", 1, 0, "ECUSeedKeyDLL");

    QQmlApplicationEngine engine;    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
