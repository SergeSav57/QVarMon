#include <QtWidgets>
#include <QFileDialog>
#include <QToolBar>
#include <QTextEdit>
#include <LoggingCategories.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "crc32.h"
#include "dlgvaraddr.h"

#ifdef Q_OS_WINDOWS
// для отслеживания подключения/отключения девайса под OS Windows
#include <windows.h> // !!! <windows.h> надо подключать перед <dbt.h>
#include <dbt.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <initguid.h>
//-------------------
#endif

// механизм отлова SIGSEGV =========================================
#define SIGSEGV_GET 0
#if SIGSEGV_GET
#include <windows.h>
#include <signal.h>
void handler_sigsegv(int signum)
{
    QMessageBox::critical(nullptr,"SIGSEGV Error!","POSIX Signal : Dereference of null pointer",QMessageBox::Ok);
    // открепить обработчик и явно завершить приложение
    signal(signum, SIG_DFL);
    exit(3);
}

void handler_sigfpe(int signum)
{
    QMessageBox::critical(nullptr,"SIGFPE Error!","POSIX Signal : Division by zero",QMessageBox::Ok);
    // открепить обработчик и явно завершить приложение
    signal(signum, SIG_DFL);
    exit(3);
}
// тестовые ф-ции, приводящие к SIGSEGV и SIGFPE
void fall1()
{   // Dereference of null pointer (разыменование нулевого указателя)
    // int* p = 0;
    // *p = 10;
    // Q_UNUSED(*p);
}
void fall2()
{   // Division by zero (деление на 0)
    // int x;
    // x = 1/0;
    // Q_UNUSED(x);
}
#endif
//==============================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if SIGSEGV_GET // работает как-то, но ограничено -надо еще с другими сигналами проверить
    // установим наши обработчики на два сигнала
    signal(SIGSEGV, handler_sigsegv);
    signal(SIGFPE, handler_sigfpe);
    fall1();
    fall2();
#endif

    qDebug(logInfo()) << " >>> Запуск программы >>>";

#ifdef Q_OS_WINDOWS
    // запретить Windows переходить в спящий режим и отключать дисплей (только для Windows)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif

    // m_dlgCreateVarFile  = new DlgCreateVarFile(this);
    m_dlgTableArray     = new DlgTableArray(this);
    m_dlgVarSel         = new DlgVarSel(this);
    m_dlgIxxat          = new DlgIXXAT(this);
    m_dlgUart           = new DlgUART(this);
    m_dlgAbout          = new DlgAbout(this);
    m_dlgHowToUse       = new DlgHowToUse(this);
    m_dlgSelectCfg      = new DlgSelectCfg(this);
    m_dlgAddAddress     = new DlgAddAddress(this);
    m_myLocalServer     = new MyLocalServer("MyLocalServer", this);

    initMainWndInterface();
    m_dlgSelectCfg->show();
}

MainWindow::~MainWindow()
{
    if(ui) // ui удаляется в closeEvent ()
        delete ui;
}

void MainWindow::clearAllVectors()
{
    this->setWindowTitle("QVarMon : " + m_unitProp.sCfgName);

    m_dlgVarSel->m_VarAllGlobalVector.clear();
    m_dlgVarSel->m_VarSelGlobalVector.clear();
    m_dlgVarSel->m_ArrAllGlobalVector.clear();
    m_dlgVarSel->m_ArrSelGlobalVector.clear();
    m_dlgVarSel->m_ArrSelGlobalVectorTmp.clear();
    m_dlgVarSel->m_chartSelVectorTmp.clear();
    m_dlgVarSel->m_recSelVector.clear();
    m_dlgVarSel->m_recSelVectorTmp.clear();
    m_dlgVarSel->m_findVector.clear();

    for(int i = 0; i < m_dlgTableWorkVector.size(); ++i) {
        m_dlgTableWorkVector[i]->m_varSelVector.clear();
        m_dlgTableWorkVector[i]->m_varSelVectorTmp.clear();
    }

    // установим по умолчанию показ таблиц окна выбора параметров
    m_dlgVarSel->setComboParamIndex(ALL_PARAM);
    // очистим таблицы окна выбора параметров
    emit signalTableClear();
    m_dlgIxxat->getDevId();
}

void MainWindow::slotStartMainWnd(uint32_t startType, unitProp_t &prop)
{
    switch(startType) {
    case START_LOAD_FLASH: // загрузка файлов из флеш
        clearAllVectors();
        m_dlgIxxat->loadVarFileFromAllBoard(prop);
        break;
    case START_LOAD_FILE: // загрузка файлов с диска
        clearAllVectors();
        // m_dlgCreateVarFile->readVarFileFromCfg(prop);
        readVarFileFromCfg(prop);
        break;
    case START_NOLOAD:
        break;
    }
}

// =========================================================================

void MainWindow::initMainWndInterface()
{
    emit signalSendMainWndHandle(this); // передача дискриптора окнам, созданным независимо от MainWindow

    QRect rect = this->geometry();
    // примерное начальное положение горизонтального разделителя
    ui->splitter_2->setSizes(QList<int>() << rect.width()*0.7 << rect.width()*0.3);
    // растягиваем таблицу параметров (левая часть), а панель интерфейса (правая часть) остается прежней
    ui->splitter_2->setStretchFactor(0,1);
    // затемним немного сплиттеры
    ui->splitter->setStyleSheet("QSplitter::handle{background:lightgray;}");
    ui->splitter_2->setStyleSheet("QSplitter::handle{background:lightgray;}");

    ui->groupBoxRecord->setStyleSheet(QString("QGroupBox {color: %1}").arg(myColorBlue.name()));
    ui->groupBoxChanels->setStyleSheet(QString("QGroupBox {color: %1}").arg(myColorBlue.name()));

    ui->tabWidgetInterface->clear();
    ui->tabWidgetInterface->addTab(m_dlgIxxat, "CAN");
    ui->tabWidgetInterface->addTab(m_dlgUart, "UART");

    ui->tabWidgetArray->clear();
    ui->tabWidgetArray->addTab(m_dlgTableArray, "Массивы");

    // иконки тулбара:
    const QIcon iconNewTab(TABNEW_ICON);
    const QIcon iconOpenTab(TABOPEN_ICON);
    const QIcon iconSaveTab(SAVE_ICON);
    const QIcon iconSelectParam(TABLE_ICON);
    const QIcon iconAddVar(ADDVAR_ICON);
    const QIcon iconSelectConf(SPISOK_ICON);
    const QIcon iconShowChart(CHART_ICON);
    const QIcon iconAbout(HELP_ICON);

    QToolBar *toolbar = new QToolBar("myToolBar");
    addToolBar(Qt::TopToolBarArea, toolbar);

    toolbar->addAction(iconOpenTab,         "Открыть вкладку", this, SLOT(on_action_FILE_OPEN_TAB_triggered()));
    toolbar->addSeparator();
    toolbar->addAction(iconNewTab,          "Создать новую вкладку", this, SLOT(on_action_FILE_CREATE_NEW_TAB_triggered()));
    toolbar->addAction(iconSaveTab,         "Сохранить вкладку", this, SLOT(on_action_FILE_SAVE_TAB_triggered()));
    toolbar->addSeparator();
    toolbar->addAction(iconSelectParam,     "Выбор параметров", this, SLOT(on_action_SELECT_PARAM_triggered()));
    toolbar->addAction(iconAddVar,          "Добавить адрес переменной", this, SLOT(on_action_ADDADDRESS_triggered()));
    toolbar->addAction(iconSelectConf,      "Выбор конфигурации загрузки", this, SLOT(on_action_SELECT_CONF_triggered()));
    toolbar->addSeparator();
    toolbar->addAction(iconShowChart,       "Вывод на график", this, SLOT(on_action_SHOW_CHART_triggered()));
    toolbar->addSeparator();
    toolbar->addAction(iconAbout,           "О программе", this, SLOT(on_action_ABOUT_triggered()));

    // установим таймер для мигания кнопки записи и его сигнал
    m_recTimer = new QTimer(this);
    connect(m_recTimer, &QTimer::timeout, this, &MainWindow::slotRecTimerAlarm);
    m_ledsTimer = new QTimer();
    connect(m_ledsTimer, &QTimer::timeout, this, &MainWindow::slotLedsTimerAlarm);
    m_ledsTimer->start(200);

    ui->EDIT_RECORD_TIME->setText("00:00:00");
    m_recFile_opened = false;
    m_recFile_started = false;

    ui->BTN_START_RECORD->setEnabled(false);
    ui->BTN_SEL_REC_PARAM->setEnabled(false);

    statusBar()->showMessage("Ready");

    initChannelLeds();

    ui->tabWidget4Work->clear();
    ui->tabWidget4Work->setStyleSheet(
        "QTabBar::tab:selected {"\
        "color: rgb(50,80,240);"\
        "}"\
        "QTabBar::tab:!selected {"\
        "color: rgb(130,130,200);"\
        "}");

    // создаем первую вкладку (!!! Потом: если в пред.работе была использована конкретная вкладка, открыть ее или набор вкладок)
    emit ui->action_FILE_CREATE_NEW_TAB->triggered();
    // переключатель вкладок
    connect(ui->tabWidget4Work, &QTabWidget::currentChanged, this, &MainWindow::slotTabWorkChanged);
    connect(ui->tabWidget4Work, &QTabWidget::tabCloseRequested, this, &MainWindow::slotTabWorkClosed);
    // инит. CAN -----------------
    m_dlgIxxat->initDlg();
    // регистрация адаптера IXXAT
    RegDevice();
    // проверка соединения с адаптером CAN
    m_dlgIxxat->checkIXXATConnect();
    // connect для ChangeDevice()
    connect(this, &MainWindow::signalChangeDevice, m_dlgIxxat, &DlgIXXAT::slotChangeDevice,
            Qt::DirectConnection);
    connect(this, &MainWindow::signalTableClear, m_dlgVarSel, &DlgVarSel::slotTableClear);
}

