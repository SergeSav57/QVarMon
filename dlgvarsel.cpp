#include <QDebug>
#include <QList>
#include <QScrollBar>
#include "dlgvarsel.h"
#include "ui_dlgvarsel.h"
#include "mainwindow.h"
#include <QStandardItemModel>   // для ComboBox (подсветка пунктов комбобокса)
// #include "mystyles.h"

// #include "LoggingCategories.h"

MainWindow  *m_mainWnd_4VarSel;

DlgVarSel::DlgVarSel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgVarSel)
{
    ui->setupUi(this);
    // уберем знак вопроса в заголовке диалога
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle("Выбор параметров для мониторинга");

    m_mainWnd_4VarSel = static_cast<MainWindow *>(parent);

    m_tableAll = ui->TABLE_ALLPAR;
    m_modelAll = new TableSelModel(0, 3, m_tableAll, this);
    m_tableAll->setModel(m_modelAll);
    itemSelectionModelAll = m_tableAll->selectionModel();
    m_tableAll->setSelectionModel(itemSelectionModelAll);

    m_tableSel = ui->TABLE_SELPAR;
    m_modelSel = new TableSelModel(0, 3, m_tableSel, this);
    m_tableSel->setModel(m_modelSel);
    itemSelectionModelSel = m_tableSel->selectionModel();
    m_tableSel->setSelectionModel(itemSelectionModelSel);

    // кнопка старт/стоп записи на главном окне
    connect(this, &DlgVarSel::signalBtnStartRecEn,
            m_mainWnd_4VarSel, &MainWindow::slotBtnStartRecEn,
            Qt::DirectConnection);
    // кнопка выбора параметров для записи на главном окне
    connect(this,  &DlgVarSel::signalBtnSelRecParamEn,
            m_mainWnd_4VarSel, &MainWindow::slotBtnSelRecParamEn,
            Qt::DirectConnection);
    // установим соединение "клик на колонке -> сортировка" по алфавиту
    connect(m_tableAll->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &DlgVarSel::slotSortByColumn,
            Qt::DirectConnection);

    ui->COMBOBOX_PARAM->addItem("Мониторинг");
    ui->COMBOBOX_PARAM->addItem("Массивы");
    ui->COMBOBOX_PARAM->addItem("График");
    ui->COMBOBOX_PARAM->addItem("Запись");
    ui->COMBOBOX_PARAM->setCurrentIndex(0);

    ui->COMBOBOX_PARAM->installEventFilter(this);
    m_tableSel->viewport()->installEventFilter(this);

    // по умолчанию работаем с мониторингом переменных
    m_regOPERATION = MODE_PARAM;
    ui->BTN_CLR_FILTER->setEnabled(false);

    // посылка адресов переменных в девайс
    connect(this, &DlgVarSel::signalSendSelParamToDevice,
            m_mainWnd_4VarSel, &MainWindow::slotSendSelParamToDevice,
            Qt::DirectConnection);
}

DlgVarSel::~DlgVarSel()
{
    delete ui;
}

void DlgVarSel::showDlgSel()
{
    int index = ui->COMBOBOX_PARAM->currentIndex();
    setComboParam(index);
    setButtonsEn();
    this->show();
    checkStateAllParamTable();
}

bool DlgVarSel::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type type = event->type();
    Q_UNUSED(type);
    if (obj == ui->COMBOBOX_PARAM) {
        // debug
        // static int cnt = 0;
        // qDebug(logInfo()) << cnt++ << " eventFilter() event_type ="<<type;
        if (event->type() == QEvent::MouseButtonPress) {
            /// qDebug()<<"DlgVarSel:COMBOBOX_PARAM -> QEvent::MouseButtonPress";
            setComboParamItemsEn(); // подсветка строк в комбобоксе
        }
    }
    if (obj == m_tableSel->viewport()) {
        // проверка на клик мыши вне таблицы
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QModelIndex idx = m_tableSel->indexAt(mouseEvent->pos());
            if(idx.row() == -1){
                m_tableSel->clearSelection();
                setButtonsEn();
            }
        }
        if (event->type() == QEvent::MouseButtonRelease)
        {   //> срабатывает, когда закончен выбор(мышкой) строк в таблице
            //> и число выделенных уже меняться не будет -
            //> это нужно для блокировки кнопок перемещения вверх-вниз при
            //> крайних значениях строк таблицы
            setButtonsEn();
        }
    }
    return QWidget::eventFilter(obj, event);
}

// =======================================================================
// === Управление ComboBox'сами ==========================================
// =======================================================================

//! потом из ini-файла добавлять все имена вкладок, открытых в прошлый сеанс
void DlgVarSel::setComboTabsName(QString tabName)
{   // при создании новой вкладки добавить в COMBOBOX_TABS имя вкладки
    ui->COMBOBOX_TABS->addItem(tabName);
    int index = ui->COMBOBOX_TABS->findText(tabName);
    ui->COMBOBOX_TABS->setCurrentIndex(index);
}

void DlgVarSel::updateComboTabsName(int index, QString tabName)
{
    ui->COMBOBOX_TABS->setItemText(index, tabName);
}

void DlgVarSel::removeComboTabsIndex(int index)
{   // при удалении вкладки удалить в COMBOBOX_TABS имя вкладки
    ui->COMBOBOX_TABS->removeItem(index);
}

void DlgVarSel::setComboTabsIndex(int index)
{   // установка позиции комбо для текущей вкладки
    ui->COMBOBOX_TABS->setCurrentIndex(index);
    m_mainWnd_4VarSel->setCurrentTabWork(index);
}

void DlgVarSel::on_COMBOBOX_TABS_activated(int index)
{   // переключает вкладки в режиме "Мониторинг"
    m_mainWnd_4VarSel->setCurrentTabWork(index);
    setAllParam();

    int curTab = m_mainWnd_4VarSel->currentTabWork();
    m_mainWnd_4VarSel->m_dlgTableWorkVector[curTab]->updateDataWorkTable();
}

void DlgVarSel::setComboParamIndex(int index)
{
    ui->COMBOBOX_PARAM->setCurrentIndex(index);
    emit ui->COMBOBOX_PARAM->activated(index);
}

