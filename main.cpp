#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QtCore>
#include "panelmodel.h"
#include "cartridgemodel.h"
#include "ramiProtocol.h"
#include "connection.h"
#define __WINMEDIA_DEBUG


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

#ifdef __linux__
    const QString DRIVER = "ODBC Driver 13 for SQL Server";
#elif _WIN32
    const QString DRIVER = "SQL Server";
#endif

//TODO: to be replaced by data provided in a file
//    const QString SERVER = "193.253.53.24";
//    const QString PORT = "1437";
//    const QString USER = "WinBizz";
//    const QString PASSWORD = "WinBizz2012";
    const QString SERVER = "127.0.0.1";
    const QString PORT = "1433";
    const QString USER = "test";
    const QString PASSWORD = "test";
    const QString NAME = "CartridgeApplication";

    //TODO: wait for the connection in a thread
    Connection connection("127.0.0.1",1234);

#ifdef __WINMEDIA_DEBUG
    //Show available drivers
    QStringList drivers = QSqlDatabase::drivers();
    QStringList::const_iterator constIter;
    qDebug() << "Available drivers:";
    for(constIter=drivers.constBegin(); constIter != drivers.constEnd(); constIter++){
        qDebug() << (*constIter).toLocal8Bit().constData();
    }

    //Test the encryption and decryption protocole for RAMI cartridge
    RamiProtocol::Params params;
    params.column=1;
    params.row=6;
    params.state=1;
    RamiProtocol::Data data=RamiProtocol::encrypt(params);
    RamiProtocol::print(data);
    auto result = RamiProtocol::decrypt(data);
    RamiProtocol::print(result);

//    connection.send(2,1,1);
#endif

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(
                "Driver={"+DRIVER+"};\
                Server="+SERVER+","+PORT+";\
                Uid="+USER+";\
                Pwd="+PASSWORD+";"
    );

    if(db.open()){
        qDebug() << "Opening of DB successful";
        QSqlQuery query(QString("SELECT Name,Ip"
                                "FROM [WinMedia].[dbo].[Computer]"
                                "WHERE Name == %1").arg(NAME));
//        if(query.size()<=0){
//            QSqlQuery insert(QString("INSERT INTO [WinMedia].[dbo].[Computer] (Name,Ip,Modify,X,Y)"
//                                     "VALUES ("+NAME+","+","+"0,0"
//                                     )
//                             );
//        }
    }
    else
        qDebug() << db.lastError();

    qmlRegisterType<CartridgeModel>("org.winmedia.guiennet",1,0,"CartridgeModel");
    qmlRegisterType<PanelModel>("org.winmedia.guiennet",1,0,"PanelModel");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    QObject* rootObject=engine.rootObjects()[0];
    QObject::connect(rootObject,SIGNAL(playerCommand(int,int,bool)),&connection,SLOT(send(int,int,bool)));


    return app.exec();
}
