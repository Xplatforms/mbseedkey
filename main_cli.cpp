#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
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

    if(info)
    {
        if(targetDll.isEmpty()){qDebug() << QCoreApplication::translate("main", "DLL not set!"); return 0;}
        QFileInfo info(targetDll);
        if(!info.isFile() || !info.isReadable())
        {
            qDebug() << QCoreApplication::translate("main", "can't read DLL or wrong filepath");
            return 0;
        }

        auto ecu = new ECUSeedKeyDLL(targetDll);
        qDebug() << ecu->ECUName();
        qDebug() << "Access Levels" << ecu->AccessTypesString();
        foreach(auto acc, ecu->AccessTypes())
        {
            qDebug() << "Access LeveL: " << QStringLiteral("%1 ").arg(acc, 2, 16, QLatin1Char('0')).toUpper() << "seed length " << ecu->seedLength(acc) << " key length " << ecu->keyLength(acc);
        }
    }


    return app.exec();
}
