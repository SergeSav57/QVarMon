#ifndef TBLSELMODEL_H
#define TBLSELMODEL_H

#include <QTableView>
#include <QAbstractTableModel>
#include <QHeaderView>

class TableSelModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableSelModel(int row, int col, QTableView *table, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int nRole = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void beginReset();
    void endReset();
    void setRowCol(int row, int col);
    void setCheckState(const QModelIndex &index, bool state);
    bool getCheckState(const QModelIndex &index);

    QHash<QModelIndex, QVariant> m_hashData;
    QHash<int, bool> m_hashCheck;

private:
    int m_Row;
    int m_Column;
    QTableView *m_table;
};

#endif // TBLSELMODEL_H
