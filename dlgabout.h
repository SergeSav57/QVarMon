#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QDialog>

namespace Ui {
class DlgAbout;
}

class DlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAbout(QWidget *parent = nullptr);
    ~DlgAbout();

#define ABOUT_STRING_VER    ("Версия QVarMon : v1.0.0")
#define ABOUT_STRING_TXT    ("  Приложение предназначено для работы с микроконтроллерами\n" \
                             "семейства STM32F40x и STM32F41x производства STMicroelectronics\n" \
                             "и совместимыми с ними от других производителей, например:\n" \
                             "GD32 производства GigaDevice.")

private slots:
    void on_pushButton_clicked();
    void on_pushButton_Help_clicked();

private:
    Ui::DlgAbout *ui;
};

#endif // DLGABOUT_H
