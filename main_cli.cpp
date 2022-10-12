#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineParser>
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

    QCommandLineParser parser;
    parser.setApplicationDescription("MBSeedKey CLI");
    parser.addHelpOption();
    parser.addVersionOption();


    parser.addOptions({
        {{"i", "info"}, QCoreApplication::translate("main", "Print information from seed-key dll(ECU Name, Comments, Access Levels, Seed/Key sizes")},
        {
            {"d", "target-dll"},
            QCoreApplication::translate("main", "DLL for seed-key generation"),
            QCoreApplication::translate("main", "SeedKey DLL")
        },
        {
            {"s", "seed"},
            QCoreApplication::translate("main", "Seed for key generation in HEX: FFABEE"),
            QCoreApplication::translate("main", "Seed")
        },
        {
            {"l", "level"},
            QCoreApplication::translate("main", "Access Level for key generation in HEX: 0B"),
            QCoreApplication::translate("main", "Access Level")
        },
     });

    // Process the actual command line arguments given by the user
    parser.process(app);

    bool info = parser.isSet("i");
    auto targetDll = parser.value("d");
    auto seed = parser.value("s");
    auto level = parser.value("l");
    qDebug() << info << targetDll;

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
