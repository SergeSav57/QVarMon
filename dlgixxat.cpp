#include <QThread>
#include <QString>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLayout>
#include <QTime>

#include "common.h"
#include "dlgixxat.h"
#include "ui_dlgixxat.h"
#include "mainwindow.h"
// #include "LoggingCategories.h"
#include "crc32.h"

// для отслеживания подключения/отключения девайса под OS Windows
// //#include <windows.h> // !!! <windows.h> надо подключать перед <dbt.h>
#include <dbt.h>
#include <SetupAPI.h>
#include <devguid.h>
//-------------------

MainWindow  *m_mainWnd_4DlgIXXAT;

DlgIXXAT::DlgIXXAT(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgIXXAT)
{
    ui->setupUi(this);
    m_mainWnd_4DlgIXXAT = static_cast<MainWindow *>(parent);
    m_readThread = nullptr;
}

DlgIXXAT::~DlgIXXAT()
{
    ixxat_deinit();
    delete ui;
}

void DlgIXXAT::initDlg()
{
    if(!m_ixxatCmd.LoadDLL()) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка","Библиотека 'vcinpl.dll' не загрузилась!", QMessageBox::Ok);
        return;
    }

    ZeroMemory(&m_devHandles.deviceInfo, sizeof(m_devHandles.deviceInfo));
    m_devHandles.hEnum    = nullptr;
    m_devHandles.hDevice  = nullptr;
    m_devHandles.hCanChn  = nullptr;
    m_devHandles.hCanCtrl = nullptr;


    m_CAN_BRS[0].BT0    = CAN_BT0_1000KB;
    m_CAN_BRS[0].BT1    = CAN_BT1_1000KB;
    m_CAN_BRS[0].btName = "1000KB";
    m_CAN_BRS[1].BT0    = CAN_BT0_800KB;
    m_CAN_BRS[1].BT1    = CAN_BT1_800KB;
    m_CAN_BRS[1].btName = "800KB";
    m_CAN_BRS[2].BT0    = CAN_BT0_500KB;
    m_CAN_BRS[2].BT1    = CAN_BT1_500KB;
    m_CAN_BRS[2].btName = "500KB";
    m_CAN_BRS[3].BT0    = CAN_BT0_250KB;
    m_CAN_BRS[3].BT1    = CAN_BT1_250KB;
    m_CAN_BRS[3].btName = "250KB";
    m_CAN_BRS[4].BT0    = CAN_BT0_125KB;
    m_CAN_BRS[4].BT1    = CAN_BT1_125KB;
    m_CAN_BRS[4].btName = "125KB";
    m_CAN_BRS[5].BT0    = CAN_BT0_100KB;
    m_CAN_BRS[5].BT1    = CAN_BT1_100KB;
    m_CAN_BRS[5].btName = "100KB";
    m_CAN_BRS[6].BT0    = CAN_BT0_50KB;
    m_CAN_BRS[6].BT1    = CAN_BT1_50KB;
    m_CAN_BRS[6].btName = "50KB";
    m_CAN_BRS[7].BT0    = CAN_BT0_20KB;
    m_CAN_BRS[7].BT1    = CAN_BT1_20KB;
    m_CAN_BRS[7].btName = "20KB";
    m_CAN_BRS[8].BT0    = CAN_BT0_10KB;
    m_CAN_BRS[8].BT1    = CAN_BT1_10KB;
    m_CAN_BRS[8].btName = "10KB";

    ui->comboBox_BitrateCAN->setEnabled(true);

    for(int i = 0; i<CAN_BRS_NUM; ++i) {
        ui->comboBox_BitrateCAN->insertItem(i, m_CAN_BRS[i].btName);
    }
    m_curBitRates = BR_1000K;
    ui->comboBox_BitrateCAN->setCurrentIndex(m_curBitRates); // 1000K -!!! потом cделать начальную установку через ini-file

    QPixmap iconLedGray(LED_GRAY_ICON);
    ui->LABEL_LED->setPixmap(iconLedGray);

    ui->labelDeviceName->setStyleSheet(QString("color: %1").arg(myColorBlue.name()));
    ui->labelSerNum->setStyleSheet(QString("color: %1").arg(myColorBlue.name()));
    ui->labelSpeed->setStyleSheet(QString("color: %1").arg(myColorBlue.name()));

    // закроем mutex, пока не будет коннекта с девайсом
    // m_mutex.lock();

    // включаем поток чтения
    m_portIsOpened = false;
    m_devIsConnected = false;
    createReadThread();
    m_readThread->stop = false;
    m_readThread->thread->start();
}

void DlgIXXAT::closeReadThread()
{
    if(m_readThread) {
        m_readThread->thread->quit();
        m_readThread->thread->wait();
        m_readThread = nullptr;
    }
}

void DlgIXXAT::createReadThread()
{
    m_readThread = new ReadThread("A", this);
    m_readThread->thread = new QThread;
    m_readThread->thread->setObjectName("MyIXXATReadThread");
    m_readThread->moveToThread(m_readThread->thread);

    connect(m_readThread->thread, &QThread::started,
            m_readThread, &ReadThread::doWorkReadThread);
    connect(m_readThread->thread, &QThread::finished,
            m_readThread, &QThread::deleteLater);
    connect(m_readThread, &ReadThread::destroyed,
            m_readThread, &ReadThread::deleteLater);
    // сигнал подключенного девайса (его аппаратного номера)
    connect(m_readThread, &ReadThread::signalDevIsConnected,
            m_mainWnd_4DlgIXXAT, &MainWindow::slotDevIsConnected);
    // посылка адресов переменных в девайс
    connect(this, &DlgIXXAT::signalSendSelParamToDevice,
            m_mainWnd_4DlgIXXAT, &MainWindow::slotSendSelParamToDevice);
    //==========================================================
    connect(m_readThread, &ReadThread::signalUpdateVarData,
            m_mainWnd_4DlgIXXAT, &MainWindow::slotUpdateVarData,
            Qt::DirectConnection);
    connect(m_readThread, &ReadThread::signalUpdateArrData,
            m_mainWnd_4DlgIXXAT, &MainWindow::slotUpdateArrData,
            Qt::DirectConnection);
    // чтение файла переменных из флеш памяти девайса
    connect(m_readThread, &ReadThread::signalReadVarFileFlash,
            m_mainWnd_4DlgIXXAT, &MainWindow::slotReadVarFileFlash,
            Qt::AutoConnection); // !!! Qt::DirectConnection здесь все тормозит
    connect(m_readThread, &ReadThread::signalVarFileError,
            this, &DlgIXXAT::slotVarFileError,
            Qt::AutoConnection);
    connect(m_readThread, &ReadThread::signalChannelChange,
            this, &DlgIXXAT::slotChannelChange,
            Qt::AutoConnection);
    connect(m_readThread, &ReadThread::signalChannelDone,
            this, &DlgIXXAT::slotChannelDone,
            Qt::AutoConnection);
}

