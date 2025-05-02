#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QComboBox>
#include <QModelIndex>
#include <QTableView>
//: Базовые классы для разработки собственных делегатов:
//: QItemDelegate – предоставляет базовые возможности для редактирования элемента;
//: QStyledItemDelegate – отличается тем, что позволяет пользовательскую настройку
//:                       стилей, поэтому, предпочтительней при нестандартных
//:                       визуализациях редактора.

class myComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit myComboBoxDelegate(QObject *parent = nullptr);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const override;

public slots:
    void slotSetModelText(const QString &text);

private:

};

#endif // COMBOBOXDELEGATE_H
