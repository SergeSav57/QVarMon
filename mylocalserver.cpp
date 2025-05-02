#include "mylocalserver.h"
#include "common.h"
#include <QMessageBox>
#include <QAbstractSocket>
#include <QThread>
#include "mainwindow.h"

MainWindow  *m_mainWnd_4Server;

MyLocalServer::MyLocalServer(QString serverName, QObject* parent)
    : QObject(parent)
{
    m_mainWnd_4Server = static_cast<MainWindow *>(parent);

    // qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState") ;
    // Создаём и запускаем сервер командой listen.
    m_localServer = new QLocalServer(this);
    if(!m_localServer->listen(serverName)) {
        QMessageBox::critical(0, "Server error",
                              "Unable to start server:" + m_localServer->errorString());
        m_localServer->close();
        return;
    }

    // Соединяем сигнал сервера о наличии нового подключения с обработчиком нового клиентского подключения
    connect(m_localServer, &QLocalServer::newConnection, this, &MyLocalServer::slotNewConnection);
    // сигнал после обрыва связи с окном массива (удаляет параметры окна
    // из списка и посылает команду девайсу на запрет сканирования массива)
    connect(this, &MyLocalServer::signalCloseDlgArr, m_mainWnd_4Server, &MainWindow::slotCloseDlgArr);

}

MyLocalServer::~MyLocalServer()
{
}

void MyLocalServer::addSocket(QLocalSocket *socket, const QString &name)
{
    socket->setProperty("socketName", name);
    connect(socket, &QLocalSocket::disconnected, this, [this, socket]() {
        handleSocketDisconnected(socket);});
    if(name == STR_CHART_ID)
        m_chartSocket = socket;
    else {
        int sz = (int)m_mainWnd_4Server->m_arrDlgPropVector.size();
        // последний член вектора свойств уже добавлен в MainWindow в ф-ции startDlgArray()
        m_mainWnd_4Server->m_arrDlgPropVector[sz - 1].socket = socket;
        setArrayName(sz - 1);
    }
}

void MyLocalServer::handleSocketDisconnected(QLocalSocket *socket)
{   // Здесь определяем, какой сокет отключился
    // QString name = socket->property("socketName").toString();
    // qDebug() << "Socket Name" << name << "disconnected.";
    if(m_chartSocket == socket) {
        m_chartSocket = 0; // окно графиков отключилось
        return;
    }

    int sz = (int)m_mainWnd_4Server->m_arrDlgPropVector.size();
    for(int i = 0; i < sz; ++i) {
        if(m_mainWnd_4Server->m_arrDlgPropVector[i].socket == socket) {
            // Удаляем сокет и все свойства окна массива из списка
            emit signalCloseDlgArr(i);
            break;
        }
    }
}

void MyLocalServer::slotNewConnection()
{   // Слот обработки нового клиентского подключения
    // Получаем сокет, подключённый к серверу
    QLocalSocket* localSocket = m_localServer->nextPendingConnection();
    if(!localSocket)
        return;
    // qDebug()<<"MyLocalServer::NewConnection; socketDescriptor ="<<localSocket->socketDescriptor();
    // Соединяем сигнал отключения сокета с обработчиком удаления сокета
    connect(localSocket, &QLocalSocket::disconnected, localSocket, &QLocalSocket::deleteLater);
    // Соединяем сигнал сокета о готовности передачи данных с обработчиком данных
    connect(localSocket, &QLocalSocket::readyRead, this, &MyLocalServer::slotReadClient);
    // QThread::msleep(100);
    // Отправляем информацию клиенту о соединении с сервером
    sendToClient(localSocket, "Server response: Connected!");
    // отправим запрос клиенту типа "Кто ты?"
    sendToClient(localSocket, "Get_Name");
    // showSelfClosedMessageBox(3000, "MyLocalServer::NewConnection");
}