void MainWindow::startDlgChart()
{
    QString  cmd{};
#ifdef QT_DEBUG
    cmd = "d:\\WORKSPACE_GD\\Qt_proj\\build-QVarChart-Qt_6_7_2_MinGW_64_bit\\debug\\QVarChart.exe";
#else
    cmd = "QVarChart.exe";
#endif

    QProcess *proc = new QProcess(this);
    if(!proc->startDetached(cmd)) {
        delete proc;
        qDebug()<<"Do not start DlgChart!";
        return;
    }
    proc->waitForStarted(200);
    proc->deleteLater();
}

void MainWindow::startDlgArray(var_t &var, QPoint *pnt)
{   Q_UNUSED(pnt);
    QString  cmd{};
#ifdef QT_DEBUG
    cmd = "d:\\WORKSPACE_GD\\Qt_proj\\build-QVarArr-Qt_6_7_2_MinGW_64_bit\\debug\\QVarArr.exe";
#else
    cmd = "QVarArr.exe";
#endif

    for(int i = 0; i < (int)m_arrDlgPropVector.size(); ++i) {
        if((m_arrDlgPropVector[i].varName == var.varID.varName) &&
            (m_arrDlgPropVector[i].unitID == var.varID.unitID)) {
            m_myLocalServer->sendToClient(m_arrDlgPropVector[i].socket, "Blink");
            return;
        }
    }

    arrayDlgProp_t prop;
    prop.socket     = 0; // определяется сервером после получения ответа от открытого окна массива
    prop.varName    = var.varID.varName;
    prop.unitName   = var.varID.unitName;
    prop.varAltType = var.varType.altType;
    prop.unitID     = var.varID.unitID;
    prop.varAddr    = var.varProp.varAddr;
    prop.varSize    = var.varProp.varSize;
    prop.varID      = var.varID.varID;
    prop.varTypeID  = var.varType.typeID;
    prop.typeSize   = var.varType.typeSize;
    prop.arrRow     = var.varProp.arrRow;
    prop.arrCol     = var.varProp.arrCol;
    prop.proc       = new QProcess(this);
    // эту стр-ру далее передадим в окно массива
    m_arrDlgPropVector.push_back(prop);

    QStringList list;
    list << "#"; // для идентификации, что запуск идет от QVarMon
    if(!prop.proc->startDetached(cmd, list)) {
        delete prop.proc;
        qDebug()<<"Do not start DlgArray!";
        return;
    }
    prop.proc->waitForStarted(200);
    prop.proc->deleteLater();

    // разрешим девайсу сканирование адресов массива
    m_dlgIxxat->sendCommand(CMD_SCAN_EN,
                            var.varID.unitID,
                            true, // разрешение
                            var.varID.varID);
}

void MainWindow::slotCloseDlgArr(int index)
{
    // запретим девайсу сканирование адресов массива
    m_dlgIxxat->sendCommand(CMD_SCAN_EN,
                            m_arrDlgPropVector[index].unitID,
                            false, // запрет
                            m_arrDlgPropVector[index].varID);
    // Удаляем сокет и все свойства окна массива из списка
    m_arrDlgPropVector.removeAt(index);
}

void MainWindow::initChannelLeds()
{   /// инит светиков каналов, надписей и всплывающего окна к ним
    // текущий фонт : найдем максимальную длину текста всплывающего меню
    QFont font = ui->frame->property("font").value<QFont>();
    QFontMetricsF fm(font);
    // qDebug() << "Font" << font.family() << font.pointSize();
    qreal widthMenu = 0;
    qreal heightMenu = 0;
    QRectF bndRectMenu{};
    // всплывающее меню канала (найдем максимальный размер прямоугольника)
    for(int j = 0; j < LED_STRING_MENU.size(); ++j) {
        bndRectMenu = fm.boundingRect(LED_STRING_MENU[j]);
        widthMenu = qMax(widthMenu, bndRectMenu.width());
        heightMenu = qMax(heightMenu, bndRectMenu.height());
    }

    unitsProperty_t prop{};
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        // инициализация светиков
        prop.ledsChVec.lbLed = new QLabel(ui->groupBoxChanels);
        prop.ledsChVec.lbLed->setPixmap(QIcon(LED_GRAY_ICON).pixmap(24,24));
        prop.ledsChVec.lbLed->installEventFilter(this); // для клика на светике
        // имя канала
        prop.ledsChVec.lbTextName = new QLabel(ui->groupBoxChanels);
        prop.ledsChVec.lbTextName->setFixedWidth(100);
        prop.ledsChVec.lbStringName = "Канал";
        // номер канала
        prop.ledsChVec.lbTextCh = new QLabel(ui->groupBoxChanels);
        prop.ledsChVec.lbTextCh->setFixedWidth(20);
        prop.ledsChVec.lbStringCh = QString("[%1]").arg(i);
        // всплывающее окно меню
        prop.ledsChVec.listWnd = new QListWidget(ui->frame);
        prop.ledsChVec.listWnd->setStyleSheet(
            "QListWidget:item {background-color:rgb(255,255,170); color:blue;}"
            "QListWidget::Item:hover{background:skyblue;}");
        prop.ledsChVec.listWnd->setGeometry(0, 0, widthMenu + 15,
                                            LED_STRING_MENU.size() * heightMenu + 8);
        prop.ledsChVec.listWnd->addItems(LED_STRING_MENU);
        prop.ledsChVec.listWnd->setSelectionMode(QAbstractItemView::SingleSelection);

        // инит свойств девайсов
        prop.isPresent = false;
        prop.isSaveVarFile = false;
        prop.isLoadVarFileFlash = false;
        prop.isLoadVarFileDisk = false;

        // сделаем невидимыми светики и надписи после предыдущей инициализации
        prop.ledsChVec.lbLed->setVisible(false);
        prop.ledsChVec.lbTextName->setVisible(false);
        prop.ledsChVec.lbTextCh->setVisible(false);
        prop.ledsChVec.listWnd->hide();

        m_unitProp.property[i] = prop;
    }
}

void MainWindow::setChannelLeds()
{   // расчет геометрических позиций и установка светиков
    int deltaY = 20;
    int stepY = 0;
    int yText = 4;
    int yWnd  = 24;
    QPoint pnt{};
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if((m_unitProp.property[i].isPresent) ||
            (m_unitProp.property[i].isOnlyView))
        {
            if(m_unitProp.property[i].isPresent) {
                if(m_unitProp.property[i].isLoadVarFileDisk || m_unitProp.property[i].isLoadVarFileFlash)
                    m_unitProp.property[i].ledsChVec.lbLed->setPixmap(QIcon(LED_GREEN_ICON).pixmap(24,24));
                else
                    m_unitProp.property[i].ledsChVec.lbLed->setPixmap(QIcon(LED_YELLOW_ICON).pixmap(24,24));
            }
            else
                m_unitProp.property[i].ledsChVec.lbLed->setPixmap(QIcon(LED_GRAY_ICON).pixmap(24,24));
            pnt.setX(-4);
            pnt.setY(stepY * deltaY + deltaY);
            m_unitProp.property[i].ledsChVec.lbLed->move(pnt);
            // имя канала
            pnt.setX(18);
            pnt.setY(stepY * deltaY + deltaY + yText);
            m_unitProp.property[i].ledsChVec.lbTextName->move(pnt);
            // номер канала
            pnt.setX(18 + 45);
            m_unitProp.property[i].ledsChVec.lbTextCh->move(pnt);
            // всплывающее окно меню
            pnt.setX(40);
            pnt.setY(stepY * deltaY + deltaY + yWnd);
            m_unitProp.property[i].ledsChVec.listWnd->move(pnt);
            // сделаем видимыми
            m_unitProp.property[i].ledsChVec.lbLed->setVisible(true);
            m_unitProp.property[i].ledsChVec.lbTextName->setVisible(true);
            m_unitProp.property[i].ledsChVec.lbTextCh->setVisible(true);

            ++stepY;
        }
        else {
            m_unitProp.property[i].ledsChVec.lbLed->setVisible(false);
            m_unitProp.property[i].ledsChVec.lbTextName->setVisible(false);
            m_unitProp.property[i].ledsChVec.lbTextCh->setVisible(false);
        }
    }
}