bool DlgIXXAT::checkIXXATConnect()
{
    DWORD            devIndex;
    SP_DEVINFO_DATA  devInfo; // в devInfo прописывается GUID
    BYTE			 buffer[100];

    HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
    devInfo.cbSize = sizeof(devInfo);
    for(devIndex=0; ; ++devIndex) {
        HDEVINFO NewDeviceInfoSet = SetupDiGetClassDevsEx(&GuidInterfaceList[0],
            NULL, NULL, DIGCF_PRESENT, DeviceInfoSet, NULL, NULL);
        if(!(SetupDiEnumDeviceInfo(NewDeviceInfoSet,devIndex,&devInfo))) {
            return false;
        }
        SetupDiGetDeviceRegistryProperty(NewDeviceInfoSet, &devInfo, SPDRP_HARDWAREID,
            NULL, buffer, sizeof(buffer), NULL); // в буфере VID&PID - здесь не используем
        SetupDiGetDeviceRegistryProperty(NewDeviceInfoSet, &devInfo, SPDRP_DEVICEDESC,
            NULL, buffer, sizeof(buffer), NULL); // в буфере имя девайса (описание устройства) - для IXXAT: "VCI4 USB-to-CAN compact"
#ifdef UNICODE
        QString str = QString::fromUtf16((const char16_t *)buffer);
#else
        QString str = QString::fromUtf8((const ushort *)buffer);
#endif
        if(str == m_sIXXAT_name) {
            if(!m_portIsOpened) {
                if(!ixxat_init(true)) // true - раширенный режим; false - стандартный
                    return false;
            }
            return true;
        }
    }
    return false;
}

void DlgIXXAT::slotChangeDevice(MSG* msg)
{
    PDEV_BROADCAST_HDR lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
    if(!lpdb)
        return;
    QString str{};
    // qDebug()<<"DeviceInterface::WM_DEVICECHANGE"<<QString::number(msg->wParam,16);
    switch(msg->wParam) {
    case DBT_DEVICEARRIVAL: // подключили девайс
        if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
            PDEV_BROADCAST_DEVICEINTERFACE lpdbv = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(lpdb);
            str = QString::fromWCharArray(lpdbv->dbcc_name);
            str = str.toUpper();
            // qDebug() << "DBT_DEVICEARRIVAL Name=" << str;
            if(str.contains(m_sIXXAT_vidpid)) {
                if(!checkIXXATConnect()) // если адаптер НЕ был подключен
                    ixxat_deinit();
            }
        }
        break;
    case DBT_DEVICEREMOVECOMPLETE: // отключили девайс
        if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
            PDEV_BROADCAST_DEVICEINTERFACE lpdbv = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(lpdb);
            str = QString::fromWCharArray(lpdbv->dbcc_name);
            str = str.toUpper();
            // qDebug() << "DBT_DEVICEREMOVECOMPLETE =" << str;
            if(str.contains(m_sIXXAT_vidpid)) {
                if(m_portIsOpened) // если адаптер был подключен
                    ixxat_deinit();
            }
        }
        break;
    default:
        break;
    }
}

void DlgIXXAT::showErrorBox(LONG err, QString op)
{
    CHAR	m_errorBuffer[100] = {};
    m_ixxatCmd.vciFormatError(err, m_errorBuffer, sizeof(m_errorBuffer));
    QString str_t = QString::fromLocal8Bit(m_errorBuffer);
    QString str = QString::fromUtf8("< %1 >\n\n%2").arg(op, str_t);
    QMessageBox::critical(Q_NULLPTR, "Ошибка инициализации IXXAT!", str, QMessageBox::Ok);
}

