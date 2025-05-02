#include <QToolButton>
#include <QMenu>
#include <QDebug>
#include <QListView>
#include "common.h"
#include "mainwindow.h"
#include "dlgtablework.h"
#include "ui_dlgtablework.h"
#include "mystyles.h"
// #include "LoggingCategories.h"


MyTabBar::MyTabBar(QWidget *parent)
{
    Q_UNUSED(parent);
    // m_rect = this->geometry();
}

void MyTabBar::paintEvent(QPaintEvent *event)
{
    QTabBar::paintEvent( event );
}

MainWindow  *m_mainWnd_4TableWork{};

DlgTableWork::DlgTableWork(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTableWork)
{
    ui->setupUi(this);
    m_mainWnd_4TableWork = static_cast<MainWindow *>(parent);

    // m_tabBar = new MyTabBar(this);
    m_table = ui->TABLE_WORK;
    m_model = new TableWorkModel(0, MAX_COLUMN, this);
    m_table->setModel(m_model);
    m_comboDelegat = new myComboBoxDelegate(this);

    this->installEventFilter(this);
    m_table->viewport()->installEventFilter(this);


    m_tableAttr.cntParams = 0; // 0 строк таблицы
    m_tableAttr.nBlocks = 0;
    m_tableAttr.old_nBlocks = 0;
    m_tableAttr.nRows = 0;
    m_tableAttr.old_nRows = 0;
    m_tableAttr.newAttr = false;
    // зарезервируем число секций заголовка в векторе размеров секций
    m_tableAttr.sz.resize(MAX_COLUMN);

    m_indexNotUsed = m_model->index(0, 0, QModelIndex());
    m_model->setIndexNotUsed(m_indexNotUsed);
    initWorkTableProperty();

    m_timerResize = new QTimer(this);
    connect(m_timerResize, &QTimer::timeout, this, &DlgTableWork::slotTimerAlarmResize);

    m_timerUpdate = new QTimer(this);
    connect(m_timerUpdate, &QTimer::timeout, this, &DlgTableWork::slotTimerAlarmUpdate);
    m_timerUpdate->start(100); // запустим таймер

    initConnectsTable();

}

DlgTableWork::~DlgTableWork()
{
    delete ui;
}

bool DlgTableWork::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type type = event->type();
    // Q_UNUSED(type);
    // static int cnt = 0;
    // qDebug() << cnt++ << " eventFilter() event_type ="<<type;

    if (obj == this) {
        if(type == QEvent::Show) {
            if(m_first) { // единожды(при показе окна) инициализируем коннекты
                // initConnectsTable();
                m_first = false;
            }
        }
        if(type == QEvent::Resize) {
            // При масштабировании окна во время обновления таблицы,
            // резко меняются индексы хеша данных модели, хеш начинает
            // обрастать новыми индексами и в итоге прога крашится.
            // Поэтому введем флаг, запрещающий обновление на время
            // масштабирования - при этом индексы сохраняются и по
            // окончании масштабирования корректируются.
            // Окончание масштабирования будем проверять по таймеру
            // по размерам окна до и после.
            if(!m_first) {
                m_resizing = true;
                QResizeEvent *ev = reinterpret_cast<QResizeEvent *>(event);
                m_wndSize = ev->size(); // последний размер окна в начале маштабирования
                                        // -сравнивается с размером по окончании периода
                                        // таймера в ф-ции slotTimerAlarmResize
                m_timerResize->start(100); // запустим таймер -если ресайз не закончился,
                                     // таймер перезапускается здесь же.
            }
        }
        // if(type == QEvent::WindowDeactivate) {
        //     // m_resizing = false;
        //     // m_table->viewport()->repaint();
        //     // qDebug()<<"QEvent::WindowDeactivate";
        // }
        // if(type == QEvent::MouseButtonPress) {
        //     type = QEvent::MouseButtonPress;
        // }
    }

    if (obj == m_table->viewport()) {
        if(type == QEvent::MouseButtonPress) {
            // проверяем координату клика в окне на предмет
            // клика вне таблицы с целью сброса выбранных ячеек
            tableClickOut();
        }
    }

    return QWidget::eventFilter(obj, event);
}

