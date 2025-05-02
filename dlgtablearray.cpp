#include "dlgtablearray.h"
#include "ui_dlgtablearray.h"
// #include "common.h"
#include "mainwindow.h"

MainWindow  *m_mainWnd_4TableArray;

DlgTableArray::DlgTableArray(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTableArray)
{
    ui->setupUi(this);
    m_mainWnd_4TableArray = static_cast<MainWindow *>(parent);
    initDlgTableArray();
}

DlgTableArray::~DlgTableArray()
{
    delete ui;
}

void DlgTableArray::initDlgTableArray()
{
    ui->TABLE_ARRAY->clear();
    // имена колонок заголовка
    QString strHeader = "Имя,Узел,Тип,Размер";
    // Указываем число колонок
    ui->TABLE_ARRAY->setColumnCount(4);
    // Включаем сетку
    ui->TABLE_ARRAY->setShowGrid(true);
    // Устанавливаем заголовки колонок
    QStringList headers = strHeader.split(",");
    ui->TABLE_ARRAY->setHorizontalHeaderLabels(headers);
    // Разрешаем выделение построчно
    ui->TABLE_ARRAY->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Скрываем колонку заголовка строк
    ui->TABLE_ARRAY->verticalHeader()->hide();
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->TABLE_ARRAY->horizontalHeader()->setStretchLastSection(true);
    // Ограничим минимальную высоту строк
    ui->TABLE_ARRAY->verticalHeader()->setMinimumSectionSize(18);
    // установим фиксированную высоту заголовка
    ui->TABLE_ARRAY->horizontalHeader()->setFixedHeight(19);
    // цвет заголовков QTableWidget, цвет выделенной строки и цвет фонта
    ui->TABLE_ARRAY->setStyleSheet(QString("%1 %2").arg(myHeaderStyle, mySelectedStyle));
    // установим автоподстройку ширины заголовков колонок под размер записей списка
    ui->TABLE_ARRAY->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->TABLE_ARRAY, &QTableWidget::itemClicked,
            this, &DlgTableArray::slotItemClicked);
}

void DlgTableArray::fillDlgTableArray()
{
    QVector<var_t> *vectorTmp = &m_mainWnd_4TableArray->m_dlgVarSel->m_ArrSelGlobalVector;
    int cntParam = (int)vectorTmp->size();
    if(cntParam)
        emit signalCreateArrayApp();

    ui->TABLE_ARRAY->setRowCount(cntParam); // Указываем число новых строк

    for(int i=0; i<cntParam; ++i) {
        ui->TABLE_ARRAY->setRowHeight(i, 18); // высота строки

        QTableWidgetItem *itemName = new QTableWidgetItem();
        itemName->setData(Qt::BackgroundRole, myItemsColor(i));
        itemName->setData(Qt::DisplayRole, (*vectorTmp)[i].varID.varName);
        ui->TABLE_ARRAY->setItem(i, NAME_SEL, itemName);

        QTableWidgetItem *itemUnit = new QTableWidgetItem();
        itemUnit->setData(Qt::BackgroundRole, myItemsColor(i));
        itemUnit->setData(Qt::DisplayRole, (*vectorTmp)[i].varID.unitName);
        ui->TABLE_ARRAY->setItem(i, UNIT_SEL, itemUnit);

        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemType->setData(Qt::BackgroundRole, myItemsColor(i));
        itemType->setData(Qt::DisplayRole, (*vectorTmp)[i].varType.altType);
        ui->TABLE_ARRAY->setItem(i, TYPE_SEL, itemType);

        QTableWidgetItem *itemArray = new QTableWidgetItem();
        QString strArr = QString("%1x%2")
                        .arg((*vectorTmp)[i].varProp.arrRow).arg((*vectorTmp)[i].varProp.arrCol);
        itemArray->setData(Qt::BackgroundRole, myItemsColor(i));
        itemArray->setData(Qt::DisplayRole, strArr);
        ui->TABLE_ARRAY->setItem(i, ARR_SEL, itemArray);
    }
}

void DlgTableArray::slotItemClicked(QTableWidgetItem *item)
{   // вызывает открытие окна, соответствующее указанному массиву
    QRect rect = ui->TABLE_ARRAY->visualItemRect(
                ui->TABLE_ARRAY->item(item->row(), item->column()));
    // точка расположения итема массива в списке в глобальных координатах экрана
    QPoint pnt = this->mapToGlobal(QPoint(rect.right(), rect.bottom()));
    // позиция в общем списке выбранных массивов в листе SEL
    uint32_t pos = item->row();
    // структура начальной ячейки массива
    var_t var = m_mainWnd_4TableArray->m_dlgVarSel->m_ArrSelGlobalVector[pos];
    m_mainWnd_4TableArray->startDlgArray(var, &pnt);
}





















