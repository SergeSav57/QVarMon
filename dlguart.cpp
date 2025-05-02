#include "dlguart.h"
#include "ui_dlguart.h"

DlgUART::DlgUART(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgUART)
{
    ui->setupUi(this);
}

DlgUART::~DlgUART()
{
    delete ui;
}