void MainWindow::slotLedsTimerAlarm()
{	// перерисовка светиков каналов(узлов) по таймеру (период 200 мс)
    static uint32_t cntAlarm = 0;
    static bool flag = true;
    uint32_t count = 0;

    // в первые 2 секунды после запуска приложения, определяем, какие каналы подключены
    // командой CMD_GET_DEV_ID для всех возможных каналов
    if(flag && (++cntAlarm % 2 == 0)) {
        m_dlgIxxat->getDevId();
    }

    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_unitProp.property[i].isPresent) {
            m_unitProp.property[i].ledsChVec.lbLed->setVisible(true);
            m_unitProp.property[i].ledsChVec.lbTextName->setVisible(true);
            m_unitProp.property[i].ledsChVec.lbTextName->setText(m_unitProp.property[i].ledsChVec.lbStringName);
            m_unitProp.property[i].ledsChVec.lbTextCh->setVisible(true);
            m_unitProp.property[i].ledsChVec.lbTextCh->setText(m_unitProp.property[i].ledsChVec.lbStringCh);
            m_unitProp.property[i].ledsChVec.lbLed->setPixmap(QIcon(LED_GREEN_ICON).pixmap(24,24));
            ++count;
        }
    }
    // if(m_unitProp.numChannels != count)
    m_unitProp.numChannels = count;
    setChannelLeds();

    // через 2 секунды после запуска приложения очистим все старые запросы CAN
    if(flag && (count > 0) && (cntAlarm == 10)) {
        flag = false; // сюда больше не попадаем
        qDebug()<<">>>>> Всего узлов :"<<count;
        m_dlgIxxat->clearAllRequest();
    }
}

void MainWindow::slotDevIsConnected(int devId)
{   //
    static int nLedsRegOld = 0; // регистр светиков, которые уже были включены
    static int nLedsReg = 0;    // регистр включенных светиков

    if(devId < 0) { // если ни один узел не подключен
        for(int i = 0; i < NUMCHANNELMAX; ++i) {
            m_unitProp.property[i].isPresent = false;
            m_unitProp.property[i].ledsChVec.lbLed->setPixmap(QIcon(LED_GRAY_ICON).pixmap(24,24));
            m_unitProp.property[i].ledsChVec.lbStringName = "Канал";
            m_unitProp.property[i].ledsChVec.lbTextName->setText(m_unitProp.property[i].ledsChVec.lbStringName);
            m_unitProp.property[i].ledsChVec.lbStringCh = QString("[%1]").arg(i);
            m_unitProp.property[i].ledsChVec.lbTextCh->setText(m_unitProp.property[i].ledsChVec.lbStringCh);
        }
        nLedsReg = 0;
        nLedsRegOld = 0;
        return;
    }

    nLedsReg |= (1 << devId);
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if ((((nLedsReg    >> i) & 0x01) == 1) &&       // включен
            (((nLedsRegOld >> i) & 0x01) == 0)) {       // но был вЫключен
            m_unitProp.property[i].isPresent = true;    // включился
            m_unitProp.property[i].unitID = i;
        }
        else if((((nLedsReg    >> i) & 0x01) == 1) &&   // включен
                (((nLedsRegOld >> i) & 0x01) == 1)) {   // и был включен
            m_unitProp.property[i].isPresent = true;    // продолжает быть включенным
            m_unitProp.property[i].unitID = i;
        }
        else {
            m_unitProp.property[i].isPresent = false;   // вЫключен
            m_unitProp.property[i].unitID = -1;
        }
    }
    nLedsRegOld = nLedsReg;
}

//=============================================================================

void MainWindow::slotSendSelParamToDevice()
{	// отправить девайсу адреса параметров для сканирования
    // -------------------простые переменные --------------------------
    // (все адреса со всех вкладок)
    // распределим переменные по номерам узлов
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        m_unitProp.property[i].varNewData.numVarsHost = 0;
        m_unitProp.property[i].varNewData.numVarsDev = 0;
    }
    // определим число передаваемых переменных:
    for(int i = 0; i < (int)m_dlgVarSel->m_VarSelGlobalVector.size(); ++i) {
        uint32_t id = m_dlgVarSel->m_VarSelGlobalVector[i].varID.unitID;
        // инкремент числа переменных в посылке хоста для каждого узла
        ++m_unitProp.property[id].varNewData.numVarsHost;
    }
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_unitProp.property[i].varNewData.numVarsHost == 0)
            continue;
        // команда для девайса : новые переменные
        // -девайс очищает массив адресов сканирования и устанавливает новое число этих адресов
        // число передаваемых переменных:
        m_dlgIxxat->sendCommand(CMD_NEW_VAR, i, m_unitProp.property[i].varNewData.numVarsHost, 0);
        if(waitFlagEn(m_dlgIxxat->m_isNewVarOK, 50)) {
            // qDebug()<<" waitFlagEn : m_isNewDataOK";
            // передаем новые адреса для сканирования
            m_dlgIxxat->sendData(VAR_DAT, i);
        }
    }

    // -------------------массивы переменных ---------------------------
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        m_unitProp.property[i].arrNewData.numArrs = 0;
        m_unitProp.property[i].arrNewData.arrSz = 0;
        m_unitProp.property[i].arrNewData.nArrFrames = 0;
    }
    for(int i = 0; i < m_dlgVarSel->m_ArrSelGlobalVector.size(); ++i) {
        uint32_t id = m_dlgVarSel->m_ArrSelGlobalVector[i].varID.unitID;
        ++m_unitProp.property[id].arrNewData.numArrs;
    }
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_unitProp.property[i].arrNewData.numArrs == 0)
            continue; // в узле нет массивов для передачи девайсу
        // команда для девайса : новые массивы
        // (девайс очищает массив адресов сканирования и устанавливает новое число этих адресов)
        m_dlgIxxat->sendCommand(CMD_NEW_ARR, i, m_unitProp.property[i].arrNewData.numArrs, 0); // число массивов узла
        if(waitFlagEn(m_dlgIxxat->m_isNewArrOK, 50)) {
            // передаем новые адреса для сканирования
            m_dlgIxxat->sendData(ARR_DAT, i);
            for(int i = 0; i < m_arrDlgPropVector.size(); ++i) {
                // восстановим разрешения на сканирование массивов, если они были
                m_dlgIxxat->sendCommand(CMD_SCAN_EN,
                                        m_arrDlgPropVector[i].unitID,
                                        true, // разрешение
                                        m_arrDlgPropVector[i].varID);
            }
        }
    }
}

// === обслуживание вкладок ============================================
void MainWindow::createNewTabWork(int index, bool opened)
{
    DlgTableWork *newTabWork = new DlgTableWork(this);
    // имя вкладки по умолчанию
    newTabWork->m_sTableName = QString("Вкладка %1").arg(index + 1);
    newTabWork->m_fOpened = opened; // true : вкладка отрыта
    newTabWork->m_index = index;
    m_dlgTableWorkVector.emplace_back(newTabWork);
    ui->tabWidget4Work->addTab(newTabWork, newTabWork->m_sTableName);
    this->m_dlgVarSel->setComboTabsName(newTabWork->m_sTableName);
    this->m_dlgVarSel->setComboTabsIndex(index);
    setCurrentTabWork(index);
}

void MainWindow::slotTabWorkChanged(int index)
{
    tabWorkActivated(index);
    setTabNewName();
}