void DlgVarSel::setComboParam(int index)
{
    m_tableAll->clearSelection();
    m_tableSel->clearSelection();
    m_fButtonsUpdateEn = false; // не изменять состояния кнопок
    switch(index) {
    case ALL_PARAM  : setAllParam();    ui->COMBOBOX_TABS->show(); break; // показать комбо переключателя вкладок
    case ALL_ARRAY  : setAllArray();    ui->COMBOBOX_TABS->hide(); break;
    case CHART_PARAM: setChartParam();  ui->COMBOBOX_TABS->hide(); break;
    case REC_PARAM  : setRecordParam(); ui->COMBOBOX_TABS->hide(); break;
    }
    m_fButtonsUpdateEn = true;
}

void DlgVarSel::on_COMBOBOX_PARAM_activated(int index)
{   // если изменили выбор режима в комбобоксе
    setComboParam(index);
}

// =======================================================================
// === Сортировка ========================================================
// =======================================================================

void DlgVarSel::slotSortByColumn(int nColumn)
{   // сортировка строк имен переменных и имен узлов по алфавиту
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;

    int section = m_tableAll->horizontalHeader()->sortIndicatorSection();
     // при первом заполнении была сортировка по возрастанию
    static Qt::SortOrder indicatorOrderName = Qt::AscendingOrder;
    static Qt::SortOrder indicatorOrderUnit = Qt::AscendingOrder;
    if((section != NAME_SEL) && (section != UNIT_SEL)) {
        m_tableAll->horizontalHeader()->setSortIndicator(NAME_SEL, indicatorOrderName);
        return;
    }

    if(vAll->empty())
        return;
    //!!! сортировка по двум колонкам NAME_SEL и UNIT_SEL
    // сортируем колонку с именами переменных, затем с именами узлов
    if(nColumn == NAME_SEL) {
        if (indicatorOrderName == Qt::AscendingOrder) {
            indicatorOrderName = Qt::DescendingOrder; // сортировка по убыванию
            std::sort(std::begin(*vAll), std::end(*vAll), compareDwnName);
            m_tableAll->horizontalHeader()->setSortIndicator(NAME_SEL, indicatorOrderName);
        }
        else {
            indicatorOrderName = Qt::AscendingOrder; // сортировка по возрастанию
            std::sort(std::begin(*vAll), std::end(*vAll), compareUpName);
            m_tableAll->horizontalHeader()->setSortIndicator(NAME_SEL, indicatorOrderName);
        }
        fillAllParamTable();
    }
    // сортируем колонку с именами узлов, затем с именами переменных
    if(nColumn == UNIT_SEL) {
        if (indicatorOrderUnit == Qt::AscendingOrder) {
            indicatorOrderUnit = Qt::DescendingOrder; // сортировка по убыванию
            std::sort(std::begin(*vAll), std::end(*vAll), compareDwnUnit);
            m_tableAll->horizontalHeader()->setSortIndicator(UNIT_SEL, indicatorOrderUnit);
        }
        else {
            indicatorOrderUnit = Qt::AscendingOrder; // сортировка по возрастанию
            std::sort(std::begin(*vAll), std::end(*vAll), compareUpUnit);
            m_tableAll->horizontalHeader()->setSortIndicator(UNIT_SEL, indicatorOrderUnit);
        }
        fillAllParamTable();
    }
}

// =======================================================================
// === Работа с таблицами (инициализация, заполнение) ====================
// =======================================================================

void DlgVarSel::slotTableClear()
{   // очистка таблиц
    while(m_modelAll->rowCount())
        m_modelAll->removeRow(0);
    while(m_modelSel->rowCount())
        m_modelSel->removeRow(0);
}

void DlgVarSel::on_TABLE_ALLPAR_itemSelectionChanged()
{   // При нажатии левой кнопкой по элементам.
    // В момент, когда изменили выбор режима в комбобоксе,
    // setButtonsEn() не должна вызываться
    if(m_fButtonsUpdateEn) { // если изменили выбор режима в комбобоксе
        // qDebug()<<"on_TABLE_ALLPAR_itemSelectionChanged() -> setButtonsEn()";
        setButtonsEn();
    }
}

void DlgVarSel::initAllParamTable()
{
    // Разрешаем множественное выделение элементов с Ctrl и Shift
    m_tableAll->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Разрешаем выделение построчно
    m_tableAll->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Растягиваем последнюю колонку на всё доступное пространство
    m_tableAll->horizontalHeader()->setStretchLastSection(true);
    // установим фиксированную высоту заголовка
    m_tableAll->horizontalHeader()->setFixedHeight(21);
    // Ограничим минимальную высоту строк
    m_tableAll->verticalHeader()->setMinimumSectionSize(18);
    m_tableAll->verticalHeader()->setDefaultSectionSize(18);
    m_tableAll->verticalHeader()->setStretchLastSection(false);
    // цвет заголовков QTableWidget
    QString headerStyle = myHeaderStyle;
    // цвет выделенной строки и цвет фонта
    QString selectedStyle = mySelectedStyle;
    m_tableAll->setStyleSheet(headerStyle + selectedStyle);
    // разрешим закраску фоном итемов
    m_tableAll->setAlternatingRowColors(true);
    // установим автоподстройку ширины заголовков колонок под размер записей списка
    m_tableAll->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // установим значек "стрелка" на заголовке колонки 0 -> сортировка по алфавиту вверх-вниз
    m_tableAll->horizontalHeader()->setSortIndicatorShown(true);
    m_tableAll->horizontalHeader()->setSortIndicator(NAME_SEL, Qt::AscendingOrder);
    m_tableAll->horizontalHeader()->setSortIndicator(UNIT_SEL, Qt::AscendingOrder);
    // Включаем сетку
    m_tableAll->setShowGrid(true);
}

