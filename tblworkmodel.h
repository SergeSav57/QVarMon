#ifndef TBLWORKMODEL_H
#define TBLWORKMODEL_H

#include <QTableView>
#include <QAbstractTableModel>
#include <QHeaderView>

class TableWorkModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableWorkModel(int row, int col, QObject *parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int nRole = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setRowCol(int row, int col);
    void beginReset();
    void endReset();

    QHash<QModelIndex, QVariant> m_hashData;

    bool    m_edit = false;

    QModelIndex     m_indexNotUsed; // индекс начала не имеющей строк части таблицы
                                    // (она появляется при числе блоков таблицы больше 1
                                    // и числе переменных меньше, чем заполнение всего
                                    // последнего блока)
    void setIndexNotUsed(QModelIndex index);

signals:
    void signalSetSys(int, QModelIndex &index);

private:
    int m_Row;
    int m_Column;
    QTableView *m_table;
};

#endif // TBLWORKMODEL_H
