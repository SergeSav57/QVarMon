#ifndef DLGADDADDRESS_H
#define DLGADDADDRESS_H

#include <QDialog>

namespace Ui {
class DlgAddAddress;
}

class DlgAddAddress : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAddAddress(QWidget *parent = nullptr);
    ~DlgAddAddress();

    typedef struct {
        QString  typeName;
        uint32_t typeID;
        uint32_t typeSize;
    } types_t;
    types_t     m_types[8];

    typedef struct {
        QString  unitName;
        uint32_t unitID;
    } units_t;
    QVector<units_t>     m_unitsVec{};

    int     m_varNumber = 0; // текущий номер адреса добавленной переменной

    void showDlg();

private slots:
    // void on_comboBox_Type_activated(int index);
    void on_BTN_FINISH_clicked();

private:
    Ui::DlgAddAddress *ui;
};

#endif // DLGADDADDRESS_H
