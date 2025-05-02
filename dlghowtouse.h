#ifndef DLGHOWTOUSE_H
#define DLGHOWTOUSE_H

#include <QDialog>

namespace Ui {
class DlgHowToUse;
}

class DlgHowToUse : public QDialog
{
    Q_OBJECT

public:
    explicit DlgHowToUse(QWidget *parent = nullptr);
    ~DlgHowToUse();

    void printing();

#define TITLE   QString("< Краткое руководство по использованию приложения \"QVarMon\" >")


private slots:
    void on_BTN_PRINT_clicked();

private:
    Ui::DlgHowToUse *ui;
};

#endif // DLGHOWTOUSE_H