bool DlgIXXAT::ixxat_init(bool fExtend)
{   //>>> fExtend => true - раширенный режим; false - стандартный
    HRESULT res;
    if(m_portIsOpened)
        return true;
    //>>> vciInitialize() : Инициализирует VCINPL2 для вызывающего процесса
    if((res = m_ixxatCmd.vciInitialize()) != VCI_OK) {
        showErrorBox(res, "Error vciInitialize( )");
        return false;
    }
    // qDebug() << "vciInitialize( ) OK";

    //=== vciDevice
    //>>> vciEnumDeviceOpen() : Открывает список всех адаптеров шины,
    //>>> зарегистрированных в VCI
    if((res = m_ixxatCmd.vciEnumDeviceOpen(&m_devHandles.hEnum)) != VCI_OK) {
        showErrorBox(res, "Error vciEnumDeviceOpen( )");
        return false;
    }
    // qDebug() << "vciEnumDeviceOpen( ) OK";

    //>>> vciEnumDeviceNext() : Определяет описание адаптера шины в списке устройств и
    //>>> увеличивает индекс внутреннего списка таким образом, чтобы последующий вызов
    //>>> функции передавал описание следующему адаптеру
    if((res = m_ixxatCmd.vciEnumDeviceNext(
             m_devHandles.hEnum, &m_devHandles.deviceInfo)) != VCI_OK) {
        showErrorBox(res, "Error vciEnumDeviceNext( )");
        return false;
    }
    // qDebug() << "vciEnumDeviceNext( ) OK";

    // vciDeviceGetInfo(m_devHandles.hEnum, &m_devHandles.deviceInfo);

    //>>> vciDeviceOpen() : Открывает адаптер шины с указанным идентификатором устройства.
    //>>> Определяет дискриптор устройства m_hDevice.
    if((res = m_ixxatCmd.vciDeviceOpen(
             m_devHandles.deviceInfo.VciObjectId, &m_devHandles.hDevice)) != VCI_OK) {
        showErrorBox(res, "Error vciDeviceOpen( )");
        return false;
    }
    // qDebug() << "vciDeviceOpen( ) OK";

    uint32_t dwCanNo = 0; // подключено одно устройство IXXAT

    //=== canChannel
    //>>> canChannelOpen() : Открывает или создает канал сообщений.
    if((res = m_ixxatCmd.canChannelOpen(
             m_devHandles.hDevice, dwCanNo, TRUE, &m_devHandles.hCanChn)) != VCI_OK) {
        showErrorBox(res, "Error canChannelOpen( )");
        return false;
    }
    // qDebug() << "canChannelOpen( ) OK";


    //=== canControl
    //>>> canControlOpen() : Открывает блок управления подключением шины к CAN адаптеру
    if((res = m_ixxatCmd.canControlOpen(
             m_devHandles.hDevice, dwCanNo, &m_devHandles.hCanCtrl)) != VCI_OK) {
        showErrorBox(res, "Error canControlOpen( )");
        return false;
    }
    // qDebug() << "canControlOpen( ) OK";


    // заполним ID CAN-сообщения
    m_CanMsg.dwTime = 0;// 100;
    m_CanMsg.uMsgInfo.Bits.type = CAN_MSGTYPE_DATA;			// 0 -data frame
    // длина сообщения в байтах
    m_CanMsg.uMsgInfo.Bytes.bFlags = CAN_MSGFLAGS_DLC;		// 0x0F -[bit 0] data length code
    m_CanMsg.uMsgInfo.Bytes.bAccept = CAN_ACCEPT_ALWAYS;	// 0xff -message always accepted
    uint8_t bMode = CAN_OPMODE_ERRFRAME;
    if (fExtend) { // расширенный формат
        bMode |= CAN_OPMODE_EXTENDED;
        m_CanMsg.uMsgInfo.Bytes.bFlags |= CAN_MSGFLAGS_EXT;	// 0x80 -[bit 7] frame format (0=11-bit, 1=29-bit)
    }
    else { // стандартный режим
        bMode |= CAN_OPMODE_STANDARD;
    }
    //>>> canControlInitialize() : Устанавливает режим работы и скорость передачи данных CAN-соединения
    uint8_t bBtr0 = m_CAN_BRS[m_curBitRates].BT0;
    uint8_t bBtr1 = m_CAN_BRS[m_curBitRates].BT1;
    if((res = m_ixxatCmd.canControlInitialize(
             m_devHandles.hCanCtrl, bMode, bBtr0, bBtr1)) != VCI_OK) {
        showErrorBox(res, "Error canControlInitialize( )");
        return false;
    }
    // qDebug() << "canControlInitialize( ) OK";


    //>>> canControlSetAccFilter() : Устанавливает 11- или 29-разрядный фильтр приема для соединения CAN.
    //>>> fExtend == false: 11-bit acceptance filter; TRUE: 29-bit
    //>>> синтаксис: canControlSetAccFilter(HANDLE hCanCtl, BOOL fExtend, uint32_t dwCode, uint32_t dwMask)
    uint32_t dwCode = CAN_ACC_CODE_ALL;
    uint32_t dwMask = CAN_ACC_MASK_ALL;
    if((res = m_ixxatCmd.canControlSetAccFilter(
             m_devHandles.hCanCtrl, fExtend, dwCode, dwMask)) != VCI_OK) {
        showErrorBox(res, "Error canControlSetAccFilter( )");
        return false;
    }
    // qDebug() << "canControlSetAccFilter( ) OK";

    //>>> canControlStart() : Запускает (TRUE) или останавливает (false) контроллер подключения CAN.
    if((res = m_ixxatCmd.canControlStart(
             m_devHandles.hCanCtrl, true)) != VCI_OK) {
        showErrorBox(res, "Error canControlStart(TRUE)");
        return false;
    }
    // qDebug() << "canControlStart( ) OK";

    // //=== canChannel
    // //>>> canChannelOpen() : Открывает или создает канал сообщений.
    // if((res = m_ixxatCmd.canChannelOpen(m_hDevice, dwCanNo, TRUE, &m_hCanChn)) != VCI_OK) {
    //     showErrorBox(res, "Error canChannelOpen( )");
    //     return false;
    // }
    // qDebug() << "canChannelOpen( ) OK";

    //>>> canChannelInitialize() : Инициализирует буферы приема и передачи канала сообщений.
    uint16_t wRxFifoSize = 128; // размер буфера приема в количестве CAN-сообщений.
    uint16_t wRxThreshold = 1; // количество сообщений, которое должен содержать буфер приема для запуска события приема
    uint16_t wTxFifoSize = 128; // размер буфера передачи в количестве CAN-сообщений
    uint16_t wTxThreshold = 1; // количество сообщений, для которых должно хватить места в буфере передачи, чтобы вызвать событие передачи
    if((res = m_ixxatCmd.canChannelInitialize(
             m_devHandles.hCanChn, wRxFifoSize, wRxThreshold,
             wTxFifoSize, wTxThreshold)) != VCI_OK) {
        showErrorBox(res, "Error canChannelInitialize( )");
        return false;
    }
    // qDebug() << "canChannelInitialize( ) OK";

    //>>> canChannelActivate() : Активирует (TRUE) или деактивирует (false)
    //>>> канал передачи сообщений.
    if((res = m_ixxatCmd.canChannelActivate(m_devHandles.hCanChn, true)) != VCI_OK) {
        showErrorBox(res, "Error canChannelActivate( )");
        return false;
    }
    // qDebug() << "canChannelActivate( ) OK";

    // органы управления вкладки CAN --------------
    ui->lineEdit_NameIXXAT->setText(m_devHandles.deviceInfo.Description);
    ui->lineEdit_SerNumIXXAT->setText(m_devHandles.deviceInfo.UniqueHardwareId.AsChar);
    // при запущенном обмене по шине CAN запретим изменение скорости шины
    ui->comboBox_BitrateCAN->setEnabled(true);
    ui->BTN_CHECK_CAN_ON->setChecked(true);
    QPixmap iconLed(LED_GREEN_ICON);
    // QPixmap iconLed(LED_RED_ICON); // красный означает подключение только к адаптеру
    ui->LABEL_LED->setPixmap(iconLed);

    m_portIsOpened = true;
    qDebug() << " >>> m_portIsOpened";

    return true;
}