void DlgTableWork::slotTimerAlarmResize()
{
    QSize size = this->size();
    if(size == m_wndSize){
        m_timerResize->stop();
        m_resizing = false;
    }
}

void DlgTableWork::slotTimerAlarmUpdate()
{
    // qDebug()<<"TimerAlarmUpdate";
    fillDataModel();
    resizeHorHeader();
    m_table->repaint();
}

// void DlgTableWork::resizeEvent(QResizeEvent *event)
// {
//     m_resizing = true;
// }

bool DlgTableWork::tableClickOut()
{   // снятие выбора строк таблицы при клике вне строк
    QPoint MousePos = QCursor::pos();
    QPoint pntMouse = m_table->viewport()->mapFromGlobal(MousePos);
    // размер горизонального заголовка по горизонтали (ширина)
    const int szHorHeaderW = m_table->horizontalHeader()->length();
    // размер горизонального заголовка по вертикали (высота)
    const int szHorHeaderH = m_table->horizontalHeader()->height();
    // размер вертикального заголовка по вертикали (высота)
    const int szVerHeaderH = m_table->verticalHeader()->height();

    if((pntMouse.x() > szHorHeaderW) ||
        (pntMouse.y() > (szVerHeaderH - szHorHeaderH - 5))) {
        m_table->clearSelection(); // клик на внешней стороне таблицы
        return true;
    }
    return false;
}

void DlgTableWork::initConnectsTable()
{
    // создание и отображение окна бинэдитора
    connect(this, &DlgTableWork::signalCreateBinEditDlg,
            m_mainWnd_4TableWork, &MainWindow::slotCreateBinEditDlg);
    // создание и отображение окна аттрибутов
    connect(this, &DlgTableWork::signalCreateVarAddrDlg,
            m_mainWnd_4TableWork, &MainWindow::slotCreateVarAddrDlg);
    // установим соединение для "клик на заголовке колонки -> система счисления"
    connect(m_table->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &DlgTableWork::slotHeaderColumnClicked,
            Qt::DirectConnection);
    // нажатие ЛКМ на ячейке таблицы
    // (для выбора системы счисления и показа бинэдитора, показа аттрибутов переменной)
    connect(m_table, &QTableView::pressed,
            this, &DlgTableWork::slotItemClicked,
            Qt::DirectConnection);
    // сигнал от комбобокса модели об установке системы счисления
    connect(m_model, &TableWorkModel::signalSetSys,
            this, &DlgTableWork::slotCellSysComboActivated,
            Qt::DirectConnection);

}

void DlgTableWork::initWorkTableProperty()
{
    // цвет заголовков QTableWidget, цвет выделенной строки и цвет фонта
    m_table->setStyleSheet(QString("%1 %2").arg(myHeaderStyle, mySelectedStyle));
    // Разрешаем выделение только одного итема
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    // разрешаем Update таблицы
    m_table->setUpdatesEnabled(true);
    // разрешить реакцию на мышь
    m_table->setMouseTracking(true);
    // для реакции ячеек таблицы на один клик
    m_table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // установим фиксированную высоту заголовка
    m_table->horizontalHeader()->setFixedHeight(20);
    // Ограничим минимальную ширину колонок
    m_table->horizontalHeader()->setMinimumSectionSize(15); // для SYS_EL
    m_table->horizontalHeader()->setDefaultSectionSize(65);
    // установим высоту строк
    m_table->verticalHeader()->setMinimumSectionSize(18);
    m_table->verticalHeader()->setDefaultSectionSize(18);
    // скроем вертикальный заголовок
    m_table->verticalHeader()->hide();
    // Включаем сетку
    m_table->setShowGrid(true);
}

