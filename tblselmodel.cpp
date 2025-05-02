#include "tblselmodel.h"
#include "common.h"
#include "dlgvarsel.h"

DlgVarSel *m_parentDlg{};

TableSelModel::TableSelModel(int row, int col, QTableView *table, QObject *parent)
    : QAbstractTableModel(parent),
    m_Row(row),
    m_Column(col),
    m_table(table)
{
    m_parentDlg = static_cast<DlgVarSel *>(parent);
}

void TableSelModel::beginReset()
{
    beginResetModel();
}

void TableSelModel::endReset()
{
    endResetModel();
}

void TableSelModel::setRowCol(int row, int col)
{
    beginResetModel();
    m_Row = row;
    m_Column = col;
    endResetModel();
}

QVariant TableSelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case NAME_SEL:
                return "Имя";
            case UNIT_SEL:
                return "Узел";
            case TYPE_SEL:
                return "Тип";
            case ARR_SEL:
                return "Размер";
            }
        }
        if(orientation == Qt::Vertical)
            return section + 1; // номер строки
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

int TableSelModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_Row;
}

int TableSelModel::columnCount(const QModelIndex &/*parent*/) const
{
    return m_Column;
}

QVariant TableSelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if(m_hashData[index].isValid())
            return m_hashData[index];
        return QVariant(); // если индекс вышел за рамки допустимых для этой таблицы
    case Qt::BackgroundRole: // (памятка! закраска идет по каждой ячейке таблицы)
        // закраска фоном каждой не четной строки (заложено в макросе myItemsColor(n))
        return myItemsColor(index.row());
    case Qt::CheckStateRole:
        if(m_table == m_parentDlg->m_tableAll) {
            if(index.column() == NAME_SEL) {
                if(m_hashCheck[index.row()] == true)
                    return  Qt::Checked;
                else
                    return  Qt::Unchecked;
            }
            return QVariant();
        }
        else
            return QVariant();
    case Qt::TextAlignmentRole:
        switch(index.column()) {
        case NAME_SEL:
        case UNIT_SEL:
        case TYPE_SEL:
            return Qt::AlignLeft;
        default:
            return Qt::AlignLeft;
        }
        return Qt::AlignLeft;
    }
    return QVariant();
}

bool TableSelModel::setData(const QModelIndex &index,
                             const QVariant &value, int role)
{   // служит для изменения данных
    if (!index.isValid())
        return false;
    switch(role) {
    case Qt::DisplayRole:
        m_hashData[index] = value;
        emit dataChanged(index, index);
        return true;
    case Qt::CheckStateRole:
        if(m_table == m_parentDlg->m_tableAll) {
            if(value == Qt::Checked) {
                m_hashCheck[index.row()] = true;
                return true;
            }
            else {
                m_hashCheck[index.row()] = false;
                return true;
            }
        }
        return true;
    case Qt::EditRole:
        return true;
    case Qt::UserRole:
        return true;
    }
    return false;
}

Qt::ItemFlags TableSelModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if(index.isValid()) {
        if(index.column()  == 0) {
            flags |= Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable;
        }
    }
    return flags;
}

void TableSelModel::setCheckState(const QModelIndex &index, bool state)
{
    // beginResetModel();
    if(state)
        m_hashCheck[index.row()] = true;
    else
        m_hashCheck[index.row()] = false;
    emit dataChanged(index, index);
    // endResetModel();

}

bool TableSelModel::getCheckState(const QModelIndex &index)
{
    return m_hashCheck[index.row()];
}
