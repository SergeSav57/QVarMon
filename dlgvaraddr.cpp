#include "dlgvaraddr.h"
#include "ui_dlgvaraddr.h"

DlgVarAddr::DlgVarAddr(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgVarAddr)
{
    ui->setupUi(this);
    // этот аттрибут гарантирует, что диалоговое окно будет удаляться всякий раз, когда оно закрывается.
    setAttribute(Qt::WA_DeleteOnClose, true);
}

DlgVarAddr::~DlgVarAddr()
{
    delete ui;
}

void DlgVarAddr::showDlgAddr(QString  &varName, uint32_t varAddr, uint32_t unitID, QString  &unitName)
{
    QFont fnt("Arial", 10, QFont::Bold);
    // fnt.setUnderline(true);
    QFontMetricsF fm(fnt);
    const qreal width = fm.horizontalAdvance(varName);
    QRect rect = this->geometry();
    if((width + 50) > rect.width()) {
        rect.setWidth(width + 50);
        this->setGeometry(rect);
    }
    ui->labelVarName->setFont(fnt);
    ui->labelVarName->setText(QString("[%1:%2] %3").arg(unitID).arg(unitName, varName));
    ui->labelVarAddr->setText(QString::number(varAddr, 16));
    this->show();
}

void DlgVarAddr::on_BTN_CLOSE_clicked()
{
    close();
}
