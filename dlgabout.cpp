#include "dlgabout.h"
#include "ui_dlgabout.h"
#include "mainwindow.h"

MainWindow  *m_mainWnd_4DlgAbout;

DlgAbout::DlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAbout)
{
    ui->setupUi(this);
    m_mainWnd_4DlgAbout = static_cast<MainWindow *>(parent);

    ui->label_str_ver->setText(ABOUT_STRING_VER);
    static const QStringList LIST_ITEMS = QStringList() << ABOUT_STRING_TXT;
    ui->listWidget->addItems(LIST_ITEMS);
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
}

DlgAbout::~DlgAbout()
{
    delete ui;
}

void DlgAbout::on_pushButton_clicked()
{
    this->close();
}

void DlgAbout::on_pushButton_Help_clicked()
{
    this->close();
    m_mainWnd_4DlgAbout->m_dlgHowToUse->show();
}