void DlgVarSel::fillAllParamTable()
{
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    if(vAll->empty())
        return;

    int cntAll = (int)vAll->size();
    m_listAll.clear();
    for(int i = 0; i < cntAll; ++i) {
        m_listAll.emplace_back((*vAll)[i].varID.varName);
        m_listAll.emplace_back((*vAll)[i].varID.unitName);
        m_listAll.emplace_back((*vAll)[i].varType.altType);
        if(m_regOPERATION & MODE_ARRAY) {
            QString strArr = QString("%1x%2")
                                 .arg((*vAll)[i].varProp.arrRow)
                                 .arg((*vAll)[i].varProp.arrCol);
            m_listAll.emplace_back(strArr);
        }
    }
    int column = 0;
    if(m_regOPERATION & MODE_ARRAY)
        column = 4;
    else
        column = 3;

    m_modelAll->setRowCol(cntAll, column);
    m_tableAll->setModel(m_modelAll);
    m_modelAll->m_hashData.clear();

    QModelIndex idx;
    for(int i = 0; i < m_listAll.size(); ++i) {
        int curRow = (int) (i / column);
        int curCol = i % column;
        idx = m_modelAll->index(curRow, curCol, QModelIndex());
        m_modelAll->setData(idx, m_listAll[i], Qt::DisplayRole);
    }
    for(int i = 0; i < cntAll; ++i) {
        // определяем выбран ли элемент в таблице и устанавливаем чекбокс, если выбран
        bool flag = getSelectedFlagParam((*vAll)[i]);
        idx = m_modelAll->index(i, 0, QModelIndex());
        m_modelAll->setCheckState(idx, flag);
    }
    ui->LABEL_ALLPAR_SUM->setText(QString("Всего параметров: %1").arg(cntAll));
}

void DlgVarSel::unCheckedAllParamTable(QVector<var_t> &delVec)
{
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    if(vAll->empty())
        return;
    for(int i = 0; i < vAll->size(); ++i) {
        for(int j = 0; j < delVec.size(); ++j) {
            if(checkVarId((*vAll)[i], delVec[j])) {
                QModelIndex idx = m_modelAll->index(i, 0);
                m_modelAll->setCheckState(idx, false);
            }
        }
        // // определяем отмечен ли элемент в таблице и выключаем чекбокс, если отмечен
        // QModelIndex idx = m_modelAll->index(i, 0, QModelIndex());
        // bool state = m_modelAll->getCheckState(idx);
        // m_modelAll->setCheckState(idx, !state);
    }
}

void DlgVarSel::initSelParamTable()
{
    // разрешаем Update таблицы
    m_tableSel->setUpdatesEnabled(true);
    // Разрешаем множественное выделение элементов с клавишами 'Ctrl' и 'Shift'
    m_tableSel->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Разрешаем выделение построчно
    m_tableSel->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Растягиваем последнюю колонку на всё доступное пространство
    m_tableSel->horizontalHeader()->setStretchLastSection(true);
    // установим фиксированную высоту заголовка
    m_tableSel->horizontalHeader()->setFixedHeight(21);
    // Ограничим минимальную высоту строк
    m_tableSel->verticalHeader()->setMinimumSectionSize(18);
    m_tableSel->verticalHeader()->setDefaultSectionSize(18);
    m_tableSel->verticalHeader()->setStretchLastSection(false);
    // цвет заголовков QTableWidget
    QString headerStyle = myHeaderStyle;
    // цвет выделенной строки и цвет фонта
    QString selectedStyle = mySelectedStyle;
    m_tableSel->setStyleSheet(headerStyle + selectedStyle);
    // разрешим закраску фоном итемов
    m_tableSel->setAlternatingRowColors(true);
    // установим автоподстройку ширины заголовков колонок под размер записей списка
    m_tableSel->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // Включаем сетку
    m_tableSel->setShowGrid(true);
}

void DlgVarSel::fillSelParamTable()
{
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vSel = ds.vSel;
    if(vSel->empty()) {
        int column = 0;
        if(m_regOPERATION & MODE_ARRAY)
            column = 4;
        else
            column = 3;
        m_modelSel->setRowCol(0, column);
        m_tableSel->setModel(m_modelSel);
        m_modelSel->m_hashData.clear();
        ui->LABEL_SELPAR_SUM->setText(QString("Выбрано параметров: 0"));
        return;
    }
    int cntSel = (int)vSel->size();
    m_listSel.clear();
    for(int i = 0; i < cntSel; ++i) {
        m_listSel.emplace_back((*vSel)[i].varID.varName);
        m_listSel.emplace_back((*vSel)[i].varID.unitName);
        m_listSel.emplace_back((*vSel)[i].varType.altType);
        if(m_regOPERATION & MODE_ARRAY) {
            QString strArr = QString("%1x%2")
                                 .arg((*vSel)[i].varProp.arrRow)
                                 .arg((*vSel)[i].varProp.arrCol);
            m_listSel.emplace_back(strArr);
        }
    }
    int column = 0;
    if(m_regOPERATION & MODE_ARRAY)
        column = 4;
    else
        column = 3;
    m_modelSel->setRowCol(cntSel, column);
    m_tableSel->setModel(m_modelSel);
    m_modelSel->m_hashData.clear();

    QModelIndex idx;
    for(int i = 0; i < m_listSel.size(); ++i) {
        int curRow = (int) (i / column);
        int curCol = i % column;
        idx = m_modelSel->index(curRow, curCol, QModelIndex());
        m_modelSel->setData(idx, m_listSel[i], Qt::DisplayRole);
    }

    ui->LABEL_SELPAR_SUM->setText(QString("Выбрано параметров: %1").arg(cntSel));
}

// ======================================================================
// === Действия мыши в таблицах =========================================
// ======================================================================

void DlgVarSel::on_TABLE_ALLPAR_pressed(const QModelIndex &index)
{
    itemSelectionModelAll->setCurrentIndex(index, QItemSelectionModel::Select);
    setButtonsEn();
}

