#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include "exutils.h"
#include "ecuseedkeydll.h"

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

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
        qStdOut() << "Found dlls in folder: " << ex_utils->foldername();
        return 0;
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
            {"a", "access-level"},
            QCoreApplication::translate("main", "Access Level for key generation in HEX: 0B"),
            QCoreApplication::translate("main", "Access Level")
        },
        {
            {"l", "key-length"},
            QCoreApplication::translate("main", "Key length (int) if default length can't be read from dll"),
            QCoreApplication::translate("main", "Key Length")
        },
     });

    // Process the actual command line arguments given by the user
    parser.process(app);

    bool info = parser.isSet("i");
    auto targetDll = parser.value("d");
    auto seed = parser.value("s");
    auto level = parser.value("a");
    auto keylength = parser.value("l");

    if(info)
    {
        if(targetDll.isEmpty()){qDebug() << QCoreApplication::translate("main", "DLL not set!"); return 0;}
        QFileInfo dll(targetDll);
        if(!dll.isFile() || !dll.isReadable())
        {
            qStdOut() << QCoreApplication::translate("main", "can't read DLL or wrong filepath") << "\n";
            return 0;
        }

        auto ecu = new ECUSeedKeyDLL(targetDll);
        qDebug() << ecu->ECUName();
        foreach(auto acc, ecu->AccessTypes())
        {
            qStdOut() << "Access Level: " << QStringLiteral("%1 ").arg(acc, 2, 16, QLatin1Char('0')).toUpper()
                      << "seed length " << ecu->seedLength(acc) << " key length " << ecu->keyLength(acc) << "\n";
        }
        return 0;
    }

    if(targetDll.isEmpty())
    {
        qStdOut() << "run this programm with --help argument to see all cmd line switches" << "\n";
        return 0;
    }

    if(seed.isEmpty())
    {
        qStdOut() << "you need to set at target dll, seed and access level for key generation. run this programm with --help to see all cmd line switches\n";
        return 0;
    }

    QFileInfo dll(targetDll);
    if(!dll.isFile() || !dll.isReadable())
    {
        qStdOut() << QCoreApplication::translate("main", "can't read DLL or wrong filepath") << "\n";
        return 0;
    }

    auto ecu = new ECUSeedKeyDLL(targetDll);
    qint32 access_level = level.toInt(Q_NULLPTR, 16);
    qint32 key_length = keylength.isEmpty()?0:keylength.toInt(Q_NULLPTR, 10);

    if(key_length <= 0)key_length = ecu->keyLength(access_level);

    QByteArray datas_hex = seed.toLocal8Bit();
    QList<qint32> result;
    result.reserve(datas_hex.size() / 2);

    for (int i = 0; i < datas_hex.size(); i += 2)
        result.push_back(QString::fromLatin1(datas_hex.data() + i, 2).toInt(Q_NULLPTR,16));


    //qStdOut() << "seed " << seed << " acc " << access_level << " key len " << key_length << " len arg " << keylength << "\n";
    foreach(auto k, ecu->generateKeyFromSeed(result, access_level, key_length))
    {
        qStdOut() << QStringLiteral("%1 ").arg(k, 2, 16, QLatin1Char('0')).toUpper();
    }




    return 0;
}