void DlgIXXAT::ixxat_deinit()
{
    // вЫключаем поток чтения
    if(m_readThread) {
        m_readThread->stop = true;
    }
    if(!m_portIsOpened)
        return;

    m_portIsOpened = false;
    m_devIsConnected = false;

    HRESULT res;
    //>>> canChannelClose() : Закрывает канал сообщений.
    if((res = m_ixxatCmd.canChannelClose(m_devHandles.hCanChn)) != VCI_OK) {
        showErrorBox(res, "canChannelClose( )");
        return;
    }
    m_devHandles.hCanChn = nullptr;

    // //>>> canControlStart() : Запускает (TRUE) или останавливает (false) контроллер подключения CAN.
    // if((res = m_ixxatCmd.canControlStart(m_devHandles.hCanCtrl, FALSE)) != VCI_OK) {
    //     showErrorBox(res, "canControlStart(FALSE)");
    //     return;
    // }

    //>>> canControlClose() : Закрывает блок управления подключением шины к CAN адаптеру
    if((res = m_ixxatCmd.canControlClose(m_devHandles.hCanCtrl)) != VCI_OK) {
        showErrorBox(res, "canControlClose( )");
        return;
    }
    m_devHandles.hCanCtrl = nullptr;

    //>>> vciDeviceClose() : Закрывает адаптер шины с указанным идентификатором устройства.
    if(m_devHandles.hDevice) {
        if((res = m_ixxatCmd.vciDeviceClose(m_devHandles.hDevice)) != VCI_OK) {
            showErrorBox(res, "vciDeviceClose( )");
            return;
        }
        m_devHandles.hDevice = nullptr;
    }

    ui->lineEdit_NameIXXAT->setText("");
    ui->lineEdit_SerNumIXXAT->setText("");
}

void DlgIXXAT::on_BTN_CHECK_CAN_ON_clicked()
{
    if(ui->BTN_CHECK_CAN_ON->isChecked()) {
        if(!m_portIsOpened)
            if(!ixxat_init(true))
                ui->BTN_CHECK_CAN_ON->setChecked(false);
        QPixmap iconLed(LED_GREEN_ICON);
        ui->LABEL_LED->setPixmap(iconLed);
        // отправим девайсу адреса параметров для сканирования
        emit signalSendSelParamToDevice();
    }
    else {
        ixxat_deinit();
        QPixmap iconLed(LED_GRAY_ICON);
        ui->LABEL_LED->setPixmap(iconLed);
    }
}

bool DlgIXXAT::saveVarFileToUnitBoard(uint32_t unitID, QFile &file)
{   //! здесь только рассчитывается контрольная сумма файла,
    //! а непосредственно запись в функции SendData()
    //! запись crc32 во флеш производится в начало сектора,
    //! непосредтвенно перед записью файла переменных

    file.open(QIODevice::ReadOnly);
    m_bufVarFileData = file.readAll();
    m_varFileCRC32 = getCRC32(m_bufVarFileData, m_bufVarFileData.size());
    file.close();
    qDebug()<<" m_varFileCRC32 ="<<QString::number(m_varFileCRC32, 16).toUpper();

    uint32_t fileSz = file.size();
    sendCommand(CMD_FILE_SAVE, unitID, fileSz, 0);
    // ждем ответа об успешном получении команды готовности для записи файла во флеш
    if(waitFlagEn(m_isVarSave2FlashOK, 1000)) { // реально время ~1ms
        qDebug()<<" waitParamEn : device ready to receive";
        // передаем файл переменных
        sendData(VAR_FILE, unitID);
        // еще раз ждем ответа -теперь об успешной записи файла во флеш
        if(waitFlagEn(m_isVarSave2FlashOK, 1000)) {
            // реально время ~400ms для 80 записей переменных
            // (здесь сумма: время передачи данных и время записи во флеш)
            qDebug()<<" waitParamEn : file is saved";
            return true;
        }
        else {
            qDebug()<<" waitFlagEn : file is NOT saved";
            return false;
        }
    }
    else {
        qDebug()<<" ERROR: waitFlagEn : TimeOUT - is NOT received from device";
        return false;
    }
    return false;
}

void DlgIXXAT::loadVarFileFromUnitBoard(uint32_t unit)
{
    // команда для девайса на передачу файла переменных из флеш в хост
    sendCommand(CMD_FILE_LOAD, unit, 0, 0);
    // ожидаем готовности узла к передаче файла с борта
    if(waitFlagEn(m_isVarLoad2HostOK, 100)) {
        qDebug() << (QString("Узел %1 готов к передаче файла переменных.Размер = %2").arg(unit).arg(m_varFileSize));
    }
    else
        qDebug() << (QString("Узел %1 НЕ готов к передаче файла переменных").arg(unit));

    // загрузка файла происходит в потоке ReadThread():case VAR_FILE
    // ожидаем флага окончания загрузки файла
    if(waitFlagEn(m_isVarLoad2HostOK, 1000)) {
        qDebug() << (QString("Узел %1 закончил передачу файла переменных.Размер = %2").arg(unit).arg(m_varFileSize));
        QThread::msleep(20);
    }
    else
        qDebug() << (QString("Узел %1 НЕ закончил передачу файла переменных").arg(unit));
}

void DlgIXXAT::loadVarFileFromAllBoard(unitProp_t &prop)
{   // загрузить с борта файл переменных из всех подключенных девайсов.
    // >: загруженный файл из ф-ции ReadThread() пересылается в DlgCreateVarFile::slotReadVarFileFlash()
    for(int i = 0; i < (int)prop.numChannels; ++i) {
        int unit = prop.listID[i].toInt();
        if(prop.property[unit].isPresent) {
            loadVarFileFromUnitBoard(unit);
            prop.property[unit].isLoadVarFileFlash = true;
            prop.property[unit].isLoadVarFileDisk = false;
        }
    }
}

void DlgIXXAT::clearRequestUnit(uint32_t unit)
{   // очистка запросов для всех узлов
    sendCommand(CMD_NEW_VAR, unit, 0, 0);
    // waitFlagEn(m_isNewVarOK, 50);
    sendCommand(CMD_NEW_ARR, unit, 0, 0);
    // waitFlagEn(m_isNewArrOK, 50);
}

void DlgIXXAT::clearAllRequest()
{   // очистка запросов для всех узлов
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_mainWnd_4DlgIXXAT->m_unitProp.property[i].isPresent) // если канал задействован
            clearRequestUnit(i);
    }
}

void DlgIXXAT::getDevId()
{   // определить ID для всех подключенных узлов
    for(int id = 0; id < NUMCHANNELMAX; ++id) {
        if(!sendCommand(CMD_GET_DEV_ID, id, 0, 0))
            return;
        // подключенный канал выдает сигнал DEV_IS_PRESENT,
        // подключенный к слоту slotDevIsConnected()
    }
}

