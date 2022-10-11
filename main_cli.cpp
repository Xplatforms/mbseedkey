#include <QCoreApplication>
#include <QDebug>
#include "exutils.h"
#include "ecuseedkeydll.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName("Xplatforms");
    app.setOrganizationDomain("mbseedkey");

    auto ex_utils = new ExUTILS(&app);

    auto args = app.arguments();
    args.removeFirst();

    if(args.size() <= 0)
    {
        qDebug() << "Found dlls in folder: " << ex_utils->foldername();

    }





    /*
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
    */

    return app.exec();
}