void MyLocalServer::slotReadClient()
{   // Слот чтения информации от клиента
    // Получаем QLocalSocket после срабатывания сигнала о готовности передачи данных
    QLocalSocket *localSocket = static_cast<QLocalSocket *>(sender());
    // qDebug()<<"MyLocalServer::slotReadClient(); socketDescriptor ="<<localSocket->socketDescriptor();
    // Создаём входной поток получения данных на основе сокета
    QDataStream in(localSocket);
    // Устанавливаем версию сериализации данных потока. У клиента и сервера они должны быть одинаковыми
    in.setVersion(QDataStream::Qt_6_7);
    // Бесконечный цикл нужен для приёма блоков данных разных размеров, от двух байт и выше
    for(;;) {
        // Если размер блока равен нулю
        if(!m_nextBlockSize) {
            // Если размер передаваемого блока меньше двух байт, выйти из цикла
            if(localSocket->bytesAvailable() < (int)sizeof(quint16))
                break;
            // Извлекаем из потока размер блока данных
            in >> m_nextBlockSize;
        }
        if(localSocket->bytesAvailable() < m_nextBlockSize)
            break;
        if(!m_nextBlockSize)
            break;

        QString str{};
        in >> str;
        qDebug()<<"Server Received :"<<str;
        if(str == STR_CHART_CLIENT) {
            addSocket(localSocket, STR_CHART_ID);
            qDebug()<<"DlgChart Conected!";
        }
        if(str == STR_ARRAY_CLIENT) {
            addSocket(localSocket, STR_ARRAY_ID);
            qDebug()<<"DlgArray Conected!";
        }
        if(str == "Param") {
            m_mainWnd_4Server->m_dlgVarSel->showDlgSel();
        }
        m_nextBlockSize = 0;
    }
}

void MyLocalServer::sendToClient(QLocalSocket* localSocket, const QString& string)
{   // Метод для отправки клиенту
    QByteArray array;
    // На основе QByteArray создаём выходной поток
    QDataStream out(&array, QIODevice::WriteOnly);
    array.clear();
    out.device()->seek(0);
    out.setVersion(QDataStream::Qt_6_7);
    out << quint16(0);

    if(string == STR_CHART_ID) {
        out << QString::fromUtf8(STR_CHART_ID);
        int cntDat = m_chartDataVec.size();
        out << QString::number(cntDat);
        for(int i = 0; i < cntDat; ++i) {
            out << m_chartDataVec[i].varName;
            out << m_chartDataVec[i].unitName;
            // out << QString::number(m_chartDataVec[i].varId);
            out << QString::number(m_chartDataVec[i].unitID);
            out << QString::number(m_chartDataVec[i].typeID);
            out << QString::number(m_chartDataVec[i].varTime);
            out << QString::number(m_chartDataVec[i].varValF);
        }
    }
    else if(string == STR_ARRAY_ID) {
        out << QString::fromUtf8(STR_ARRAY_ID);
        int cntDat = m_arrayDataVec.size();
        out << QString::number(cntDat);
        for(int i = 0; i < cntDat; ++i) {
            // out << QString::number(m_arrayDataVec[i].memData);
            out << QString::number(m_arrayDataVec[i].data0);
            out << QString::number(m_arrayDataVec[i].data1);
        }
    }
    else if(string == STR_ARRAY_PROP) {
        out << QString::fromUtf8(STR_ARRAY_PROP);
        out << m_prop.varName;
        out << m_prop.unitName;
        out << m_prop.varAltType;
        out << QString::number(m_prop.unitID);
        out << QString::number(m_prop.varAddr);
        out << QString::number(m_prop.varSize);
        // out << QString::number(m_prop.varID);
        out << QString::number(m_prop.varTypeID);
        out << QString::number(m_prop.typeSize);
        out << QString::number(m_prop.arrRow);
        out << QString::number(m_prop.arrCol);
    }
    else {
        out << string;
    }
    // Перемещаем указатель на начало блока
    out.device()->seek(0);
    // Записываем двухбайтное значение действительного размера блока без учёта поля 2 байта для размера блока
    out << quint16(array.size() - sizeof(quint16));
    // Отправляем получившийся блок клиенту
    localSocket->write(array);
}

void MyLocalServer::newChartData(QVector<chartData_t> &vec)
{
    m_chartDataVec = vec;
    if(!m_chartSocket)
        return;
    else
        sendToClient(m_chartSocket, STR_CHART_ID);
}

void MyLocalServer::newArrayData(int index, QVector<curData_t> &vec)
{
    m_arrayDataVec = vec;
    if(m_mainWnd_4Server->m_arrDlgPropVector[index].socket == 0)
        return;
    QString name = m_mainWnd_4Server->m_arrDlgPropVector[index].socket->property("socketName").toString();
    if(name == STR_ARRAY_ID) {
        sendToClient(m_mainWnd_4Server->m_arrDlgPropVector[index].socket, STR_ARRAY_ID);
    }
}

void MyLocalServer::setArrayName(int index)
{
    m_prop = m_mainWnd_4Server->m_arrDlgPropVector[index];
    sendToClient(m_mainWnd_4Server->m_arrDlgPropVector[index].socket, STR_ARRAY_PROP);

}