bool DlgIXXAT::sendCommand(uint8_t cmdID, uint32_t unitID, uint32_t par1, uint32_t par2)
{	// формирует команду
    // var: par1: число переменных; par2: 0;
    // arr: par1: число массивов,EN; par2: 0, индекс массива;
    // file: par1: размер файла переменных, par2: 0;
    memset(&m_CanMsg.dwMsgId, 0, sizeof(m_CanMsg.dwMsgId));
    SET_CMD_NAME(m_CanMsg.dwMsgId, cmdID);  // имя команды (биты 0..3)
    SET_ID_CMD(m_CanMsg.dwMsgId, CAN_CMD|PC_REQ); // передаем команду от РС
    SET_ID_DEV(m_CanMsg.dwMsgId, (unitID + 1));   // установим ID узла
    switch(cmdID) { // дополнительные параметры команды
    case CMD_GET_DEV_ID:
        // par1 всегда 0
        // par2 всегда 0
        break;
    case CMD_NEW_VAR: // новый список переменных
        // par2 всегда 0
        SET_VAR_NUM(m_CanMsg.dwMsgId, par1);    // число переменных (биты 4..19)
        // qDebug() << "PC->Msg:CMD_NEW_DAT";
        break;
    case CMD_NEW_ARR: // новый список массивов
        // par2 всегда 0
        SET_ARR_NUM(m_CanMsg.dwMsgId, par1);    // число массивов (биты 4..19)
        // qDebug() << "PC->Msg:CMD_NEW_ARR";
        break;
    case CMD_SCAN_EN: // разрешить/запретить сканирование адресов массива
        SET_ARR_EN(m_CanMsg.dwMsgId, par1);     // 1:разрешено, 0:запрещено (бит 19)
        SET_ARR_ID(m_CanMsg.dwMsgId, par2);     // индекс массива в списке выбранных (биты 4..18)
        // qDebug() << "PC->Msg:CMD_SCAN_EN";
        break;
    case CMD_GET_DATA:
        // par1 всегда 0
        // par2 всегда 0
        break;
    case CMD_FILE_SAVE:
        // par2 всегда 0
        SET_FILE_SZ(m_CanMsg.dwMsgId, par1);   // размер файла переменных (биты 4..19)
        break;
    case CMD_FILE_LOAD:
        // par1 всегда 0
        // par2 всегда 0
        break;
    }

    // ! команда передается только в составе ID кадра CAN (длина данных = 0)
    // обнулим длину данных кадра CAN (биты 3..0) -[поле DLC]
    m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC;
    if (!sendMsg(&m_CanMsg)) {
        getMessageError();
        return false;
    }
    return true;
}

void DlgIXXAT::sendData(uint8_t cmdID, uint32_t unitID)
{	// передаем адреса переменных/массивов
    QVector<varProp_t> unitVector{};
    switch(cmdID) {
    case VAR_DAT: { // переменные ===
        const QVector<var_t> &tmpVector = m_mainWnd_4DlgIXXAT->m_dlgVarSel->m_VarSelGlobalVector;
        int varSz = tmpVector.size();
        varProp_t varProp;
        // заполним вектор адресами переменных текущего узла и их идентификаторами
        for(int i = 0; i < varSz; ++i) {
            if(unitID == tmpVector[i].varID.unitID) {
                varProp.varAddr = tmpVector[i].varProp.varAddr;
                varProp.varID = tmpVector[i].varID.varID;
                unitVector.emplace_back(varProp);
            }
        }
        // передаем ID переменной и ее адрес
        for(int i = 0; i < (int)unitVector.size(); ++i) {
            memset(&m_CanMsg.dwMsgId, 0, sizeof(m_CanMsg.dwMsgId));
            memset(&m_CanMsg.abData, 0, sizeof(m_CanMsg.abData));
            SET_ID_DEV(m_CanMsg.dwMsgId, (unitID + 1));         // установим ID узла
            SET_VAR_ID(m_CanMsg.dwMsgId, unitVector[i].varID);  // установим ID переменной
            SET_ID_CMD(m_CanMsg.dwMsgId, VAR_DAT);              // передаем данные
            // адрес в формате little-endian
            m_CanMsg.abData[0] = (unitVector[i].varAddr >>  0) & 0xFF;
            m_CanMsg.abData[1] = (unitVector[i].varAddr >>  8) & 0xFF;
            m_CanMsg.abData[2] = (unitVector[i].varAddr >> 16) & 0xFF;
            m_CanMsg.abData[3] = (unitVector[i].varAddr >> 24) & 0xFF;
            m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC; // очистим DLC
            m_CanMsg.uMsgInfo.Bytes.bFlags |= 4;
            if (!sendMsg(&m_CanMsg)) {
                getMessageError();
                return;
            }
        }
        break;
    }

    case ARR_DAT: { // массивы ===
        // передаем базовые адреса и размеры массивов
        const QVector<var_t> &tmpVector = m_mainWnd_4DlgIXXAT->m_dlgVarSel->m_ArrSelGlobalVector;
        int arrSz = tmpVector.size();
        varProp_t varProp;
        QVector<varProp_t> unitVector{};
        for(int i = 0; i < arrSz; ++i) { // поиск массивов для текущего узла
            if(unitID == tmpVector[i].varID.unitID) {
                varProp.varID   = tmpVector[i].varID.varID;
                varProp.varAddr = tmpVector[i].varProp.varAddr;
                varProp.varSize = tmpVector[i].varProp.varSize;
                unitVector.emplace_back(varProp);
            }
        }
        for(int i = 0; i < (int)unitVector.size(); ++i) {
            memset(&m_CanMsg.dwMsgId, 0, sizeof(m_CanMsg.dwMsgId));
            memset(&m_CanMsg.abData, 0, sizeof(m_CanMsg.abData));
            SET_ID_DEV(m_CanMsg.dwMsgId, (unitID + 1));   // установим ID узла
            SET_ARR_ID(m_CanMsg.dwMsgId, unitVector[i].varID); // установим ID массива
            SET_ID_CMD(m_CanMsg.dwMsgId, ARR_DAT);  // идентификатор команды/данных массива
            // базовый адрес массива в формате little-endian
            m_CanMsg.abData[0] = (unitVector[i].varAddr >>  0) & 0xFF;
            m_CanMsg.abData[1] = (unitVector[i].varAddr >>  8) & 0xFF;
            m_CanMsg.abData[2] = (unitVector[i].varAddr >> 16) & 0xFF;
            m_CanMsg.abData[3] = (unitVector[i].varAddr >> 24) & 0xFF;
            // размер массива
            m_CanMsg.abData[4] = (unitVector[i].varSize >>  0) & 0xFF;
            m_CanMsg.abData[5] = (unitVector[i].varSize >>  8) & 0xFF;
            // установим длину данных кадра CAN 4 байт
            m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC; // сначала очистим DLC
            m_CanMsg.uMsgInfo.Bytes.bFlags |= 6;
            if (!sendMsg(&m_CanMsg)) {
                getMessageError();
                return;
            }
        }
        break;
    }

    case VAR_FILE: { // запись файла переменных во флеш ===
        memset(&m_CanMsg.dwMsgId, 0, sizeof(m_CanMsg.dwMsgId));
        memset(&m_CanMsg.abData, 0, sizeof(m_CanMsg.abData));
        SET_ID_CMD(m_CanMsg.dwMsgId, VAR_FILE); // передаем файл переменных
        SET_ID_DEV(m_CanMsg.dwMsgId, (unitID + 1));   // установим ID узла
        // сначала запишем КС файла в формате big-endian, т.к. КС вычисляется в этом формате
        // ! (для примера: crc = 0x64d442d2 -> получаем в девайсе: 0xd242d464)
        m_CanMsg.abData[0] = (m_varFileCRC32 >>  0) & 0xFF; // для примера: d2
        m_CanMsg.abData[1] = (m_varFileCRC32 >>  8) & 0xFF; // для примера: 42
        m_CanMsg.abData[2] = (m_varFileCRC32 >> 16) & 0xFF; // для примера: d4
        m_CanMsg.abData[3] = (m_varFileCRC32 >> 24) & 0xFF; // для примера: 64
        m_CanMsg.abData[4] = 0; // обнулим
        m_CanMsg.abData[5] = 0;
        m_CanMsg.abData[6] = 0;
        m_CanMsg.abData[7] = 0;
        m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC; // очистим DLC
        m_CanMsg.uMsgInfo.Bytes.bFlags |= 8;
        if (!sendMsg(&m_CanMsg)) {
            getMessageError();
            return;
        }
        qDebug()<<"VarFile CRC32 is sended to flash";
        // данные файла
        int bufSz = m_bufVarFileData.size();
        int dlc = 0; // сколько байт передавать
        // далее записываем весь файл в формате little-endian
        for(int i = 0; i < bufSz; ) {
            for(int j = 0; j < 8; ++j) {
                if(i < bufSz){
                    m_CanMsg.abData[j] = m_bufVarFileData[i++];
                    ++dlc;
                }
            }
            m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC; // очистим DLC
            m_CanMsg.uMsgInfo.Bytes.bFlags |= dlc;
            dlc = 0;
            if (!sendMsg(&m_CanMsg)) {
                getMessageError();
                return;
            }
            if(i >= bufSz-1) {
                qDebug()<<"VarFile Data is sended to flash";
                break;
            }
        }

        // for(int i = 0; i < bufSz; ) {
        //     if(i < bufSz){
        //         m_CanMsg.abData[0] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[1] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[2] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[3] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[4] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[5] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[6] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }
        //     if(i < bufSz){
        //         m_CanMsg.abData[7] = m_bufVarFileData[i++];
        //         ++dlc;
        //     }

        //     m_CanMsg.uMsgInfo.Bytes.bFlags &= ~CAN_MSGFLAGS_DLC; // очистим DLC
        //     m_CanMsg.uMsgInfo.Bytes.bFlags |= dlc;
        //     dlc = 0;
        //     if (!sendMsg(&m_CanMsg)) {
        //         getMessageError();
        //         return;
        //     }
        //     if(i >= bufSz-1)
        //         break;
        // }
        // qDebug()<<"VarFile Data is sended";

        break;
    } // case VAR_FILE
    } // switch(cmdID)
}

