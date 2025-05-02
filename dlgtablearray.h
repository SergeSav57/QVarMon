#ifndef DLGTABLEARRAY_H
#define DLGTABLEARRAY_H

#include <QDialog>
#include <QTableWidgetItem>
#include "common.h"

namespace Ui {
class DlgTableArray;
}

class DlgTableArray : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTableArray(QWidget *parent = nullptr);
    ~DlgTableArray();

public slots:
    void slotItemClicked(QTableWidgetItem *item);

signals:
    void signalCreateArrayApp();
    void signalCreateArrayDlg(var_t&, QPoint*);

public:
    void initDlgTableArray();
    void fillDlgTableArray();

private:
    Ui::DlgTableArray *ui;
};

#endif // DLGTABLEARRAY_H