void DlgVarSel::on_TABLE_ALLPAR_clicked(const QModelIndex &index)
{   // клик на строке в списке ALL
    QVector<var_t>	*vAll{};
    QVector<var_t>	*vSel{};
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    vAll = ds.vAll;
    vSel = ds.vSel;
    if(vAll->empty())
        return;

    int szSel = (int)vSel->size();
    int row = index.row();
    QModelIndex idx = m_modelAll->index(row, 0, QModelIndex());
    bool state = m_modelAll->getCheckState(idx);
    if(state) { // если чекбокс был отмечен
        bool isUsed = false;
        for(int i=0; i < szSel; ++i) {
            if(checkVarId((*vAll)[row], (*vSel)[i])) {
                isUsed = true;
                break; // параметр уже есть в списке SEL -выходим
            }
        }
        if(!isUsed) { // параметр еще не выбран
            // при следующей перерисовке ALL итем будет отмечен
            if((m_regOPERATION & MODE_FIND) != MODE_FIND) { // если НЕ режим поиска
                // при следующей перерисовке ALL чекбокс будет отмечен
                setSelectedFlagParam(vAll, row, true);
                vSel->emplace_back((*vAll)[row]); // заполняем вектор выбранных параметров
            }
            else { // если режим поиска
                // отметим чекбоксы в таблице всех параметров
                // и заполним вектор выбранных параметров
                for(int j=0; j < (int)m_VarAllGlobalVector.size(); ++j) {
                    if(checkVarId(m_VarAllGlobalVector[j], (*vAll)[row])) {
                        // при выходе из режима поиска в ALL чекбокс будет отмечен
                        setSelectedFlagParam(&m_VarAllGlobalVector, j, true);
                        // заполняем вектор выбранных параметров
                        vSel->emplace_back(m_VarAllGlobalVector[j]);
                        break;
                    }
                }
            }
        }
    }
    else { // если чекбокс был снят
        setSelectedFlagParam(vAll, row, false);
        for(int i=0; i < szSel; ++i) {
            if(checkVarId((*vAll)[row], (*vSel)[i])) { // совпал ID параметра ALL и SEL списков
                vSel->remove(i); // удалим параметр из SEL
                break;
            }
        }
    }

    checkSelParamGlobal();

    checkStateAllParamTable();
    fillSelParamTable();
    // qDebug()<<"on_TABLE_ALLPAR_itemClicked() -> setButtonsEn()";
    setButtonsEn();
}

void DlgVarSel::on_TABLE_SELPAR_pressed(const QModelIndex &index)
{   Q_UNUSED(index);
    setButtonsEn();
}

// =======================================================================
// === Установка, снятие и проверка признаков выбора параметров ==========
// === и их отмеченных состояний  для мониторинга для разных режимов =====
// =======================================================================

void DlgVarSel::setSelectedFlagParam(QVector<var_t> *vec, int index, bool flag)
{   // устанавливает признак выбора элемента в таблице vec
    if(m_regOPERATION & MODE_PARAM)
        (*vec)[index].varFlags.f_Monitor = flag;
    if(m_regOPERATION & MODE_CHART)
        (*vec)[index].varFlags.f_Chart = flag;
    if(m_regOPERATION & MODE_RECORD)
        (*vec)[index].varFlags.f_Record = flag;
    if(m_regOPERATION & MODE_ARRAY)
        (*vec)[index].varFlags.f_Array = flag;
}

bool DlgVarSel::getSelectedFlagParam(var_t &var)
{   // определяет выбран ли элемент var в таблице
    bool flag = false;
    if(m_regOPERATION & MODE_PARAM)
        flag = var.varFlags.f_Monitor;
    if(m_regOPERATION & MODE_CHART)
        flag = var.varFlags.f_Chart;
    if(m_regOPERATION & MODE_RECORD)
        flag = var.varFlags.f_Record;
    if(m_regOPERATION & MODE_ARRAY)
        flag = var.varFlags.f_Array;
    return flag;
}

void DlgVarSel::checkStateAllParamTable()
{   // определяет выбран ли элемент в таблице и, если выбран, устанавливаем чекбокс
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty() || vSel->empty())
        return;

    for(int j = 0; j < (int)vSel->size(); ++j) {
        for(int i = 0; i < (int)vAll->size(); ++i) {
            if(checkVarId((*vAll)[i], (*vSel)[j])) {
                setSelectedFlagParam(vAll, i, true);
                QModelIndex idx = m_modelAll->index(i, 0, QModelIndex());
                m_modelAll->setCheckState(idx, true);
                break;
            }
        }
    }
}

void DlgVarSel::checkSelParamGlobal()
{   // проверка наличия/отсутствия всех параметров во всех вкладках
    // в глобальном векторе параметров и в случае
    // их отсутствия/наличия, добавляем/удаляем их
    if((m_regOPERATION & MODE_PARAM) != MODE_PARAM)
        return;
    // проверка наличия дублирования параметров...
    // - пробежим по вкладкам
    for(int i = 0; i < (int)m_mainWnd_4VarSel->m_dlgTableWorkVector.size(); ++i) {
        // копия временного вектора выбранных параметров вкладки
        // (после изменений временный вектор копируеся в рабочий)
        QVector<var_t>	vec = m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_varSelVectorTmp;
        // проверим, есть ли текущий параметр вкладки в составе глобального вектора
        for(int j = 0; j < (int)vec.size(); ++j) {
            bool isUsed = false;
            var_t vTmpParam = vec[j]; // искомый параметр
            for(int k = 0; k < (int)m_VarSelGlobalVector.size(); ++k) {
                if(checkVarId(vTmpParam, m_VarSelGlobalVector[k])) {
                    isUsed = true; // уже есть этот параметр
                    break; // переходим к следующему параметру вкладки
                }
            }
            if(!isUsed) { // если в глобальном векторе нет этого параметра -сунем его туда
                m_VarSelGlobalVector.emplace_back(vTmpParam);
                // m_fSelectedChanged = true; // есть изменения (вступают в силу после кнопки OK в этом окне)
            }
        } // переходим к следующей вкладке
    }

    // проверка на отсутствие текущего параметра вкладки в составе глобального вектора
    for(int i = (int)m_VarSelGlobalVector.size() - 1; i >= 0 ; --i) { // удаление делаем с конца вектора
        var_t vTmpParam = m_VarSelGlobalVector[i]; // искомый параметр
        // - пробежим по вкладкам
        bool isUsed = false;
        for(int j = 0; j < (int)m_mainWnd_4VarSel->m_dlgTableWorkVector.size(); ++j) {
            for(int k = 0; k < (int)m_mainWnd_4VarSel->m_dlgTableWorkVector[j]->m_varSelVectorTmp.size(); ++k) {
                if(checkVarId(vTmpParam, m_mainWnd_4VarSel->m_dlgTableWorkVector[j]->m_varSelVectorTmp[k])) {
                    isUsed = true; // есть этот параметр во вкладке
                    break; // переходим к следующей вкладке
                }
            }
        }
        if(!isUsed) { // если во вкладках нет этого параметра
            // :=> заблокируем передачу новых данных, пока удаляются параметры
            QMutexLocker lockerVarData(&m_mainWnd_4VarSel->m_mutexUpdateVarData);
            // удалим параметр из общего списка выбранных для вкладок
            m_VarSelGlobalVector.remove(i);
            // удалим параметр из SEL для графика
            eraseParam(m_chartSelVectorTmp, vTmpParam);
            // удалим параметр из SEL для записи
            eraseParam(m_recSelVectorTmp, vTmpParam);
            lockerVarData.unlock();
        }
    }
    for(int i = 0; i < (int)m_mainWnd_4VarSel->m_dlgTableWorkVector.size(); ++i) {
        m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->initWorkTable();
    }
}