bool DlgIXXAT::sendMsg(CANMSG *CanMsg)
{
    if((m_ixxatCmd.canChannelSendMessage(m_devHandles.hCanChn, 1000/*ms*/, CanMsg)) != VCI_OK)
        return false;
    return true;
}

void DlgIXXAT::getMessageError()
{
    /*
#define CAN_STATUS_TXPEND    0x00000001 // transmission pending
#define CAN_STATUS_OVRRUN    0x00000002 // data overrun occurred
#define CAN_STATUS_ERRLIM    0x00000004 // error warning limit exceeded
#define CAN_STATUS_BUSOFF    0x00000008 // bus off status
#define CAN_STATUS_ININIT    0x00000010 // init mode active
#define CAN_STATUS_BUSCERR   0x00000020 // bus coupling error
 * */
    CANCHANSTATUS Status;
    m_ixxatCmd.canChannelGetStatus(m_devHandles.hCanChn, &Status);
    if(Status.sLineStatus.dwStatus & 0x3f) {
        switch(Status.sLineStatus.dwStatus)
        {
        case CAN_STATUS_TXPEND:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Контроллер CAN в данный момент передает сообщение на шину", QMessageBox::Ok);
            break;
        case CAN_STATUS_OVRRUN:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Произошло переполнение данных в приемном буфере контроллера CAN", QMessageBox::Ok);
            break;
        case CAN_STATUS_ERRLIM:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Произошло переполнение счетчика ошибок контроллера CAN", QMessageBox::Ok);
            break;
        case CAN_STATUS_BUSOFF:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Контроллер CAN перешел в состояние 'ШИНА выключена'", QMessageBox::Ok);
            break;
        case CAN_STATUS_ININIT:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Контроллер CAN находится в остановленном состоянии", QMessageBox::Ok);
            break;
        case CAN_STATUS_BUSCERR:
            QMessageBox::critical(Q_NULLPTR, "Ошибка!", "Неисправное соединение шины", QMessageBox::Ok);
            break;
        }
    }
}

void DlgIXXAT::slotVarFileError(int unit)
{
    QMessageBox::critical(Q_NULLPTR, "Ошибка!",
                          QString("Файл переменных на борту узла %1 отсутствует")
                          .arg(unit), QMessageBox::Ok);
}