void MainWindow::slotTabWorkClosed(int index)
{
    for(int i = 0; i < m_dlgTableWorkVector.size(); ++i) {
        // ищем удаляемую вкладку
        if(m_dlgTableWorkVector[i]->m_index == index) {
            // в списке ALL (все загруженные параметры) выключим чекбоксы удаляемых элементов вкладки
            for(int j = 0; j < m_dlgVarSel->m_VarAllGlobalVector.size(); ++j) { // все загруженные параметры
                for(int n = 0; n < m_dlgTableWorkVector[i]->m_varSelVectorTmp.size(); ++n) { // выбранные во вкладке
                    if(checkVarId(m_dlgVarSel->m_VarAllGlobalVector[j],
                                  m_dlgTableWorkVector[i]->m_varSelVectorTmp[n])) {
                        m_dlgVarSel->m_VarAllGlobalVector[j].varFlags.f_Monitor = false; // чекбокс будет выключен
                    }
                }
            }
            // удаляем вкладку из списка вкладок
            // m_dlgTableWorkVector.remove(i);
            m_dlgTableWorkVector.erase(m_dlgTableWorkVector.cbegin() + i);
            // удаляем вкладку из GUI
            ui->tabWidget4Work->removeTab(i);
            // удаляем имя вкладки из COMBOBOX_TABS
            this->m_dlgVarSel->removeComboTabsIndex(i);
            break;
        }
    }
}

void MainWindow::setCurrentTabWork(int index)
{
    ui->tabWidget4Work->setCurrentIndex(index); // вкладку открываем и делаем текущей
}

void MainWindow::setTabWorkName(int index, const QString &name)
{
    ui->tabWidget4Work->tabBar()->setTabText(index, name);
    m_dlgTableWorkVector[index]->m_sTableName = name;
}

QString MainWindow::nameTabWork(int index)
{
    // return m_dlgTableWorkVector[index]->m_sTableName;
    // или так
    return ui->tabWidget4Work->tabText(index);
}

void MainWindow::tabWorkActivated(int index)
{
    for(int i = 0; i < m_dlgTableWorkVector.size(); ++i)
        m_dlgTableWorkVector[i]->m_fOpened = false;
    if((index < 0) || (index > m_dlgTableWorkVector.size() -1))
        return;
    m_dlgTableWorkVector[index]->m_fOpened = true;
    ui->tabWidget4Work->setCurrentIndex(index);
    this->m_dlgVarSel->setComboTabsIndex(index);
}

int MainWindow::countTabWork()
{
    return ui->tabWidget4Work->count();
}

int MainWindow::currentTabWork()
{
    return ui->tabWidget4Work->currentIndex();
}

static int curUnit = -1;
void MainWindow::ledCh_pressed(int pos)
{
    curUnit = pos;
    if(!m_unitProp.property[pos].isPresent)
        return;
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_unitProp.property[i].isPresent)
            m_unitProp.property[i].ledsChVec.listWnd->hide();
    }

    if(m_unitProp.property[pos].unitVarFile.isEmpty()) {
        if(QMessageBox::Yes == QMessageBox::information(nullptr, "Внимание",
                              "Открыть окно выбора конфигурации загрузки?",
                              QMessageBox::Yes|QMessageBox::No)) {
            m_dlgSelectCfg->show();
        }
        return;
    }
    m_unitProp.property[pos].ledsChVec.listWnd->show();
    connect(m_unitProp.property[pos].ledsChVec.listWnd, &QListWidget::itemClicked,
            this, &MainWindow::slotLedChMenu, Qt::SingleShotConnection); // одноразовая обработка сигнала
}

void MainWindow::slotLedChMenu(QListWidgetItem *item)
{
    QString str = item->text();
    if(str == LED_STRING_MENU[0]) { // "Закрыть"
        for(int i = 0; i < NUMCHANNELMAX; ++i) {
            // if(m_unitProp.property[i].isPresent)
                m_unitProp.property[i].ledsChVec.listWnd->hide();
        }
    }
    if(str == LED_STRING_MENU[1]) { // "Загрузить параметры с борта"
        m_dlgIxxat->loadVarFileFromUnitBoard(curUnit);
    }
    if(str == LED_STRING_MENU[2]) { // "Загрузить параметры из файла"
        if(m_unitProp.property[curUnit].isLoadVarFileFlash) {
            if(QMessageBox::No ==
                QMessageBox::question(nullptr, "",
                                      "Файл параметров уже загружен с борта узла.\nЖелаете его заменить?",
                                      QMessageBox::Yes|QMessageBox::No)) {
                return;
            }
            m_unitProp.property[curUnit].fReloadFile = true;
        }
        if(m_unitProp.property[curUnit].isLoadVarFileDisk) {
            if(QMessageBox::No ==
                QMessageBox::question(nullptr, "",
                                      "Файл параметров уже загружен с диска.\nЖелаете его заменить?",
                                      QMessageBox::Yes|QMessageBox::No)) {
                return;
            }
            m_unitProp.property[curUnit].fReloadFile = true;
        }
        QString filePath = QFileDialog::getOpenFileName(this,
                            QString("Открыть файл переменных узла %1").arg(curUnit),
                            ".", "Файлы переменных (*.svar)");
        QFile file;
        file.setFileName(filePath);
        // загружаем файлы для одного узла
        // m_dlgCreateVarFile->readVarFile(curUnit, filePath, false);
        readVarFile(curUnit, filePath, false);

    }
    if(str == LED_STRING_MENU[3]) { // "Сохранить файл параметров на борт"
        QString filePath = QFileDialog::getOpenFileName(this,
            QString("Открыть файл переменных узла %1").arg(curUnit),
            ".", "Файлы переменных (*.svar)");
        QFile file;
        file.setFileName(filePath);
        m_dlgIxxat->saveVarFileToUnitBoard(curUnit, file);
    }
    if(str == LED_STRING_MENU[4]) { // "Сохранить параметры с борта в файл"
        m_unitProp.property[curUnit].isSaveVarFile = true;
        m_dlgIxxat->loadVarFileFromUnitBoard(curUnit);
    }
    m_unitProp.property[curUnit].ledsChVec.listWnd->hide();
}