void DlgTableWork::initWorkTable()
{
    // макс.число строк таблицы, которое может уложиться по вертикали
    m_tableAttr.maxRowsByVert = getTableVSize();
    // вычислим число параллельных блоков таблицы (подтаблиц)
    m_tableAttr.nBlocks = (m_tableAttr.cntParams +
                          (m_tableAttr.maxRowsByVert - 1)) /
                          m_tableAttr.maxRowsByVert;
    // вычислим число строк в таблице
    m_tableAttr.nRows = (m_tableAttr.maxRowsByVert < m_tableAttr.cntParams) ?
                         m_tableAttr.maxRowsByVert : m_tableAttr.cntParams;

    // при изменении числа параметров или числа блоков строк при масштабировании
    // устанавливаем новую модель и делегата ComboBox
    if(((m_tableAttr.old_nBlocks != m_tableAttr.nBlocks) ||
        (m_tableAttr.old_nRows != m_tableAttr.nRows))) {

        m_tableAttr.newAttr = true;
        m_tableAttr.old_nBlocks = m_tableAttr.nBlocks;
        m_tableAttr.old_nRows = m_tableAttr.nRows;
        // зарезервируем число секций заголовка в векторе размеров секций
        m_tableAttr.sz.resize(m_tableAttr.nBlocks * MAX_COLUMN);

        //===> установим модель c новыми размерами
        m_model->setRowCol(m_tableAttr.nRows, MAX_COLUMN * m_tableAttr.nBlocks);
        m_table->setModel(m_model);
        m_model->m_hashData.clear();
        // не используемые строки и колонки в таблице
        int rowNotUsed = m_tableAttr.cntParams % m_tableAttr.maxRowsByVert - 1;
        int сolNotUsed = MAX_COLUMN * (m_tableAttr.nBlocks - 1);
        m_indexNotUsed = m_model->index(rowNotUsed, сolNotUsed, QModelIndex());
        m_model->setIndexNotUsed(m_indexNotUsed);

        // установим делегата QComboBox на колонки SYS_EL
        // qDebug() << "Before m_comboDelegat";
        for(int i = 0; i < m_tableAttr.nBlocks * MAX_COLUMN; ++i) {
            if((i - SYS_EL) % MAX_COLUMN == 0)
                m_table->setItemDelegateForColumn(i, m_comboDelegat);
        }
        // qDebug() << "After m_comboDelegat";
    }
}

void DlgTableWork::updateDataWorkTable()
{
    if(m_resizing)
        return;
    m_tableAttr.cntParams = (int)m_varSelVector.size();
    if(!m_tableAttr.cntParams)
        return;

    // при необходимости установим модель с новыми параметрами
    initWorkTable();

    QMutexLocker lockerVarData(&m_mutexVarData4Model);
    m_list.clear();
    for(int i = 0; i < m_tableAttr.cntParams; ++i) {
        m_list.emplace_back(QString::number(i + 1));
        m_list.emplace_back(m_varSelVector[i].varID.unitName);
        m_list.emplace_back(m_varSelVector[i].varID.varName);
        switch(m_varSelVector[i].varProp.sysID) {
        case DECSYS:
            m_list.emplace_back(m_varSelVector[i].varData.varValueSD);
            m_list.emplace_back(SMB_DEC);
            break;
        case HEXSYS:
            m_list.emplace_back(m_varSelVector[i].varData.varValueSH);
            m_list.emplace_back(SMB_HEX);
            break;
        case FLOATSYS:
            if(m_varSelVector[i].varData.varValueF < 1.0e-9)
                m_list.emplace_back("");
            else
                m_list.emplace_back(QString::number(m_varSelVector[i].varData.varValueF, 'f', 3));
            m_list.emplace_back(SMB_FLT);
            break;
        }
        m_list.emplace_back(m_varSelVector[i].varType.altType);
    }
}