// =======================================================================
// === Удаление параметра из списка (вектора) выбранных для вкладок
// =======================================================================

void DlgVarSel::eraseParam(QVector<var_t> &vDest,var_t vDel)
{   // удаляет параметр vDel из вектора vDest, если он там есть
    for(int i = (int)vDest.size() - 1; i >= 0 ; --i) { // удаление делаем с конца вектора
        if(checkVarId(vDel, vDest[i])) {
            // удалим параметр из vDest
            vDest.remove(i);
            break;
        }
    }
}

// =======================================================================
// === Кнопки перемещения ================================================
// =======================================================================

void DlgVarSel::on_BTN_SELALL_clicked()
{   // заполняет лист SEL всем имеющимися параметрами в листе ALL (без использ.чекбокса)
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty())
        return;

    for(int i=0; i < (int)vAll->size(); ++i) {
        bool isUsed = false; // проверим есть ли текущий параметр в составе выбранных
        //! (для себя :) хотел применить foreach, но вычитал, что получаем оверхед по времени
        //! компиляции на обработку файлов препроцессором, поскольку выполняется
        //! предварительное копирование контейнера
        for(int j=0; j < (int)vSel->size(); ++j) {
            if(checkVarId((*vAll)[i], (*vSel)[j])) {
                isUsed = true; // в SEL уже есть этот параметр
                break; // переходим к следующему параметру листа ALL
            }
        }
        if(!isUsed) { // параметр еще не выбран
            if((m_regOPERATION & MODE_FIND) != MODE_FIND) { // если НЕ режим поиска
                // при следующей перерисовке ALL чекбокс будет отмечен
                setSelectedFlagParam(vAll, i, true);
                vSel->emplace_back((*vAll)[i]); // заполняем вектор выбранных параметров
            }
            else { // если режим поиска
                // отметим чекбоксы в таблице всех параметров
                // и заполним вектор выбранных параметров
                for(int j=0; j < (int)m_VarAllGlobalVector.size(); ++j) {
                    if(checkVarId(m_VarAllGlobalVector[j], (*vAll)[i])) {
                        // при выходе из режима поиска в ALL чекбокс будет отмечен
                        setSelectedFlagParam(&m_VarAllGlobalVector, j, true);
                        // заполняем вектор выбранных параметров
                        vSel->emplace_back(m_VarAllGlobalVector[j]);
                        break;
                    }
                }
            }
        }
    }

    checkSelParamGlobal();
    fillSelParamTable();
    checkStateAllParamTable();
    // снимем всё выделение в списке
    m_tableAll->clearSelection();
    // qDebug()<<"on_BTN_SELALL_clicked() -> setButtonsEn()";
    setButtonsEn();
    if(m_regOPERATION & MODE_FIND) {
        // выход из режима поиска
        emit ui->BTN_CLR_FILTER->clicked();
    }
}