void DlgIXXAT::slotChannelChange(bool en)
{   //>>> последовательное переключение опроса каналов
    if(!en) { // первое включение
        // определим, какие каналы подключены
        chProp.nUnits = 0;
        chProp.preUnit = 0;
        chProp.curUnit = 0;
        for(int i = 0; i < NUMCHANNELMAX; ++i) {
            chProp.chan[i].isPresent = false;
            chProp.chan[i].set = false;
        }
        for(int i = 0; i < NUMCHANNELMAX; ++i) {
            if(m_mainWnd_4DlgIXXAT->m_unitProp.property[i].isPresent) {
                chProp.chan[i].isPresent = true;
                ++chProp.nUnits;
                chProp.curUnit = i;
            }
        }
    }

    for(uint32_t unit = 0; unit < NUMCHANNELMAX; ++unit) {
        if(!chProp.chan[unit].isPresent)
            continue; // если канал не подключен
        // если предыдущий канал завершил передачу,переключаем канал
        if(chProp.chan[chProp.preUnit].set == 0) {
            if(chProp.nUnits > 1) { // если каналов > 1
                if(unit == chProp.preUnit) // если канал есть предыдущий
                    continue;
            } // иначе работаем с тем же каналом
            chProp.chan[unit].set = 1;
            sendCommand(CMD_GET_DATA, unit, 0, 0);
            chProp.preUnit = unit;
        }
        // иначе ждем, пока не освободится
        else if(chProp.chan[chProp.preUnit].set == 1) {
            return;
        }
    }
}

void DlgIXXAT::slotChannelDone(uint32_t unit)
{
    chProp.chan[unit].set = 0;
    // после последнего канала проверим наличие на шине подключенных каналов
    if(unit == chProp.curUnit)
        getDevId();
    slotChannelChange(true);
}

//==========================================
// ---- ПОТОК ЧТЕНИЯ -----------------------
//==========================================

ReadThread::ReadThread(QString s, void *dlg)
    : name(s)
{
    stop = true;
    pIxxat = (DlgIXXAT*)dlg;
}

ReadThread::~ReadThread()
{
    qDebug() << "~~~~~ReadThread is Finished";
}

