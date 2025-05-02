#ifndef DLGVARADDR_H
#define DLGVARADDR_H

// #include "common.h"
#include <QDialog>

namespace Ui {
class DlgVarAddr;
}

class DlgVarAddr : public QDialog
{
    Q_OBJECT

public:
    explicit DlgVarAddr(QWidget *parent = nullptr);
    ~DlgVarAddr();

    void showDlgAddr(QString  &varName, uint32_t varAddr, uint32_t unitID, QString  &unitName);

private slots:
    void on_BTN_CLOSE_clicked();

private:
    Ui::DlgVarAddr *ui;
};

#endif // DLGVARADDR_H
