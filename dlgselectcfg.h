#ifndef DLGSELECTCFG_H
#define DLGSELECTCFG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "dlgconfig.h"
#include "common.h"

namespace Ui {
class DlgSelectCfg;
}

class DlgSelectCfg : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSelectCfg(QWidget *parent = nullptr);
    ~DlgSelectCfg();

#define DELETE_ON 0

    DlgConfig           *m_dlgConfig;

    QStringList         m_listPathCfgFiles{};
    QStringList         m_listNameCfgFiles{};
    QStringList         m_listCfgNames{};
    QStringList         m_listCfgFlash{};
    QStringList         m_listCfgFile{};
    QStringList         m_listCfgEditors{};
    QList<QStringList>  m_listUnitsName{};
    QList<QStringList>  m_listUnitsID{};
    QList<QStringList>  m_listFileName{};


    void findCfgFiles();
    void readXmlFile(QString path);
#if DELETE_ON
    void DeleteItem (QTreeWidget *treeWidget, QTreeWidgetItem *currentItem); //удаление элемента из QTreeWidget
#endif
    QTreeWidgetItem *setTopItem(QTreeWidget *treeWidget, int index, QString text, QString prop,
                                QString icon = nullptr, bool expanded = false);
    QTreeWidgetItem *setChildItem(QTreeWidgetItem *parent_item, QString text, QString prop,
                                  QString icon = nullptr, bool expanded = false);
    void on_start();

signals:
    void signalStartMainWnd(uint32_t startType, unitProp_t &prop);

private slots:
    void on_BTN_NEWCONF_clicked();
    void on_BTN_START_CFG_clicked();
#if DELETE_ON
    void on_BTN_DELETE_clicked();
#endif
    void on_treeWidget_Name_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_Name_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::DlgSelectCfg *ui;
    void closeEvent(QCloseEvent * e) override; // закрытие окна
    QTreeWidgetItem *m_item4Start = nullptr;   // текущий элемент для старта конфигурации/редактора
#if DELETE_ON
    QTreeWidgetItem *m_item4Delete = nullptr;  // текущий элемент для удаления, запоминается при клике в QTreeWidget
#endif
};

#endif // DLGSELECTCFG_H