void DlgVarSel::on_BTN_DESELALL_clicked()
{   // удаляет все элементы из текущего(curTab) списка SEL
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty())
        return;

    QVector<var_t> delSelTmp{}; // удаляемые параметры
    delSelTmp = *vSel;
    // удаляем элементы из SEL списка
    for(int i = (int)vSel->size() - 1; i >= 0; --i) { // удаляем с конца списка SEL
        for(int j=0; j < (int)vAll->size(); ++j) {
            if(checkVarId((*vSel)[i], (*vAll)[j])) {
                if((m_regOPERATION & MODE_FIND) != MODE_FIND) { // если НЕ режим поиска
                    // при следующей перерисовке ALL итем НЕ будет отмечен
                    setSelectedFlagParam(vAll, j, false);
                    // удалим параметр из SEL
                    vSel->remove(i);
                }
                else { // если режим поиска
                    // выключим чекбокс в таблице всех параметров
                    // и удалим параметр из вектора выбранных параметров
                    for(int j = 0; j < (int)m_VarAllGlobalVector.size(); ++j) {
                        if(checkVarId(m_VarAllGlobalVector[j], (*vAll)[i])) {
                            // при выходе из режима поиска в ALL чекбокс НЕ будет отмечен
                            setSelectedFlagParam(&m_VarAllGlobalVector, j, false);
                            // удалим параметр из SEL
                            vSel->remove(i);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    checkSelParamGlobal();
    unCheckedAllParamTable(delSelTmp);
    setButtonsEn();
}

void DlgVarSel::on_BTN_SELPAR_clicked()
{   // заполняет лист SEL параметрами, выбранными в листе ALL
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty())
        return;

    for(int i=0; i < (int)vAll->size(); ++i) { // пробежим по всем параметрам таблицы ALL
        QModelIndex idx = m_modelAll->index(i, 0, QModelIndex());
        bool state = m_modelAll->getCheckState(idx);
        bool selected = getSelectedStateParam(m_tableAll, idx);
        // если параметр в ALL выбран, проверим нет ли его в SEL
        if(state || selected) { // если чекбокс отмечен
            bool isUsed = false; // проверим есть ли текущий параметр в составе выбранных в SEL
            for(int j=0; j < (int)vSel->size(); ++j) {
                if(checkVarId((*vSel)[j], (*vAll)[i])) {
                    isUsed = true; // в SEL уже есть этот параметр
                    break; // переходим к след.параметру ALL
                }
            }
            if(!isUsed) { // если параметр есть в составе выбранных в SEL
                if((m_regOPERATION & MODE_FIND) != MODE_FIND) { // если НЕ режим поиска
                    // при следующей перерисовке ALL чекбокс будет отмечен
                    setSelectedFlagParam(vAll, i, true);
                    vSel->emplace_back((*vAll)[i]); // заполняем вектор выбранных параметров
                }
                else { // если режим поиска MODE_FIND
                    // отметим чекбоксы в таблице всех параметров
                    // и заполним вектор выбранных параметров
                    for(int j=0; j < (int)m_VarAllGlobalVector.size(); ++j) {
                        if(checkVarId(m_VarAllGlobalVector[j], (*vAll)[i])) {
                            // при выходе из режима поиска в ALL чекбокс будет отмечен
                            setSelectedFlagParam(&m_VarAllGlobalVector, j, true);
                            // заполняем вектор выбранных параметров
                            vSel->emplace_back(m_VarAllGlobalVector[j]);
                            break;
                        }
                    }
                }
            }
        }
    }

    checkSelParamGlobal();
    checkStateAllParamTable();
    fillSelParamTable();
    // снимем выделение в списке
    m_tableAll->clearSelection();
    setButtonsEn();
    if(m_regOPERATION & MODE_FIND) {
        // выход из режима поиска
        emit ui->BTN_CLR_FILTER->clicked();
    }
}

void DlgVarSel::on_BTN_DESELPAR_clicked()
{   // удаляет из листа SEL параметры, выбранные в листе ALL
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty())
        return;

    QVector<var_t> delSelTmp{}; // удаляемые параметры
    // удаляем элементы из SEL списка и снимаем выбор соответствующих строк
    for(int i = (int)vSel->size() - 1; i >= 0; --i) { // удаляем с конца списка SEL
        QModelIndex idx = m_modelSel->index(i, 0, QModelIndex());
        bool selected = getSelectedStateParam(m_tableSel, idx);
        if(selected) { // строка выбрана
            for(int j=0; j < (int)vAll->size(); ++j) {
                if(checkVarId((*vSel)[i], (*vAll)[j])) {
                    if((m_regOPERATION & MODE_FIND) != MODE_FIND) { // если НЕ режим поиска
                        // при следующей перерисовке ALL итем НЕ будет отмечен
                        setSelectedFlagParam(vAll, j, false);
                        // снимем выделение строк
                        selectTableRow(m_tableSel, false, idx);
                        // вставим параметр в число удаляемых
                        delSelTmp.emplace_back((*vSel)[i]);
                        // удалим параметр из SEL
                        vSel->remove(i);
                    }
                    else { // если режим поиска
                        // выключим чекбокс в таблице всех параметров
                        // и удалим параметр из вектора выбранных параметров
                        for(int j = 0; j < (int)m_VarAllGlobalVector.size(); ++j) {
                            if(checkVarId(m_VarAllGlobalVector[j], (*vAll)[i])) {
                                // при выходе из режима поиска в ALL чекбокс НЕ будет отмечен
                                setSelectedFlagParam(&m_VarAllGlobalVector, j, false);
                                // снимем выделение строк
                                selectTableRow(m_tableSel, false, idx);
                                // вставим параметр в число удаляемых
                                delSelTmp.emplace_back((*vSel)[i]);
                                // удалим параметр из SEL
                                vSel->remove(i);
                                // vSel->erase(vSel->cbegin() + i);
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    checkSelParamGlobal();
    unCheckedAllParamTable(delSelTmp);
    fillSelParamTable();
    checkStateAllParamTable();
    setButtonsEn();
}

void DlgVarSel::on_BTN_UP_clicked()
{
    m_tableSel->setFocus();
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vSel = ds.vSel;
    if(vSel->empty())
        return;

    var_t	param{};
    // список выделенных строк
    QModelIndexList listSelectedRows = itemSelectionModelSel->selectedRows();
    // список новых строк после перемещения
    QModelIndexList newListSelectedRows{};
    QModelIndex idx{};
    for(int i = 0; i < (int)listSelectedRows.size(); ++i) {
        idx = listSelectedRows[i];
        param = (*vSel)[idx.row()];
        vSel->remove(idx.row());
        vSel->insert(idx.row() -1, param);
        idx = m_modelSel->index(idx.row() -1, 0);
        newListSelectedRows << idx;
    }

    fillSelParamTable();
    // выделим новые строки
    for(int i = 0; i < (int)newListSelectedRows.size(); ++i) {
        selectTableRow(m_tableSel, true, newListSelectedRows[i]);
    }
    m_tableSel->setFocus();
    setButtonsEn();
}

void DlgVarSel::on_BTN_DOWN_clicked()
{
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vSel = ds.vSel;
    if(vSel->empty())
        return;

    var_t	param{};
    // список выделенных строк
    QModelIndexList listSelectedRows = itemSelectionModelSel->selectedRows();
    // список новых строк после перемещения
    QModelIndexList newListSelectedRows{};
    QModelIndex idx{};
    // начинаем перемещение с нижних индексов
    for(int i = (int)listSelectedRows.size() -1; i >= 0 ; --i) {
        idx = listSelectedRows[i];
        param = (*vSel)[idx.row() +1];
        vSel->remove(idx.row() +1);
        vSel->insert(idx.row(), param);
        idx = m_modelSel->index(idx.row() +1, 0);
        newListSelectedRows << idx;
    }

    fillSelParamTable();
    //! Важно : выделим новые строки тоже снизу, т.к. QModelIndexList
    //! заполняется по порядку выделения строк
    for(int i = (int)newListSelectedRows.size() -1; i >= 0 ; --i) {
        selectTableRow(m_tableSel, true, newListSelectedRows[i]);
    }
    m_tableSel->setFocus();
    setButtonsEn();
}

// =======================================================================
// === Снятие/установка выбора строки ===============================
// =======================================================================

void DlgVarSel::selectTableRow(QTableView *table, bool select, const QModelIndex &index)
{   // снятие/установка выбора строки
    if(table == m_tableAll) {
        if(select)
            itemSelectionModelAll->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        else
            itemSelectionModelAll->select(index, QItemSelectionModel::Deselect);
    }
    if(table == m_tableSel) {
        for(int i = 0; i < m_tableSel->model()->columnCount(); ++i) {
            QModelIndex idx = m_tableSel->model()->index(index.row(), i);
            if(select)
                itemSelectionModelSel->setCurrentIndex(idx, QItemSelectionModel::Select);
            else
                itemSelectionModelSel->setCurrentIndex(idx, QItemSelectionModel::Deselect);
        }
    }
}

// =======================================================================
// === Установки режимов выбора параметров ===============================
// =======================================================================

void DlgVarSel::getDestSrcVectors(DestSrc_t *ds)
{   // определяем с какими данными работаем в установленном режиме
    if(m_regOPERATION & MODE_PARAM) {
        ds->vAll = &m_VarAllGlobalVector;
        int curTab = m_mainWnd_4VarSel->currentTabWork();
        ds->vSel = &m_mainWnd_4VarSel->m_dlgTableWorkVector[curTab]->m_varSelVectorTmp;
    }
    if(m_regOPERATION & MODE_ARRAY) {
        ds->vAll = &m_ArrAllGlobalVector;
        ds->vSel = &m_ArrSelGlobalVectorTmp;
    }
    if(m_regOPERATION & MODE_CHART) {
        ds->vAll = &m_VarSelGlobalVector;
        ds->vSel = &m_chartSelVectorTmp;
    }
    if(m_regOPERATION & MODE_RECORD) {
        ds->vAll = &m_VarSelGlobalVector;
        ds->vSel = &m_recSelVectorTmp;
    }
    // для поиска параметров в таблице всех параметров
    if(m_regOPERATION & MODE_FIND) {
        ds->vAll = &m_findVector;
    }
}

void DlgVarSel::setAllParam()
{
    this->setWindowTitle("Выбор параметров для мониторинга");
    ui->LABEL_ALL_PARAM->setText("Все параметры");
    ui->LABEL_SEL_PARAM->setText("Параметры для мониторинга");
    MODE_CLEAR(m_regOPERATION);
    m_regOPERATION |= MODE_PARAM;
    QMutexLocker lockerVarData(&m_mainWnd_4VarSel->m_mutexUpdateVarData);
    filterClr();
    initAllParamTable();
    fillAllParamTable();
    initSelParamTable();
    fillSelParamTable();
    checkStateAllParamTable();
    QScrollBar *sbar = m_tableAll->verticalScrollBar();
    sbar->setValue(0);
}

void DlgVarSel::setAllArray()
{
    this->setWindowTitle("Выбор массивов параметров для мониторинга");
    ui->LABEL_ALL_PARAM->setText("Все массивы параметров");
    ui->LABEL_SEL_PARAM->setText("Массивы параметров для мониторинга");
    MODE_CLEAR(m_regOPERATION);
    m_regOPERATION |= MODE_ARRAY;
    filterClr();
    initAllParamTable();
    fillAllParamTable();
    initSelParamTable();
    fillSelParamTable();
    checkStateAllParamTable();
    QScrollBar *sbar = m_tableAll->verticalScrollBar();
    sbar->setValue(0);
}

void DlgVarSel::setChartParam()
{
    this->setWindowTitle("Выбор параметров для построения графиков");
    ui->LABEL_ALL_PARAM->setText("Доступные параметры");
    ui->LABEL_SEL_PARAM->setText("Параметры для графиков");
    MODE_CLEAR(m_regOPERATION);
    m_regOPERATION |= MODE_CHART;
    filterClr();
    initAllParamTable();
    fillAllParamTable();
    initSelParamTable();
    fillSelParamTable();
    checkStateAllParamTable();
    QScrollBar *sbar = m_tableAll->verticalScrollBar();
    sbar->setValue(0);
}

void DlgVarSel::setRecordParam()
{
    this->setWindowTitle("Выбор параметров для записи в файл");
    ui->LABEL_ALL_PARAM->setText("Доступные параметры");
    ui->LABEL_SEL_PARAM->setText("Параметры для записи");
    MODE_CLEAR(m_regOPERATION);
    m_regOPERATION |= MODE_RECORD;
    filterClr();
    initAllParamTable();
    fillAllParamTable();
    initSelParamTable();
    fillSelParamTable();
    checkStateAllParamTable();
    QScrollBar *sbar = m_tableAll->verticalScrollBar();
    sbar->setValue(0);
}

void DlgVarSel::setButtonsEn()
{   // установка состояний кнопок перемещения
    ui->BTN_DESELALL->setEnabled(false);
    ui->BTN_DESELPAR->setEnabled(false);
    ui->BTN_SELPAR->setEnabled(false);
    ui->BTN_UP->setEnabled(false);
    ui->BTN_DOWN->setEnabled(false);

    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    QVector<var_t>	*vSel = ds.vSel;
    if(vAll->empty())
        return;

    // TABLE_SELPAR
    bool flagUp   = false;
    bool flagDown = false;
    bool flagDeselPar = false;
    int sizeSel = (int)vSel->size();
    QModelIndex idx{};
    bool selected = false;
    if(m_modelSel->rowCount() == sizeSel) {
        for(int i = 0; i < sizeSel; ++i) {
            idx = m_modelSel->index(i, 0);
            selected = getSelectedStateParam(m_tableSel, idx);
            // если хотя бы один итем выбран
            if(selected)
                break;
            //     flagLeft = true; // включить кнопку BTN_DESELPAR
        }
    }

    if(selected) {
        flagDeselPar = true; // включить кнопку BTN_DESELPAR
        idx = m_modelSel->index(0, 0);
        selected = getSelectedStateParam(m_tableSel, idx);
        if(selected) // если первый итем выбран
            flagUp = false; // вЫключить кнопку BTN_UP
        else
            flagUp = true; // включить кнопку BTN_UP
        idx = m_modelSel->index(sizeSel - 1, 0);
        selected = getSelectedStateParam(m_tableSel, idx);
        if(selected) // если последний итем выбран
            flagDown = false; // вЫключить кнопку BTN_DOWN
        else
            flagDown = true; // включить кнопку BTN_DOWN
    }

    ui->BTN_DESELALL->setEnabled(!vSel->isEmpty());
    ui->BTN_UP->setEnabled(flagUp);
    ui->BTN_DOWN->setEnabled(flagDown);
    ui->BTN_DESELPAR->setEnabled(flagDeselPar);

    // TABLE_ALLPAR
    bool flagRight  = false;
    for(int i = 0; i < (int)vAll->size(); ++i) {
        assert(m_modelAll->rowCount() == vAll->size());
        QModelIndex idx = m_modelAll->index(i, 0);
        // bool state = m_modelAll->getCheckState(idx);
        bool selected = getSelectedStateParam(m_tableAll, idx);
        // если хотя бы один параметр отмечен или выбран
        if(/*state || */selected) {
            flagRight = true; // включить кнопку BTN_SELPAR
            break;
        }
    }
    ui->BTN_SELPAR->setEnabled(flagRight);
}

void DlgVarSel::setComboParamItemsEn() // сигнал от eventFilter
{   // подсветка пунктов комбобокса
    QStandardItemModel* comboModel = (QStandardItemModel*) ui->COMBOBOX_PARAM->model();
    if(m_VarSelGlobalVector.empty()) { // если таблица выбранных параметров пуста
        comboModel->item(ALL_PARAM  )->setEnabled(true);
        comboModel->item(ALL_ARRAY  )->setEnabled(true);
        comboModel->item(CHART_PARAM)->setEnabled(false);
        comboModel->item(REC_PARAM  )->setEnabled(false);
        emit signalBtnSelRecParamEn(false); // заблокируем кнопку выбора параметров для записи

    }
    else { // если таблица выбранных параметров заполнена
        comboModel->item(ALL_PARAM  )->setEnabled(true);
        comboModel->item(ALL_ARRAY  )->setEnabled(true);
        comboModel->item(CHART_PARAM)->setEnabled(true);
        comboModel->item(REC_PARAM  )->setEnabled(true);
        emit signalBtnSelRecParamEn(true); // разблокируем кнопку выбора параметров для записи
    }
    // qDebug()<<"setComboItemsEn() -> setButtonsEn()";
    setButtonsEn();
}

// =======================================================================
// === Фильтр ============================================================
// =======================================================================

void DlgVarSel::on_EDIT_FILTER_textEdited(const QString &arg1)
{
    m_regOPERATION &= ~MODE_FIND;
    DestSrc_t ds{};
    getDestSrcVectors(&ds);
    QVector<var_t>	*vAll = ds.vAll;
    if(vAll->empty())
        return;

    m_findVector.clear();
    QString sFind = arg1.toUpper();
    int cnt = (int)vAll->size();
    for(int i=0; i<cnt; ++i) {
        QString sName = (*vAll)[i].varID.varName.toUpper();
        int pos = sName.indexOf(sFind);
        if(pos > -1) {
            m_findVector.emplace_back((*vAll)[i]);
        }
    }
    if(!m_findVector.empty()) {
        m_regOPERATION |= MODE_FIND;
        ui->BTN_DESELALL->setEnabled(false);
        ui->BTN_CLR_FILTER->setEnabled(true); // разрешим кнопку очистки фильтра
        fillAllParamTable();
    }
}

void DlgVarSel::on_BTN_CLR_FILTER_clicked()
{
    filterClr();
    // fillAllParamTable();
}

void DlgVarSel::filterClr()
{
    if(m_regOPERATION & MODE_FIND) {
        m_regOPERATION &= ~MODE_FIND;
        ui->EDIT_FILTER->clear();
        ui->BTN_CLR_FILTER->setEnabled(false);
        ui->BTN_DESELALL->setEnabled(true);
        fillAllParamTable();
    }
}

bool DlgVarSel::getSelectedStateParam(QTableView *table, const QModelIndex &index)
{
    if(table == m_tableAll)
        return itemSelectionModelAll->isSelected(index);
    if(table == m_tableSel)
        return itemSelectionModelSel->isSelected(index);
    return false;
}

// ======================================================================
// === Кнопка окончания выбора параметров ===============================
// ======================================================================

void DlgVarSel::on_BTN_OK_clicked()
{   // заполнить рабочие таблицы во всех вкладках, в таблице массивов, в таблице графиков,
    // передать девайсу новые адреса параметров,
    // закрыть диалог выбора параметров
    // --------------------------------
    // обновим все окрытые вкладки рабочих таблиц
    for(int i = 0; i < m_mainWnd_4VarSel->m_dlgTableWorkVector.size(); ++i) {
        // удалим старые параметры
        m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_varSelVector.clear();
        // вставим новые параметры из временного вектора m_varSelVectorTmp
        for(int j = 0; j < m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_varSelVectorTmp.size(); ++j) {
            m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_varSelVector.emplace_back(
                m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_varSelVectorTmp[j]);
        }
        m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->m_tableAttr.newAttr = true;
        m_mainWnd_4VarSel->m_dlgTableWorkVector[i]->updateDataWorkTable();
    }

    // заполним вкладки рабочих таблиц
    m_mainWnd_4VarSel->m_dlgVarSel->m_ArrSelGlobalVector.clear();
    for(int i = 0; i < m_mainWnd_4VarSel->m_dlgVarSel->m_ArrSelGlobalVectorTmp.size(); ++i) {
        m_mainWnd_4VarSel->m_dlgVarSel->m_ArrSelGlobalVector.emplace_back(
            m_mainWnd_4VarSel->m_dlgVarSel->m_ArrSelGlobalVectorTmp[i]);
    }

    // заполним таблицу массивов в главном окне
    m_mainWnd_4VarSel->m_dlgTableArray->fillDlgTableArray();
    m_mainWnd_4VarSel->m_chartSelVector = m_mainWnd_4VarSel->m_dlgVarSel->m_chartSelVectorTmp;

    // для записи в файл заполним массив выбранными параметрами
    m_mainWnd_4VarSel->m_dlgVarSel->m_recSelVector.clear();
    for(int i = 0; i < m_mainWnd_4VarSel->m_dlgVarSel->m_recSelVectorTmp.size(); ++i) {
        m_mainWnd_4VarSel->m_dlgVarSel->m_recSelVector.emplace_back(
            m_mainWnd_4VarSel->m_dlgVarSel->m_recSelVectorTmp[i]);
    }

    // активность кнопок (в ui->MainWindow) для записи параметров в файл *.mon
    // (если вектор не пуст, соотв.кнопка активна)
    emit signalBtnStartRecEn(!m_recSelVector.empty()); // выбранные парам. для записи
    emit signalBtnSelRecParamEn(!m_VarSelGlobalVector.empty()); // выбранные парам., доступные для записи

    // отправим девайсу адреса параметров для сканирования
    emit signalSendSelParamToDevice();
    // толкнем переключатель данных каналов
    m_mainWnd_4VarSel->m_dlgIxxat->getDevId();
    delayMs(5);
    m_mainWnd_4VarSel->m_dlgIxxat->slotChannelChange(false);

    this->close();
}

