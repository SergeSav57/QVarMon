#include "dlgaddaddress.h"
#include "ui_dlgaddaddress.h"
#include "common.h"
#include "mainwindow.h"

MainWindow  *m_mainWnd_4AddAddress{};

DlgAddAddress::DlgAddAddress(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgAddAddress)
{
    ui->setupUi(this);
    m_mainWnd_4AddAddress = static_cast<MainWindow *>(parent);

    m_types[0].typeID = TYPE_INT8;
    m_types[1].typeID = TYPE_UINT8;
    m_types[2].typeID = TYPE_INT16;
    m_types[3].typeID = TYPE_UINT16;
    m_types[4].typeID = TYPE_INT32;
    m_types[5].typeID = TYPE_UINT32;
    m_types[6].typeID = TYPE_FLOAT;
    m_types[0].typeName = "int8";
    m_types[1].typeName = "uint8";
    m_types[2].typeName = "int16";
    m_types[3].typeName = "uint16";
    m_types[4].typeName = "int32";
    m_types[5].typeName = "uint32";
    m_types[6].typeName = "float";
    m_types[0].typeSize = 1;
    m_types[1].typeSize = 1;
    m_types[2].typeSize = 2;
    m_types[3].typeSize = 2;
    m_types[4].typeSize = 4;
    m_types[5].typeSize = 4;
    m_types[6].typeSize = 4;
    for(int i = 0; i < 7; ++i)
        ui->comboBox_Type->addItem(m_types[i].typeName);

}
void DlgAddAddress::showDlg()
{
    m_unitsVec.clear();
    units_t unit{};
    for(int i = 0; i < NUMCHANNELMAX; ++i) {
        if(m_mainWnd_4AddAddress->m_unitProp.property[i].isPresent) {
            unit.unitName = m_mainWnd_4AddAddress->m_unitProp.property[i].unitName;
            unit.unitID = m_mainWnd_4AddAddress->m_unitProp.property[i].unitID;
            m_unitsVec.emplace_back(unit);
            ui->comboBox_Unit->addItem(unit.unitName);
        }
    }
    this->show();
}

DlgAddAddress::~DlgAddAddress()
{
    delete ui;
}

// void DlgAddAddress::on_comboBox_Type_activated(int index)
// {

// }

void DlgAddAddress::on_BTN_FINISH_clicked()
{
    uint32_t addr = ui->lineEdit_Address->text().toUInt(nullptr, 16);
    if(!addr) {
        showSelfClosedMessageBox(2000, "Введите корректный адрес переменной");
        return;
    }
    QString varName = ui->lineEdit_Name->text();
    if(varName.isEmpty()) {
        showSelfClosedMessageBox(2000, "Введите имя переменной");
        return;
    }
    int pos = ui->comboBox_Unit->currentIndex();
    QString unitName = m_unitsVec[pos].unitName;
    int unitID = m_unitsVec[pos].unitID;
    pos = ui->comboBox_Type->currentIndex();
    QString varType = m_types[pos].typeName;
    int varTypeID = m_types[pos].typeID;
    int varTypeSz = m_types[pos].typeSize;

    var_t var{};
    var.varProp.varAddr = addr;
    var.varID.varName = varName;
    var.varID.varID = m_varNumber + unitID * 1000 + (1 << BIT_VAR_EX);
    var.varID.unitName = unitName;
    var.varID.unitID = unitID;
    var.varType.altType = varType;
    var.varType.typeID = varTypeID;
    var.varType.typeSize = varTypeSz;

    int curTab = m_mainWnd_4AddAddress->currentTabWork();
    m_mainWnd_4AddAddress->m_dlgVarSel->m_VarAllGlobalVector.emplace_back(var);
    m_mainWnd_4AddAddress->m_dlgVarSel->m_VarSelGlobalVector.emplace_back(var);
    m_mainWnd_4AddAddress->m_dlgTableWorkVector[curTab]->m_varSelVectorTmp.emplace_back(var);
    m_mainWnd_4AddAddress->m_dlgVarSel->on_BTN_OK_clicked();

    close();
}

