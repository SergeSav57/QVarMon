#ifndef DLGUART_H
#define DLGUART_H

#include <QDialog>

namespace Ui {
class DlgUART;
}

class DlgUART : public QDialog
{
    Q_OBJECT

public:
    explicit DlgUART(QWidget *parent = nullptr);
    ~DlgUART();

private:
    Ui::DlgUART *ui;
};

#endif // DLGUART_H