void ReadThread::doWorkReadThread()
{   // поток для чтения сообщений по каналу CAN
    CANCHANSTATUS Status;
    CANMSG sCanMsg;
    qDebug() << "doWorkReadThread()" << QThread::currentThread()->objectName();

    int curDevID = -1; // текущий ID узла ('-1' -все светики выключены)
    emit signalDevIsConnected(curDevID);

    curData_t varCurData{};
    curData_t arrCurData{};
    QByteArray fileData{};
    fileData.reserve(16000);

    while(!stop) {
        if(!pIxxat->m_portIsOpened) // порт открывается после инициализации CAN
            continue;

        //>>> canChannelGetStatus() : Определяет текущее состояние канала сообщений,
        //>>> а также текущие настройки и текущее состояние контроллера,
        //>>> подключенного к каналу.
        pIxxat->m_ixxatCmd.canChannelGetStatus(pIxxat->m_devHandles.hCanChn, &Status);
        if (Status.sLineStatus.dwStatus > CAN_STATUS_TXPEND) {
            // нет связи (при подключенном IXXAT)   !!! не срабатывает !!!
            pIxxat->m_devIsConnected = false;
            continue; // что-то пошло не так (см.выше getMessageError()->dwStatus)
        }
        //>>> canChannelWaitRxEvent() : Ожидает, пока с шины CAN не будет получено сообщение CAN
        //>>>                           или не истечет интервал ожидания (ms)
        int res = pIxxat->m_ixxatCmd.canChannelWaitRxEvent(pIxxat->m_devHandles.hCanChn, 1000);
        if(res == VCI_E_TIMEOUT) {
            // pIxxat->getDevId();
            // curDevID = -1; // выключить все светики каналов
            // emit signalDevIsConnected(curDevID);
            continue;
        }
        else if(res == VCI_OK)
        {   //>>> canChannelPeekMessage() : Считывает следующее сообщение CAN из приемного буфера канала сообщений
            int rs = pIxxat->m_ixxatCmd.canChannelPeekMessage(pIxxat->m_devHandles.hCanChn, &sCanMsg);
            if(rs == VCI_E_RXQUEUE_EMPTY) {
                continue;
            }
            uint8_t cmdId = GET_ID_CMD(sCanMsg.dwMsgId);
            if(!cmdId)
                continue; // если не пришли ни команда, ни данные

            unitProp_t *propTmp = &m_mainWnd_4DlgIXXAT->m_unitProp;

            // *** РАЗБОР КОМАНД и СООБЩЕНИЙ ******************************************
            switch(cmdId) {
            case CAN_CMD: // принимаем от девайса команды и ответы на команды от РС
                switch(GET_CMD_NAME(sCanMsg.dwMsgId)) {
                case DEV_IS_PRESENT: // девайс присутствует на шине (1 раз в секунду)
                    // ID узла, передающего данные
                    //!!! (приходящий ID на 1 больше, т.к. аппаратный фильтр CAN девайса
                    //!!! не может определить ID=0)
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    // отметим присутствие соответствующего канала в свойствах узлов
                    ///propTmp->property[curDevID].isPresent = true;
                    propTmp->property[curDevID].isPresent = true;
                    pIxxat->m_devIsConnected = true;
                    // включение зеленого LED соответствующего канала
                    // qDebug() << "Dev->Msg:DEV_IS_PRESENT; ID ="<<curDevID;
                    emit signalDevIsConnected(curDevID);
                    break;
                case NEW_VAR_OK: // старые переменные удалены
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    pIxxat->m_isNewVarOK = true; // девайс дал ответ "NEW_DAT_OK"
                    // можно отправлять девайсу адреса параметров для сканирования переменных
                    // qDebug(logInfo()) << "Dev->Msg:NEW_DAT_OK";
                    break;
                case CMD_VAR_NUM: // посылка девайса -число переменных в пакете
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    // число всех переменных в посылке девайса
                    propTmp->property[curDevID].varNewData.numVarsDev = GET_VAR_NUM(sCanMsg.dwMsgId);
                    // очистим вектор данных переменных
                    propTmp->property[curDevID].varNewData.vecCurData.clear();
                    break;
                case NEW_ARR_OK: // старые массивы удалены
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    pIxxat->m_isNewArrOK = true; // девайс дал ответ "NEW_ARR_OK"
                    // можно отправлять девайсу адреса параметров для сканирования массивов
                    // qDebug(logInfo()) << "Dev->Msg:NEW_ARR_OK";
                    break;
                case CMD_ARR_NUM: // число всех массивов (самый первый фрейм)
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    propTmp->property[curDevID].arrNewData.numArrs = GET_ARR_NUM(sCanMsg.dwMsgId);
                    // очистим вектор данных всех массивов
                    propTmp->property[curDevID].arrAllVector.clear();
                    // qDebug() << "\nCMD_ARR_NUM:Channel"<<curDevID<<"Num="<<GET_ARR_NUM(sCanMsg.dwMsgId);
                    break;
                case CMD_ARR_SZ: // размер текущего массива (перед каждым массивом)
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    // очистим вектор данных текущего массива
                    propTmp->property[curDevID].arrNewData.vecCurData.clear();
                    // размер текущего массива (в байтах)
                    propTmp->property[curDevID].arrNewData.arrSz = GET_ARR_SZ(sCanMsg.dwMsgId);
                    // число кадров в массиве (кратно 8и байтам)
                    propTmp->property[curDevID].arrNewData.nArrFrames =
                        (propTmp->property[curDevID].arrNewData.arrSz + 7) / 8;
                    // qDebug() << "CMD_ARR_SZ:Channel"<<curDevID<<"Size="<<GET_ARR_SZ(sCanMsg.dwMsgId);
                    break;
                case SCAN_EN_OK: // ответ на разрешение/запрет сканирования адресов массива
                    pIxxat->m_isScanOK = true; // девайс дал ответ "SCAN_EN_OK"
                    break;
                case FILE_SAVE_OK:  // девайс готов на прием файла переменных или
                                    // девайс закончил прием файла переменных
                    pIxxat->m_isVarSave2FlashOK = true; // девайс дал ответ "VAR_SAVE_OK"
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    // qDebug()<<QString("Dev->Msg:FILE_SAVE_OK Unit = %1").arg(curDevID);
                    break;
                case FILE_LOAD_OK:  // девайс готов передавать файл переменных или
                                    // девайс закончил передавать файл переменных
                    pIxxat->m_isVarLoad2HostOK = true; // девайс дал ответ "FILE_LOAD_OK"
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    pIxxat->m_varFileSize = GET_FILE_SZ(sCanMsg.dwMsgId);
                    if(pIxxat->m_varFileSize >= 0xFFFF)
                        emit signalVarFileError(curDevID); // "Файл переменных на борту узла %1 отсутствует"
                    fileData.clear();
                    break;
                case SYNC_DATA_END: // сигнал синхронизации для РС: конец передачи данных канала
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    emit signalChannelDone(curDevID);
                    // pIxxat->getDevId();
                    break;
                }
                break; // case CAN_CMD -> to while(!stop)

            // *** ДАННЫЕ ПРОСТЫХ ПЕРЕМЕННЫХ ***************************************************
            case VAR_DAT: {
                QMutexLocker lockerVarData(&m_mainWnd_4DlgIXXAT->m_mutexUpdateVarData);
                curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                varCurData.varID = GET_VAR_ID(sCanMsg.dwMsgId);
                // Преобразуем 8 байт sCanMsg.abData[x] в два слова uint32
                // Порядок байтов little-endian (младший байт в конце)
                varCurData.data0 = BYTES2INT_LE(sCanMsg.abData, 0);
                varCurData.data1 = BYTES2INT_LE(sCanMsg.abData, 4);
                // заталкиваем данные в вектор с текущим ID канала
                propTmp->property[curDevID].varNewData.vecCurData.emplace_back(varCurData);
                // если получили данные от всех переменных в пакете
                if(propTmp->property[curDevID].varNewData.vecCurData.size() ==
                    propTmp->property[curDevID].varNewData.numVarsDev) {
                    // передадим эти данные на конвертацию значений
                    // в значения, определяемые типом переменных
                    lockerVarData.unlock(); // разлочем мьютекс синхронизации
                    // сигнал обновления данных для слота MainWindow::slotUpdateVarData()
                    emit signalUpdateVarData(curDevID, propTmp->property[curDevID].varNewData.vecCurData);
                    propTmp->property[curDevID].varNewData.vecCurData.clear();
                }
                break; // case CAN_DAT -> to while(!stop)
            }

            // *** ДАННЫЕ МАССИВОВ ********************************************************
            case ARR_DAT: { // данные поступают дампом памяти сразу по всем адресам массива, начиная от базового
                QMutexLocker lockerArrData(&m_mainWnd_4DlgIXXAT->m_mutexUpdateArrData);
                curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                arrCurData.varID = GET_ARR_ID(sCanMsg.dwMsgId);
                // Преобразуем 8 байт sCanMsg.abData[x] в два слова uint32
                // (порядок байтов little-endian (младший байт в конце )
                arrCurData.data0 = BYTES2INT_LE(sCanMsg.abData, 0);
                arrCurData.data1 = BYTES2INT_LE(sCanMsg.abData, 4);
                // заталкиваем данные в вектор с текущим ID канала
                propTmp->property[curDevID].arrNewData.vecCurData.emplace_back(arrCurData);
                // если полученный размер массива vecCurData равен числу фреймов этого массива
                if(propTmp->property[curDevID].arrNewData.vecCurData.size() ==
                    propTmp->property[curDevID].arrNewData.nArrFrames) {
                    lockerArrData.unlock(); // разлочем мьютекс синхронизации
                    emit signalUpdateArrData(curDevID, propTmp->property[curDevID].arrNewData);
                    propTmp->property[curDevID].arrNewData.vecCurData.clear();
                }
                break;
            } // case ARR_DAT

            // *** РАБОТА С ФАЙЛОМ ПЕРЕМЕННЫХ ********************************************************
            case VAR_FILE: {
                fileData.push_back(sCanMsg.abData[0]);
                fileData.push_back(sCanMsg.abData[1]);
                fileData.push_back(sCanMsg.abData[2]);
                fileData.push_back(sCanMsg.abData[3]);
                fileData.push_back(sCanMsg.abData[4]);
                fileData.push_back(sCanMsg.abData[5]);
                fileData.push_back(sCanMsg.abData[6]);
                fileData.push_back(sCanMsg.abData[7]);
                uint32_t fsize = fileData.size();
                // qDebug()<<"fileData.size()"<<fsize;
                if(fsize >= pIxxat->m_varFileSize + 0x08) { // + КС(8 байт)
                    curDevID = GET_ID_DEV(sCanMsg.dwMsgId) - 1;
                    // qDebug()<<"fileData.size()"<<fileData.size();
                    emit signalReadVarFileFlash(curDevID, fileData);
                    break;
                }
                break; // to while(!stop)
            } // case VAR_FILE
            } // switch(cmdId)
        } // else if(res == VCI_OK)
        else {
            curDevID = -1;
            emit signalDevIsConnected(curDevID);
            stop = true; // выход из потока
            qDebug() << "ReadThread ERROR";
        }
    }
    thread->wait(); // здесь wait() нужен !!! -без него поток остается висеть
    pIxxat->m_readThread = nullptr;
    qDebug() << "MyIXXATReadThread is Finished";
}

