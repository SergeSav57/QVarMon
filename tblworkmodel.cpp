#include "tblworkmodel.h"
#include "common.h"

TableWorkModel::TableWorkModel(int row, int col, QObject *parent)
    : QAbstractTableModel(parent),
    m_Row(row),
    m_Column(col)
{
}

void TableWorkModel::beginReset()
{
    beginResetModel();
}

void TableWorkModel::endReset()
{
    endResetModel();
}

void TableWorkModel::setIndexNotUsed(QModelIndex index)
{
    m_indexNotUsed = index;
}

void TableWorkModel::setRowCol(int row, int col)
{
    beginResetModel();
    m_Row = row;
    m_Column = col;
    endResetModel();
}

int TableWorkModel::rowCount(const QModelIndex &/*parent*/) const
{   // возвращает количество строк в модели
    return m_Row;
}

int TableWorkModel::columnCount(const QModelIndex &/*parent*/) const
{   // возвращает количество колонок в модели
    return m_Column;
}

QVariant TableWorkModel::data(const QModelIndex &index, int role) const
{   // возвращает или изменяет элемент данных, соответствующий указанному модельному индексу
    if(!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if(m_hashData[index].isValid())
            return m_hashData[index];
        return QVariant(); // если индекс вышел за рамки допустимых для этой таблицы
    case Qt::BackgroundRole: // (памятка! закраска идет по каждой ячейке таблицы)
        if(index.column() % MAX_COLUMN == 0) // колонка нумерации параметров чуть зеленоватая
            return QColor(220, 240, 230);
        else // закраска фоном каждой не четной строки (заложено в макросе myItemsColor(n))
            return myItemsColor(index.row());
    case Qt::TextAlignmentRole:
        switch(index.column() % MAX_COLUMN) {
        case NUM_EL:
        case SYS_EL:
            return Qt::AlignCenter;
        default:
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

bool TableWorkModel::setData(const QModelIndex &index,
                               const QVariant &value, int role)
{   // служит для изменения данных
    if (!index.isValid())
        return false;
    switch(role) {
    case Qt::DisplayRole:
        m_hashData[index] = value;
        emit dataChanged(index, index);
        return true;
    case Qt::EditRole: //:=> сода попадаем по цепочке:
        //:=> DlgTableWork::slotItemClicked():ui->TABLE_WORK->edit() =>
        //:=> ComboBoxDelegate::createEditor() => ComboBoxDelegate::setModelData()
        if((index.column() - SYS_EL) % MAX_COLUMN == 0) {
            int pos = value.toInt();
            emit signalSetSys(pos,  (QModelIndex &)index);
        }
        return true;
    case Qt::UserRole: { // мои данные для отладки
        // int val = value.toInt();
        // if(val == 321)
        //     m_block = true;
        // if(val == 123)
        //     m_block = false;
        return true;
        }
    }
    return false;
}

/* == памятка ==
#define NUM_EL			0
#define UNIT_EL			1
#define NAME_EL			2
#define VAL_EL			3
#define SYS_EL			4
#define TYPE_EL			5
#define MAX_COLUMN		6 // число колонок таблицы (для построения дополнительных таблиц справа)
 */

QVariant TableWorkModel::headerData(int section, Qt::Orientation orientation, int role) const
{   // предоставляет таблице что-то для отображения в своих заголовках
    switch (role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Horizontal) {
            int pos = section % MAX_COLUMN;
            switch(pos) {
            case NUM_EL:
                return "N";
            case UNIT_EL:
                return "Узел";
            case NAME_EL:
                return "Имя";
            case VAL_EL:
                return "Значение";
            case SYS_EL:
                return "S";
            case TYPE_EL:
                return "Тип";
            }
        }
        // if(orientation == Qt::Vertical) // не используем
        //     return section; // номер строки (задается в setData())
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

Qt::ItemFlags TableWorkModel::flags(const QModelIndex &index) const
{   //  определяет свойства элементов, в частности, возможность их редактирования
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if( (m_indexNotUsed.row() < index.row()) &&
        (m_indexNotUsed.column() < index.column()))
    {   // при КЛМ на пустой части таблицы, ячейки не редактируются
        return flags;
    }
    if(index.isValid()) {
        if( ((index.column() - SYS_EL) % MAX_COLUMN == 0) ||
            ((index.column() - VAL_EL) % MAX_COLUMN == 0) )
        {
            flags |= Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }
    }
    return flags;
}