QModelIndex DlgTableWork::getCurIndex(int i)
{   // вычисляет индекс для таблицы текущей конфигурации (с учетом строк, колонок, блоков)
    if(i == 0) { // сброс в начальные значения
        tabStr.maxRow = m_tableAttr.nRows; // реальное число строк в таблице
        tabStr.curBlock = 0;
        tabStr.curRow = -1;
        tabStr.curCol = 0;
    }
    // текущая колонка текущей строки
    tabStr.curCol = i % MAX_COLUMN + tabStr.curBlock * MAX_COLUMN;
    if((tabStr.curCol % MAX_COLUMN) == 0) // каждое значение номера колонки, кратное MAX_COLUMN -> новая строка
        ++tabStr.curRow;
    if(tabStr.curRow == tabStr.maxRow) { // при достижении макс.строк, переходим к след.блоку
        ++tabStr.curBlock;
        tabStr.curRow = 0; // начинаем новый блок со строки 0
        // номер колонки увеличивается на число колонок в каждом блоке
        tabStr.curCol = i % MAX_COLUMN + tabStr.curBlock * MAX_COLUMN;
    }
    return m_model->index(tabStr.curRow, tabStr.curCol, QModelIndex());
}

void DlgTableWork::fillDataModel()
{   // заполним модель данными
    if(m_list.isEmpty())
        return;
    QMutexLocker lockerVarData(&m_mutexVarData4Model);
    QModelIndex idx;
    int nPos = 0;
    m_model->m_hashData.clear();
    for(int i = 0; i < m_list.size(); ++i) {
        idx = getCurIndex(nPos++);
        m_model->setData(idx, m_list[i], Qt::DisplayRole);
    }
}

void DlgTableWork::resizeHorHeader()
{
    int nSections = m_tableAttr.nBlocks * MAX_COLUMN;
    if(m_tableAttr.newAttr) {
        m_tableAttr.newAttr = false;
        // установка ширины секций заголовка сразу после обновления числа данных таблицы
        for(int i = 0; i < nSections; ++i) {
            if(NUM_EL == i % MAX_COLUMN)
                m_tableAttr.sz[i] = 25;
            else if(SYS_EL == i % MAX_COLUMN)
                m_tableAttr.sz[i] = 21;
            else {
                m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
                m_tableAttr.sz[i] = m_table->columnWidth(i); // запомним ее для дальнейшей отрисовки
            }
            m_table->setColumnWidth(i, m_tableAttr.sz[i]); // отрисуем все секции со своими размерами
        }
    }
    else {
        // измерение ширины в течение остального времени (пока нет обновления числа данных)
        for(int i = 0; i < nSections; ++i) {
            if(SYS_EL == i % MAX_COLUMN) // ширину колонки SYS_EL зафиксируем
                m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            else
                m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
            m_tableAttr.sz[i] = m_table->columnWidth(i); // измерение ширины, установленной юзером
        }
    }
}

//--------------------------------------------------------------------

void DlgTableWork::slotHeaderColumnClicked(int nColumn)
{   // создадим комбобокс на заголовке для глобального выбора
    // системы счисления данных переменных
    int curTab  = m_mainWnd_4TableWork->currentTabWork();
    if(m_mainWnd_4TableWork->m_dlgTableWorkVector[curTab]->m_varSelVector.isEmpty())
        return;
    if((nColumn - SYS_EL) % MAX_COLUMN == 0) {
        // прямоугольник всего горизонтального заголовка
        QRect rectH = m_table->horizontalHeader()->contentsRect();
        // прямоугольник всего вертикального заголовка
        QRect rectV = m_table->verticalHeader()->contentsRect();
        // ширина секции горизонтального заголовка
        int sz = m_table->horizontalHeader()->sectionSize(nColumn);
        // величина сдвига таблицы после скроллинга
        int pos = m_table->horizontalHeader()->sectionViewportPosition(nColumn);
        QRect rect2 { pos+rectV.width(), -rectH.bottom(), sz, 40 };

        QComboBox* comboSys = new QComboBox(this); // !!! не забыть удалить после закрытия
        comboSys->setEditable(false);
        comboSys->setStyleSheet(StyleHelper::getComboBoxDelegateStyleSheet());

        QListView *view = new QListView(comboSys);
        view->setStyleSheet("QListView::item{height: 18px}");
        comboSys->setView(view);
        comboSys->addItems(QStringList() << "D" << "H");
        comboSys->setGeometry(rect2);
        comboSys->setCurrentIndex(m_globalSys);
        comboSys->showPopup();
        connect(comboSys, &QComboBox::activated,
                this, &DlgTableWork::slotComboHeaderActivated,
                Qt::SingleShotConnection);
    }
}