void MainWindow::RegDevice(/*HANDLE hwnd*/)
{   // регистрация драйвера преобразователя CAN->USB IXXAT
    // для отслеживания подключения/отключения устройства
#ifdef Q_OS_WINDOWS
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    uint8_t i;
    for(i=0; i<sizeof(GuidInterfaceList) / sizeof(GUID); ++i) {
        devInt.dbcc_classguid = GuidInterfaceList[i];
        HDEVNOTIFY hDevNotify =
            RegisterDeviceNotification((HANDLE)winId(), &devInt, DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
        if(!hDevNotify) {
            GetLastError();
            qDebug(logCritical()) << "Ошибка регистрации GuidInterface";
        }
    }
#endif
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if (obj == m_unitProp.property[i].ledsChVec.lbLed) {
            if(m_unitProp.property[i].isPresent) {
                if(event->type() == QEvent::MouseButtonPress) {
                    ledCh_pressed(i);
                    break;
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

bool MainWindow::nativeEvent(const QByteArray &eventType ,void *message, qintptr *result)
{   // "qintptr" является определением типа для qint64 или qint32 для соответствующих систем.
    // ! Сигнатура виртуальной QWidget::nativeEvent() именно такая.
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    MSG* msg = reinterpret_cast<MSG*>(message);
#ifdef Q_OS_WINDOWS
    if(msg->message == WM_DEVICECHANGE) {
        // qDebug() << " !!! MainWindow::nativeEvent():WM_DEVICECHANGE";
        emit signalChangeDevice(msg);
        return true;
    }
#endif
    return false;
}

void MainWindow::slotCreateBinEditDlg(var_t &var, QPoint *pnt)
{   // pos -номер позицию переменной в векторе параметров
    // pnt -точка расположения итема переменной в списке в глобальных координатах экрана
    int binWndCnt = (int)m_dlgBinEditVector.size(); // число открытых окон бинэдитора
    for(int i = 0; i < binWndCnt; ++i) {
        if(checkVarId(m_dlgBinEditVector[i]->m_var, var)) {
            m_dlgBinEditVector[i]->Blink();
            return;
        }
    }
    DlgBinEditor* dlgBin = new DlgBinEditor(this);
    dlgBin->m_dlg = dlgBin;
    m_dlgBinEditVector.emplace_back(dlgBin);
    // точка начала расположения окна бинэдитора в локальных координатах MainWindow
    int curTab = currentTabWork();
    QPoint pntLoc = m_dlgTableWorkVector[curTab]->mapToParent(*pnt);
    int x = pntLoc.x() + 8;
    int y = pntLoc.y() + 25;
    dlgBin->move(x, y);
    dlgBin->ShowBinEditor(var);
}

void MainWindow::DeleteDlgBinEdit(DlgBinEditor* dlgBin)
{
    int dlgCnt = (int)m_dlgBinEditVector.size(); // число открытых окон бинэдитора
    for(int i = 0; i < dlgCnt; ++i) {
        if(m_dlgBinEditVector.at(i)->m_dlg == dlgBin) {
            // m_dlgBinEditVector.remove(i);
            m_dlgBinEditVector.erase(m_dlgBinEditVector.cbegin() + i);
            break;
        }
    }
}

void MainWindow::slotCreateVarAddrDlg(uint32_t pos, QPoint *pnt)
{   // pos -номер позиции переменной в векторе параметров
    // pnt -точка расположения итема переменной в списке в глобальных координатах экрана
    int curTab = currentTabWork();
    if(pos > m_dlgTableWorkVector[curTab]->m_varSelVectorTmp.size() - 1)
        return;
    DlgVarAddr *dlgAddr = new DlgVarAddr(this);
    // точка начала расположения окна аттрибутов в локальных координатах MainWindow
    QPoint pntLoc = m_dlgTableWorkVector[curTab]->mapToParent(*pnt);
    int x = pntLoc.x() + 8;
    int y = pntLoc.y() + 25;
    dlgAddr->move(x, y);
    dlgAddr->showDlgAddr(m_dlgTableWorkVector[curTab]->m_varSelVectorTmp[pos].varID.varName,
                         m_dlgTableWorkVector[curTab]->m_varSelVectorTmp[pos].varProp.varAddr,
                         m_dlgTableWorkVector[curTab]->m_varSelVectorTmp[pos].varID.unitID,
                         m_dlgTableWorkVector[curTab]->m_varSelVectorTmp[pos].varID.unitName);
}

void MainWindow::slotUpdateArrData(uint32_t unit, arrNewData_t &data)
{	// Пересылка полученных данных массива с шины CAN
    // в соответствующие открытые окна отображения массивов
    // -----------------------------------------------------------
    Q_UNUSED(unit);
    if(m_arrDlgPropVector.empty())
        return; // если ни одного окна массивов не открыто
    // Mutex для синхронизации с потоком чтения по шине CAN
    QMutexLocker lockerArr(&m_mutexUpdateArrData);
    for(int i = 0; i < m_arrDlgPropVector.size(); ++i) {
        if(m_arrDlgPropVector[i].varID == data.vecCurData[0].varID) {
            m_myLocalServer->newArrayData(i, data.vecCurData);
            break; // переход к следующему массиву
        }
    }
}

void MainWindow::Trans2SysType(var_t *var)
{	// ф-ция преобразования данных из памяти микроконтроллера в значения соответствующих
    // типов и приведения в типу float для дальнейших матаматических вычислений
    // (также преобразуем число в строку для отображения в таблице (листе)
    // ...
    //>>> ПАМЯТКА >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //>>> преобразование значений числа типа float в памяти в само число >>>>>>>>>>>>>>>>
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //> 1. объявляем (float) переменную var
    //> 2. определяем ее адрес в памяти -unsigned *pvar
    //> 3. имеем представление числа в памяти (пришло по CAN в буфер): 4E 22 F7 42
    //>    -начиная со старшего байта -соответствует (float) 123.567
    //> 4. переставляем байты -получаем HEX число 0x42F7224E
    //> 5. пишем в память по адресу *pvar полученное число
    //> 6. получаем var = 123.567 (0x42f7224e)
    //===================================================================================

    switch(var->varType.typeID) {
    case TYPE_INT8: {
        const int8_t db = (int8_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number((uint8_t)(db), 16).toUpper();
        }
        break;
    case TYPE_UINT8: {
        const uint8_t db = (uint8_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number(db, 16).toUpper();
        }
        break;
    case TYPE_INT16: {
        const int16_t db = (int16_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number((uint16_t)db, 16).toUpper();
        }
        break;
    case TYPE_UINT16: {
        const uint16_t db = (uint16_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number(db, 16).toUpper();
        }
        break;
    case TYPE_INT32: {
        const int32_t db = (int32_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number((uint32_t)db, 16).toUpper();
        }
        break;
    case TYPE_UINT32: {
        const uint32_t db = (uint32_t)var->varData.memData;
        var->varData.varValueF = (float)db;
        var->varData.varValueSD = QString::number(db);
        var->varData.varValueSH = QString::number(db, 16).toUpper();
        }
        break;
    case TYPE_FLOAT: {
        float var_f = 0; // объявим переменную типа float
        uint32_t* pvar_f = (uint32_t*)&var_f; // адрес в памяти переменной var_f
        *pvar_f = var->varData.memData; // определяем значение переменной и размещаем его по адресу этой переменной
        var->varData.varValueF = var_f;
        var->varData.varValueSD = QString::number((float)var_f, 'f', 3); // строковое значение переменной
        }
        break;
    }
}

void MainWindow::slotUpdateVarData(uint32_t unit, QVector<curData_t> &vecNewData)
{   //!=> с шины CAN поступило обновление сразу для всех адресов узла unit
    QMutexLocker lockerVar(&m_mutexUpdateVarData);
    const int varNewCnt = (int)vecNewData.size(); // число обновленных переменных
    const int varGlobalCnt = (int)m_dlgVarSel->m_VarSelGlobalVector.size(); // число всех выбранных переменных
    for (int i = 0; i < varGlobalCnt; ++i) {
        for(int j = 0; j < varNewCnt; ++j) {
            if(m_dlgVarSel->m_VarSelGlobalVector[i].varID.varID == vecNewData[j].varID) {
                // время фиксации величины переменной
                m_dlgVarSel->m_VarSelGlobalVector[i].varData.varTime = vecNewData[j].data0; // время
                // значение переменной (в памяти микроконтроллера)
                m_dlgVarSel->m_VarSelGlobalVector[i].varData.memData = vecNewData[j].data1; // данные
                // преобразуем данные
                Trans2SysType(&m_dlgVarSel->m_VarSelGlobalVector[i]);
                // переберем все вкладки (работаем с рабочими векторами открытых вкладок)
                for(int k = 0; k < (int)m_dlgTableWorkVector.size(); ++k) {
                    if(m_dlgTableWorkVector[k]->m_fOpened) { // работаем с открытой вкладкой
                        // всего имеем переменных во вкладке
                        const int varWorkUse = (int)m_dlgTableWorkVector[k]->m_varSelVector.size();
                        if(!varWorkUse)
                            break;
                        static int varWorkCnt = 0; // счетчик найденных переменных
                        // перебор параметров во вкладке
                        for(int n = 0; n < varWorkUse; ++n) {
                            // ищем из числа глобально выбранных переменных, переменные во вкладке
                            if(checkVarId(m_dlgTableWorkVector[k]->m_varSelVector[n],
                                          m_dlgVarSel->m_VarSelGlobalVector[i])) {
                                // копируем преобразованные данные в вектор открытой вкладки
                                m_dlgTableWorkVector[k]->m_varSelVector[n] = m_dlgVarSel->m_VarSelGlobalVector[i];
                                if(++varWorkCnt >= varNewCnt) {
                                    // перебрали все параметры вкладки -> обновим таблицу
                                    varWorkCnt = 0;
                                    m_dlgTableWorkVector[k]->updateDataWorkTable();
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        // передадим обновленные данные в окно CDlgBinEditor, если оно открыто (выбрано юзером) ----------
        // перебор открытых окон бинэдитора
        for (int n = 0; n < (int)m_dlgBinEditVector.size(); ++n) {
            if(checkVarId(m_dlgBinEditVector[n]->m_var, m_dlgVarSel->m_VarSelGlobalVector[i])) {
                m_dlgBinEditVector[n]->UpdateBinEditData(m_dlgVarSel->m_VarSelGlobalVector[i]);
                break;
            }
        }

        // передадим данные на построение графиков ---------------------------
        if(!m_chartSelVector.isEmpty()) {
            chartData_t  chartData{};
            // укороченный вектор для передачи данных через сервер
            static QVector<chartData_t> tmpChartData{};
            tmpChartData.resize(m_chartSelVector.size());
            static int varChartCnt = 0; // счетчик найденных переменных
            // всего имеем переменных для графика
            const int varChartUse = (int)m_chartSelVector.size();
            for (int n = 0; n < varChartUse; ++n) {
                // если varId совпали и совпал номер узла, заносим в массив данные переменной
                if((checkVarId(m_chartSelVector[n], m_dlgVarSel->m_VarSelGlobalVector[i])) &&
                    (m_chartSelVector[n].varID.unitID == unit)) {
                    chartData.varName  = m_dlgVarSel->m_VarSelGlobalVector[i].varID.varName;
                    chartData.unitName = m_dlgVarSel->m_VarSelGlobalVector[i].varID.unitName;
                    chartData.unitID   = m_dlgVarSel->m_VarSelGlobalVector[i].varID.unitID;
                    chartData.typeID   = m_dlgVarSel->m_VarSelGlobalVector[i].varType.typeID;
                    chartData.varTime  = m_dlgVarSel->m_VarSelGlobalVector[i].varData.varTime;
                    chartData.varValF  = m_dlgVarSel->m_VarSelGlobalVector[i].varData.varValueF;
                    tmpChartData[n] = chartData;
                    if (++varChartCnt == varChartUse) {
                        // перебрали все выбранные переменные -> обновим графики
                        varChartCnt = 0;
                        if(m_myLocalServer->m_chartSocket)
                            m_myLocalServer->newChartData(tmpChartData);
                        tmpChartData.clear();
                    }
                    break;
                }
            }
        }

        // сохраним данные в *.mon файле ---------------------------
        if (m_recFile_opened) {
            static int varMonCnt = 0; // счетчик найденных переменных
            // всего имеем переменных для записи
            const int varMonUse = (int)m_dlgVarSel->m_recSelVector.size();
            for (int n = 0; n < varMonUse; ++n) {
                // если varId совпали, заносим в вектор данные переменной
                if(checkVarId(m_dlgVarSel->m_recSelVector[n], m_dlgVarSel->m_VarSelGlobalVector[i])) {
                    m_dlgVarSel->m_recSelVector[n] = m_dlgVarSel->m_VarSelGlobalVector[i];
                    if (++varMonCnt == varMonUse) {
                        // перебрали все параметры для записи -> запишем в файл
                        varMonCnt = 0;
                        saveRecData();
                    }
                    break; // сравнение идет по одному параметру каждую итерацию по varGlobalCnt
                }
            }
        }
    }
}

// Для записи файла *.mon ====================================================

#define SEC_MS	1000
#define MIN_MS	(SEC_MS * 60)
#define HOUR_MS	(MIN_MS * 60)

QTime operator - (const QTime &t1, const QTime &t2)
{   // мой оператор для вычитания отсчетов времени для класса QTime
    int diff = t2.msecsTo(t1);
    return QTime(diff / HOUR_MS,
                (diff % HOUR_MS) / MIN_MS,
                (diff % MIN_MS) / SEC_MS,
                 diff % SEC_MS);
}

void MainWindow::slotRecTimerAlarm()
{	// отсчет времени записи файла *.mon и мигание кнопки записи
    static int cnt = 0;
    QPixmap iconOn(RECORD_ICON);
    QPixmap iconOff(RECPAUSE_ICON);
    QTime timeDelta;
    if (cnt++ == 4) { // таймер настроен на 250мс -обновляем время с периодом 1сек
        cnt = 0;
        ui->BTN_START_RECORD->setIcon(QIcon(iconOff));
        m_recTimeEnd = QTime::currentTime();
        // применим собственный перегруженный оператор "QTime operator-"
        timeDelta = m_recTimeEnd - m_recTimeStart;
        QString str = timeDelta.toString("hh:mm:ss");
        ui->EDIT_RECORD_TIME->setText(str);
    }
    if (cnt == 2) // мигание иконки кнопки записи
        ui->BTN_START_RECORD->setIcon(QIcon(iconOn));
}

void MainWindow::saveGr()
{
    QDateTime cur_time = QDateTime::currentDateTime();
    QString str = cur_time.toString("dd.MM.yyyy_hh.mm.ss");
    QString fileName = QString("./QVarMon_%1.mon").arg(str);
    m_recFile.setFileName(fileName);
    if(m_recFile.open(QIODevice::WriteOnly)) {
        m_recStream.setDevice(&m_recFile);
        m_recFile_opened = true;
        m_recFile_started = true; // для однократной записи заголовка файла
        m_recTimeStart = QTime::currentTime();
        m_startTime = 0;
    }
    else {
        qDebug()<< "File 'Gr' not opened";
    }
}

void MainWindow::saveRecData()
{   // записывает данные в файл из ф-ции MainWindow::slotUpdateVarData()
    // при каждом обновлении переменных (с периодом ~100 мс)
    //---------------------------------
    // временнЫе метки для записи в *.mon файл
    QString sRec{}, sTmp{};
    int cntMon = (int)m_dlgVarSel->m_recSelVector.size();
    if(m_recFile_started) { // здесь пропишем заголовок и обозначим начало времени отсчета
        m_recFile_started = false;
        // пропишем в файл число переменных и поле time, как 1й параметр файла формата *.mon
        sRec = QString("%1\ntime[s]\n").arg(cntMon + 1);
        // пропишем в файл имена переменных с префиксом имени узла
        QString str;
        for (int n = 0; n < cntMon; n++) {
            str = QString("%1.%2").arg(m_dlgVarSel->m_recSelVector[n].varID.unitName,
                                       m_dlgVarSel->m_recSelVector[n].varID.varName);
            if(n < cntMon - 1)
                sRec += str + "\n";
            else
                sRec += str;
        }
        m_recStream << sRec << Qt::endl;
        m_startTime = clock();
        sRec = "";
    }
    double curTime;
    // число миллисекунд между началом записи и текущим временем
    curTime = (clock() - m_startTime) / (double)CLOCKS_PER_SEC; // 1000
    sRec = QString("%1").arg(curTime, 10, 'f', 3); // точность 0.001 сек
    for (int n = 0; n < cntMon; n++) {
        sTmp = QString("%1").arg(
            m_dlgVarSel->m_recSelVector[n].varData.varValueSD, 12);
        sRec += sTmp;
    }
    m_recStream << sRec << Qt::endl;
}

void MainWindow::on_BTN_START_RECORD_clicked()
{
   if(ui->BTN_START_RECORD->isChecked()) { // кнопку нажали -запись пошла
       // отключим режим выбора параметров для *.mon файла
       QPixmap icon(RECPAUSE_ICON);
       ui->BTN_START_RECORD->setIcon(QIcon(icon));
       // запустим таймер обновления показаний в индикаторе и мигания кнопки записи
       m_recTimer->start(250);
       saveGr();
   }
   else { // остановка записи
       m_recTimer->stop(); // остановим таймер
       QPixmap icon(RECORD_ICON);
       ui->BTN_START_RECORD->setIcon(QIcon(icon));
       m_recFile_opened = false;
       m_recFile.flush();
       m_recFile.close();
       ui->EDIT_RECORD_TIME->setText("00:00:00");
   }
}

void MainWindow::on_BTN_SEL_REC_PARAM_clicked()
{
    if(m_dlgVarSel->m_VarSelGlobalVector.empty()) {
        showSelfClosedMessageBox(2000, "Нет доступных параметров для записи");
        return;
    }
    // переключим в диалоге выбора параметров комбо на режим "Запись"
    // и покажем диалог выбора параметров
    m_dlgVarSel->setComboParamIndex(REC_PARAM);
    m_dlgVarSel->showDlgSel();
}

void MainWindow::slotBtnStartRecEn(bool en)
{
    ui->BTN_START_RECORD->setEnabled(en);
}

void MainWindow::slotBtnSelRecParamEn(bool en)
{
    ui->BTN_SEL_REC_PARAM->setEnabled(en);
}

void MainWindow::on_tabWidget4Work_tabBarDoubleClicked(int index)
{
    QTabBar *tabBar = ui->tabWidget4Work->tabBar();
    QRect tabRect  = tabBar->tabRect(index);

    m_editTabWnd.editWnd = new QLineEdit(ui->tabWidget4Work);
    m_editTabWnd.editWnd->setGeometry(tabRect.left()+4, 4, tabRect.width() -35, 17);
    m_editTabWnd.editWnd->show();
    m_editTabWnd.index = index;
    connect(m_editTabWnd.editWnd, &QLineEdit::editingFinished, this, &MainWindow::setTabNewName);
}

void MainWindow::setTabNewName()
{
    if(!m_editTabWnd.editWnd)
        return;
    QString str = m_editTabWnd.editWnd->text();
    if(str.isEmpty())
        str = m_dlgTableWorkVector[m_editTabWnd.index]->m_sTableName;
    setTabWorkName(m_editTabWnd.index, str);
    m_editTabWnd.editWnd->close();
    delete m_editTabWnd.editWnd;
    m_editTabWnd.editWnd = 0;
}

void MainWindow::on_action_FILE_OPEN_TAB_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                        "Открыть вкладку", "", "Файлы вкладок (*.stab)");

    int numTab = ui->tabWidget4Work->count(); // общее число вкладок
    QString tabName{};
    int nVar = 0;
    var_t var{};
    QFile tabFile{};
    tabFile.setFileName(filePath);
    if(tabFile.open(QIODevice::ReadOnly)) {
        createNewTabWork(numTab, true); // создаем следующую вкладку и открываем
        QDataStream stream(&tabFile);
        stream.setVersion(QDataStream::Qt_6_7);
        stream >> tabName;
        setTabWorkName(numTab, tabName);
        stream >> nVar;
        for(int i = 0; i < nVar; ++i) {
            stream >> var.varID.varName;    //
            stream >> var.varID.unitName;   //
            stream >> var.varID.unitID;     //
            stream >> var.varProp.varAddr;  //
            stream >> var.varProp.varID;    //
            stream >> var.varProp.varSize;  //
            stream >> var.varType.altType;  //
            stream >> var.varType.typeID;   //
            stream >> var.varType.typeSize; //
            m_dlgTableWorkVector[numTab]->m_varSelVector.emplace_back(var);
            m_dlgTableWorkVector[numTab]->m_varSelVectorTmp.emplace_back(var);
            m_dlgVarSel->m_VarSelGlobalVector.emplace_back(var);
        }
        tabFile.close();
        m_dlgVarSel->updateComboTabsName(numTab, tabName);
        m_dlgTableWorkVector[numTab]->updateDataWorkTable();
        // отправим девайсу адреса параметров для сканирования
        slotSendSelParamToDevice();
        // толкнем переключатель данных каналов
        m_dlgIxxat->getDevId();
        delayMs(5);
        m_dlgIxxat->slotChannelChange(false);
    }
}

void MainWindow::on_action_FILE_CREATE_NEW_TAB_triggered()
{
    int numTab = ui->tabWidget4Work->count(); // общее число вкладок
    createNewTabWork(numTab, true); // создаем следующую вкладку и открываем
    ui->tabWidget4Work->setCurrentIndex(numTab);
}

void MainWindow::on_action_FILE_SAVE_TAB_triggered()
{
    QFile tabFile{};
    for(int i = 0; i < (int)m_dlgTableWorkVector.size(); ++i) {
        if(m_dlgTableWorkVector[i]->m_fOpened) { // работаем с открытой вкладкой
            int nVar = m_dlgTableWorkVector[i]->m_varSelVector.size();
            tabFile.setFileName(m_dlgTableWorkVector[i]->m_sTableName + ".stab");
            if(tabFile.open(QIODevice::WriteOnly)) {
                QDataStream stream(&tabFile);
                stream.setVersion(QDataStream::Qt_6_7);
                stream << m_dlgTableWorkVector[i]->m_sTableName;
                stream << nVar;
                for(int j = 0; j < nVar; ++j) {
                    const var_t var = m_dlgTableWorkVector[i]->m_varSelVector[j];
                    stream << var.varID.varName;    //
                    stream << var.varID.unitName;   //
                    stream << var.varID.unitID;     //
                    stream << var.varProp.varAddr;  //
                    stream << var.varProp.varID;    //
                    stream << var.varProp.varSize;  //
                    stream << var.varType.altType;  //
                    stream << var.varType.typeID;   //
                    stream << var.varType.typeSize; //
                    if(stream.status() != QDataStream::Ok) {
                        qDebug() << "Ошибка записи VarFile";
                    }
                }
            }
            tabFile.flush();
            tabFile.close();
        }
    }
}

void MainWindow::on_action_SELECT_CONF_triggered()
{
    m_dlgSelectCfg->show();
}

void MainWindow::on_action_SELECT_PARAM_triggered()
{
    m_dlgVarSel->showDlgSel();
}

void MainWindow::on_action_ADDADDRESS_triggered()
{
    m_dlgAddAddress->showDlg();
}

void MainWindow::on_action_APP_EXIT_triggered()
{
    this->close();
    this->destroy();
}

void MainWindow::on_action_SHOW_CHART_triggered()
{
    startDlgChart();
}

void MainWindow::on_action_ABOUT_triggered()
{
    m_dlgAbout->show();
}

void MainWindow::on_action_HOWTOUSE_triggered()
{
    m_dlgHowToUse->show();
}

void MainWindow::closeEvent (QCloseEvent * e)
{
#ifdef Q_OS_WINDOWS
    // снять запрет Windows переходить в спящий режим
    SetThreadExecutionState(ES_CONTINUOUS);
#endif
    // закроем сервер
    qDebug()<<"Closing Server";
    m_myLocalServer->m_localServer->close();

    // очистка запросов для всех узлов
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_unitProp.property[i].isPresent) { // если канал задействован
            m_dlgIxxat->sendCommand(CMD_NEW_VAR, i, 0, 0);
            m_dlgIxxat->sendCommand(CMD_NEW_ARR, i, 0, 0);
        }
    }
    // закроем все открытые окна массивов и графика
    for(int i = 0; i < m_arrDlgPropVector.size(); ++i) {
        m_myLocalServer->sendToClient(m_arrDlgPropVector[i].socket, "Close");
    }

    // отключить IXXAT и остановить поток ReadThread
    // (в потоке есть сигналы со слотами в MainWindow)
    m_dlgIxxat->ixxat_deinit();

    this->close();
    e->accept();
    delete ui;
    ui = nullptr; //! чтобы в ~MainWindow() не пытаться еще раз удалять Ui
}

// ======================================================================
// *** Read VarFile *****************************************************
// ======================================================================

void MainWindow::readVarFile(uint32_t unit, QString fileName, bool list)
{   // загрузка и чтение файла из файловой системы
    // list == false -для одного канала, true -для несколькх
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        // читаем дескриптор файла
        m_fileDescTmp = {};
        stream >> m_fileDescTmp;
        m_varAttrVectorTmp.clear();
        // читаем файл в структуры varAttr_t
        file.seek(0x30); // начало описания переменных
        varAttr_t attr{};
        for (uint32_t i = 0; i < m_fileDescTmp.cnt; i++) {
            stream >> attr;
            attr.desc = m_fileDescTmp;
            // установим имя узла(канала) в соответствии с конфигурацией
            attr.desc.uID = m_unitProp.property[unit].unitID;
            attr.desc.uName = m_unitProp.property[unit].unitName;
            m_varAttrVectorTmp.emplace_back(attr);
        }
        if(stream.status() != QDataStream::Ok) {
            qDebug() << "readVarFile: Ошибка чтения VarFile";
        }
        file.close();
    }
    else {
        QMessageBox::critical(nullptr, "Ошибка!",
                              "Проверьте корректность пути к файлу", QMessageBox::Ok);
    }
    if(!list) { // только для одного узла
        m_varAttrVector[unit] = m_varAttrVectorTmp;
        fillVarSelGlobalTable(unit); // заполним глобальную таблицу выбора параметров
    }
}

void MainWindow::readVarFileFromCfg(unitProp_t &prop)
{
    for(int i = 0; i < (int)prop.numChannels; ++i) {
        int unit = prop.listID[i].toInt();
        // загружаем файлы для всех узлов (указывает аргумент true)
        readVarFile(unit, prop.listVarFilePath[i], true);
        m_varAttrVector[unit] = m_varAttrVectorTmp;
        prop.property[unit].isLoadVarFileDisk = true;
        prop.property[unit].isLoadVarFileFlash = false;
        // заполним глобальную таблицу выбора параметров
        fillVarSelGlobalTable(unit);
    }
}

void MainWindow::slotReadVarFileFlash(uint32_t unit, QByteArray fileData)
{   // чтение файла из флеш памяти девайса (signal в файле "dlgixxat.cpp" ф-ции ReadThread()
    ///>> 1. Если сохранение файла переменных из флеш девайса в файл "*.svar" на диске
    // (здесь КС не проверяем -пишем в файл, как есть)
    if(m_unitProp.property[unit].isSaveVarFile) {
        m_unitProp.property[unit].isSaveVarFile = false;
        QString filePath = QFileDialog::getSaveFileName(NULL,
                                                        "Сохранить файл переменных", ".", "*.svar");
        QFile newVarFile(filePath);
        if(newVarFile.open(QIODevice::WriteOnly)) {
            // запись в бинарный файл производим методом write(), т.к.
            // QDataStream добавляет в начало файла дополнительные
            // байты размера файла
            fileData.remove(0,8); // удалим первые 8 байт с КС файла
            int len = newVarFile.write(fileData);
            if(len == -1)
                qDebug() << "write len =" << len << "; Error =" << newVarFile.errorString();
            newVarFile.close();
        }
        else
            qDebug() << "Ошибка открытия файла для записи";
    }
    else { ///>> 2. Если загрузка файла из флеш девайса для мониторинга
        QDataStream stream(fileData);
        // читаем КС файла
        uint32_t cs32_orig = 0;
        stream >> cs32_orig; // КС расположена в первых 8-и (uint32_t) байтах
        // преобразуем big-endian в little-endian
        cs32_orig = BE2LE(cs32_orig);
        // читаем дескриптор файла
        stream.device()->seek(0x08); // переместимся в начало дескриптора файла
        stream >> m_fileDescTmp;
        // читаем файл в структуры varAttr_t
        stream.device()->seek(0x30 + 0x08); // начало описания переменных (0x30 байт) + 8 байт КС
        m_varAttrVectorTmp.clear();
        varAttr_t attr{};
        for (uint32_t i = 0; i < m_fileDescTmp.cnt; i++) {
            stream >> attr;
            attr.desc = m_fileDescTmp;
            // установим имя узла в соответствии с конфигурацией
            attr.desc.uID = m_unitProp.property[unit].unitID;
            attr.desc.uName = m_unitProp.property[unit].unitName;
            m_varAttrVectorTmp.emplace_back(attr);
        }

        if(stream.status() > QDataStream::Ok) {
            qDebug() << QString("readVarFileFlash: ReadError! VarFile unit : %1; status=%2")
                            .arg(unit).arg(stream.status()).toLatin1();
            return;
        }

        qDebug() << QString("readVarFileFlash: readed VarFile unit : %1; Params = %2")
                        .arg(unit).arg(m_varAttrVectorTmp.size()).toLatin1();

        fileData.remove(0,8); // удалим первые 8 байт с КС файла
        uint32_t cs32 = getCRC32(fileData, m_fileDescTmp.fSz);
        if(cs32_orig != cs32) {
            qDebug() << QString("readVarFileFlash: CRC32 Error! VarFile unit : %1").arg(unit);
            return;
        }

        if(m_unitProp.property[unit].isPresent) { // узел подключен
            m_varAttrVector[unit] = m_varAttrVectorTmp;
            qDebug() << QString("VarFile downloaded from the unit : %1; Параметров %2").arg(unit).arg(m_varAttrVector[unit].size());
            // заполним глобальную таблицу выбора параметров
            fillVarSelGlobalTable(unit);
        }
    }
}

void MainWindow::fillVarSelGlobalTable(uint32_t unit)
{   // заполняет глобальную таблицу выбора параметров
    QVector<var_t>	m_VarAllVectorTmp{};    /// список всех переменных, загруженных из файла
    QVector<var_t>	m_ArrAllVectorTmp{};    /// список переменных, выбранных для мониторинга для всех вкладок

    if(m_unitProp.property[unit].fReloadFile) {
        m_unitProp.property[unit].fReloadFile = false;
        for(int i = m_dlgVarSel->m_VarAllGlobalVector.size() - 1; i >= 0; --i) {
            if(m_dlgVarSel->m_VarAllGlobalVector[i].varID.unitID == unit)
                m_dlgVarSel->m_VarAllGlobalVector.remove(i);
        }
        for(int i = m_dlgVarSel->m_ArrAllGlobalVector.size() - 1; i >= 0; --i) {
            if(m_dlgVarSel->m_ArrAllGlobalVector[i].varID.unitID == unit)
                m_dlgVarSel->m_ArrAllGlobalVector.remove(i);
        }
    }

    int cntVar = m_varAttrVector[unit].size();
    var_t tmp{};
    // int idx = 1;
    for(int i = 0; i < cntVar; ++i) {
        tmp.varID.varID         = i + unit * 1000;
        tmp.varFlags.f_Monitor  = false;
        tmp.varFlags.f_Chart    = false;
        tmp.varFlags.f_Record   = false;
        tmp.varFlags.f_Array    = false;
        tmp.varProp.sysID       = DECSYS;
        // из дескриптора
        tmp.varID.unitID    = m_varAttrVector[unit][i].desc.uID;
        tmp.varID.unitName  = m_varAttrVector[unit][i].desc.uName;
        // аттрибуты
        tmp.varID.varName   = m_varAttrVector[unit][i].varName;
        tmp.varProp.varSize = m_varAttrVector[unit][i].varSize;
        tmp.varProp.varAddr = m_varAttrVector[unit][i].varAddr;
        tmp.varType.typeSize= m_varAttrVector[unit][i].typeSize;
        tmp.varProp.arrCol  = m_varAttrVector[unit][i].arrCol;
        tmp.varProp.arrRow  = m_varAttrVector[unit][i].arrRow;
        tmp.varType.typeID  = m_varAttrVector[unit][i].varTypeID;
        tmp.varData.memData = 0;

        switch(tmp.varType.typeID) {
        case TYPE_INT8:   tmp.varType.altType  = "int8";
            break;
        case TYPE_UINT8:  tmp.varType.altType  = "uint8";
            break;
        case TYPE_INT16:  tmp.varType.altType  = "int16";
            break;
        case TYPE_UINT16: tmp.varType.altType  = "uint16";
            break;
        case TYPE_INT32:  tmp.varType.altType  = "int32";
            break;
        case TYPE_UINT32: tmp.varType.altType  = "uint32";
            break;
        case TYPE_FLOAT:  tmp.varType.altType  = "float";
            tmp.varProp.sysID = FLOATSYS;
            break;
        }

        if(tmp.varProp.arrCol > 0) // массивы
            m_ArrAllVectorTmp.emplace_back(tmp);
        else // переменные
            m_VarAllVectorTmp.emplace_back(tmp);
    }
    // сортировка строк в глобальных векторах всех переменных и массивов
    foreach (QString str, m_unitProp.listID) {
        if(str == QString::number(unit)) {
            m_dlgVarSel->m_VarAllGlobalVector.append(m_VarAllVectorTmp);
            m_dlgVarSel->m_ArrAllGlobalVector.append(m_ArrAllVectorTmp);
            // переменные
            for (int i = 0; i < (int)m_dlgVarSel->m_VarAllGlobalVector.size(); i++) {
                // сортировка строк имен переменных по алфавиту
                std::sort(
                    std::begin(m_dlgVarSel->m_VarAllGlobalVector),
                    std::end(m_dlgVarSel->m_VarAllGlobalVector),
                    compareUpName);
            }
            // массивы
            for (int i = 0; i < (int)m_dlgVarSel->m_ArrAllGlobalVector.size(); i++) {
                // сортировка строк имен массивов по алфавиту
                std::sort(
                    std::begin(m_dlgVarSel->m_ArrAllGlobalVector),
                    std::end(m_dlgVarSel->m_ArrAllGlobalVector),
                    compareUpName);
            }
            m_dlgVarSel->showDlgSel();
        }
    }
}

QDataStream &operator << (QDataStream &stream, const MainWindow::varFileDesc_t &desc)
{   // запись в поток дескриптора файла
    stream << desc.ver;     // версия формата файла
    stream << desc.fSz;     // размер файла
    stream << desc.cnt;     // число записей переменных
    stream << desc.uID;     // ID узла
    stream << desc.uName;   // имя узла
    return stream;
}

QDataStream &operator >> (QDataStream &stream, MainWindow::varFileDesc_t &desc)
{   // чтение из потока дескриптора файла
    stream >> desc.ver;
    stream >> desc.fSz;
    stream >> desc.cnt;
    stream >> desc.uID;
    stream >> desc.uName;
    return stream;
}

QDataStream &operator << (QDataStream &stream, const MainWindow::varAttr_t &attr)
{   // запись в поток аттрибутов переменной
    stream << attr.varName;
    uint32_t par = 0;
    par |=  (attr.typeSize  << 0) | // тип переменной для идентификации
            (attr.varTypeID << 4) | // размер типа переменной
            (attr.arrCol    << 8) | // число колонок массива переменных
            (attr.arrRow    << 20); // число строк массива переменных
    stream << par;
    stream << attr.varSize;      // размер переменной в байтах (простых переменных, массивов, структур)
    stream << attr.varAddr;      // адрес переменной в памяти микроконтроллера
    return stream;
}

QDataStream &operator >> (QDataStream &stream, MainWindow::varAttr_t &attr)
{   // чтение из потока аттрибутов переменной
    stream >> attr.varName;
    uint32_t par = 0;
    stream >> par;
    attr.typeSize  = (par >> 0)  & 0x000f;
    attr.varTypeID = (par >> 4)  & 0x000f;
    attr.arrCol    = (par >> 8)  & 0x0fff;
    attr.arrRow    = (par >> 20) & 0x003f;
    attr.reserved  = 0;
    stream >> attr.varSize;      // размер переменной в байтах (простых переменных, массивов, структур)
    stream >> attr.varAddr;      // адрес переменной в памяти микроконтроллера
    return stream;
}


