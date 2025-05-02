#ifndef MYLOCALSERVER_H
#define MYLOCALSERVER_H

#include <QLocalServer>
#include <QLocalSocket>
#include "common.h"

class MyLocalServer : public QObject
{
    Q_OBJECT

public:
    MyLocalServer(QString serverName, QObject* parent = 0);
    ~MyLocalServer();

    QLocalServer           *m_localServer{};
    QLocalSocket           *m_chartSocket{};
    QVector<chartData_t>    m_chartDataVec{};

    // вектор данных для окон массивов
    // QVector<arrayData_t>    m_arrayDataVec{};
    QVector<curData_t>   m_arrayDataVec{};

    arrayDlgProp_t          m_prop;

    void addSocket(QLocalSocket *socket, const QString &name);
    void handleSocketDisconnected(QLocalSocket *socket);

    // Метод для отправки клиенту данных
    void sendToClient(QLocalSocket* localSocket, const QString &string);
    void newChartData(QVector<chartData_t> &vec);
    // void newArrayData(int index, QVector<arrayData_t> &vec);
    void newArrayData(int index, QVector<curData_t> &vec);
    void setArrayName(int index);

#define STR_CHART_CLIENT    "DlgChart"
#define STR_CHART_ID        "ChartID"
#define STR_ARRAY_CLIENT    "DlgArray"
#define STR_ARRAY_ID        "ArrayID"
#define STR_ARRAY_PROP      "ArrayProperty"

private:
    // QLocalServer* m_localServer{};
    // Переменная для хранения размера получаемого от клиента блока
    quint16 m_nextBlockSize = 0;

public slots:
    // Слот обработки нового клиентского подключения
    virtual void slotNewConnection();
    // Слот чтения информации от клиента
    void slotReadClient();
    // void slotStateChanged();//QAbstractSocket::SocketState socketState);

signals:
    void signalUpdateDataFromSocket(QString &message);
    void signalCloseDlgArr(int index);

// protected:
//      void incomingConnection (quintptr socketDescriptor);
};

#endif // MYLOCALSERVER_H