void DlgTableWork::slotComboHeaderActivated(int index)
{
    uint32_t sysId = 0;
    switch(index) {
    case DECSYS:
        sysId = DECSYS;
        break;
    case HEXSYS:
        sysId = HEXSYS;
        break;
    }
    m_globalSys = sysId;
    // все переменные переводим в выбранную систему счисления
    for(int i = 0; i < m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector.size(); ++i) {
        if(m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector[i].varProp.sysID != FLOATSYS) {
            m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector[i].varProp.sysID = sysId;
        }
    }
    updateDataWorkTable();
}

void DlgTableWork::slotItemClicked(const QModelIndex &index)
{
    if( (m_indexNotUsed.row() < index.row()) &&
        (m_indexNotUsed.column() < index.column()))
        return; // если клик вне таблицы
    if (!index.isValid())
        return;
    if((index.column() - NAME_EL) % MAX_COLUMN == 0) {
        // вывод окна с именем и адресом переменной
        int column = index.column();
        // вычислим номер позиции переменной в векторе
        int pos = ((column - NAME_EL) / MAX_COLUMN) * m_tableAttr.maxRowsByVert + index.row();
        QRect rect = m_table->visualRect(index);
        // точка расположения итема переменной в списке в глобальных координатах экрана
        QPoint pnt = this->mapToGlobal(QPoint(rect.right(), rect.bottom()));
        emit signalCreateVarAddrDlg(pos, &pnt);
    }
    if((index.column() - SYS_EL) % MAX_COLUMN == 0) {
        // запускаем createEditor делегата QComboBox
        m_table->edit(index);
    }
}

void DlgTableWork::slotCellSysComboActivated(int pos, QModelIndex &index)
{
    if(pos < 0)
        return;
    if (!index.isValid())
        return;
    // вычислим номер позиции переменной в векторе
    int curPos = ((index.column() - SYS_EL) / MAX_COLUMN) * m_tableAttr.maxRowsByVert + index.row();
    uint32_t sysId = 0;
    switch(pos) {
    case DECSYS:
        sysId = DECSYS;
        break;
    case HEXSYS:
        sysId = HEXSYS;
        break;
    case BINSYS:
        showBinEditor(index);
        return;
    }
    for(int i = 0; i < m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector.size(); ++i) {
        if(checkVarId(m_varSelVector[curPos], m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector[i])) {
            m_mainWnd_4TableWork->m_dlgVarSel->m_VarSelGlobalVector[i].varProp.sysID = sysId;
            break;
        }
    }
    updateDataWorkTable();
}

void DlgTableWork::showBinEditor(QModelIndex &index)
{
    int column = index.column();
    // вычислим номер позиции переменной в векторе
    int pos = ((column - NAME_EL) / MAX_COLUMN) * m_tableAttr.maxRowsByVert + index.row();
    QRect rect = m_table->visualRect(index);
    // точка расположения итема переменной в списке в глобальных координатах экрана
    QPoint pnt = this->mapToGlobal(QPoint(rect.right(), rect.bottom()));
    var_t var = m_varSelVector[pos];
    emit signalCreateBinEditDlg(var, &pnt);
}

#include <QScrollBar>
int DlgTableWork::getTableVSize()
{	// рассчитывает макс.число строк по вертикали таблицы
    QRect tblRect = m_table->viewport()->geometry();
    //> максимальное число строк, которое может вписаться в высоту
    //> представления таблицы с учетом высоты полосы
    //> горизонтального скроллинга
    QScrollBar *sbar = m_table->horizontalScrollBar();
    const int sbarH = sbar->height();
    int rowH = m_table->verticalHeader()->minimumSectionSize();
    if(!rowH)
        return 20;
    return ((tblRect.height() - sbarH) / rowH);
}

