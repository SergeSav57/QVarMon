#include "dlgselectcfg.h"
#include "ui_dlgselectcfg.h"
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include "mainwindow.h"

MainWindow  *m_mainWnd_4DlgSelCfg;

DlgSelectCfg::DlgSelectCfg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgSelectCfg)
{
    ui->setupUi(this);
    m_mainWnd_4DlgSelCfg = static_cast<MainWindow *>(parent);

    m_dlgConfig = new DlgConfig(this);

    // затемним немного сплиттер
    ui->splitter->setStyleSheet("QSplitter::handle{background:lightgray;}");

    ui->treeWidget_Name->setColumnCount(1);
    QStringList headers1{"Имя файла / Название версии"};
    ui->treeWidget_Name->setHeaderLabels(headers1);
    ui->treeWidget_Prop->setColumnCount(2);
    QStringList headers2{"Свойство","Значение"};
    ui->treeWidget_Prop->setHeaderLabels(headers2);
    ui->treeWidget_Prop->header()->resizeSection(0, 150);

#if DELETE_ON
    ui->BTN_DELETE->setVisible(true);
#else
    ui->BTN_DELETE->setVisible(false);
#endif

    // найдем все файлы конфигураций
    findCfgFiles();

    connect(this, &DlgSelectCfg::signalStartMainWnd,
            m_mainWnd_4DlgSelCfg, &MainWindow::slotStartMainWnd);
}

DlgSelectCfg::~DlgSelectCfg()
{
    delete ui;
}

void DlgSelectCfg::readXmlFile(QString path)
{
    m_dlgConfig->xmlReadFile(path);
}

void DlgSelectCfg::closeEvent(QCloseEvent * e)
{
    Q_UNUSED(e);
    unitProp_t prop{}; // как заглушка
    emit signalStartMainWnd(START_NOLOAD, prop);
}

void DlgSelectCfg::findCfgFiles()
{
    QString CurrentDir = QDir::currentPath() + "/config/";
    QDir dirPath(CurrentDir);
    if(!dirPath.exists()) //проверяем наличие директории
        dirPath.mkdir(CurrentDir);
    QStringList files = QDir(CurrentDir).entryList(QStringList("conf_*.xml"), QDir::Files);
    QString strErr = "Файлы конфигураций отсутствуют.\n"
                     "Проверьте: файлы должны находиться\n"
                     "в директории '../""config/""'\n"
                     "и начинаться с префикса 'conf_'.\n"
                     "Или нажмите кнопку 'Создать' для\n"
                     "создания новой конфигурации загрузки.";
    if(files.isEmpty()) {
        QMessageBox::information(nullptr, "", strErr, QMessageBox::Ok);
        return;
    }

    foreach (const QFileInfo &fi, dirPath.entryInfoList(QStringList("conf_*.xml"))) {
        m_listPathCfgFiles << fi.absoluteFilePath();
        m_listNameCfgFiles << fi.fileName();
    }

    for(int i = 0; i < files.size(); ++i) {
        readXmlFile(m_listPathCfgFiles[i]);
        m_listCfgNames   << m_dlgConfig->m_cfgName;
        m_listCfgFlash   << STR_LOADFLASH;
        m_listCfgFile    << STR_LOADFILE;
        m_listCfgEditors << STR_VEREDITOR;
        m_listUnitsName  << m_dlgConfig->m_cfgProp.unitName;
        // m_listUnitsName.append(m_dlgConfig->m_cfgProp[i].Name);
        m_listUnitsID    << m_dlgConfig->m_cfgProp.ID;
        m_listFileName   << m_dlgConfig->m_cfgProp.varFileName;
    }

    setTopItem(ui->treeWidget_Name, 0, STR_CREATEVARFILE, "", CREATVAR_ICON, false);
    // директория, содержащая файлы конфигурации
    QTreeWidgetItem* nameTopItem = setTopItem(ui->treeWidget_Name, 1, CurrentDir, "", nullptr, true);
    for(int i = 0; i < m_listNameCfgFiles.size(); ++i) {
        // имена файлов конфигураций
        QTreeWidgetItem* child_nameFile = setChildItem(nameTopItem, m_listNameCfgFiles[i], "", XMLSOURCE_ICON, true);
        // имена конфигураций
        QTreeWidgetItem* child_cfgName = setChildItem(child_nameFile, m_listCfgNames[i], "", DCHIP_ICON, true);
        // типы загрузки
        setChildItem(child_cfgName, m_listCfgFlash[i], "", DCHIP_ICON);
        setChildItem(child_cfgName, m_listCfgFile[i], "", OPEN_ICON);
        // имена редакторов конфигурации
        setChildItem(child_nameFile, m_listCfgEditors[i], "", ENGINEERING_ICON);
    }
}

QTreeWidgetItem *DlgSelectCfg::setTopItem(QTreeWidget *treeWidget, int index,
                                          QString text, QString prop,
                                          QString icon, bool expanded)
{
    QTreeWidgetItem* top_item = new QTreeWidgetItem;
    top_item->setText(0, text);
    top_item->setText(1, prop); // строка свойств
    top_item->setIcon(0, QIcon(icon));
    treeWidget->insertTopLevelItem(index, top_item);
    // treeWidget->addTopLevelItem(top_item); // тоже можно применять, если дерево пусто
    top_item->setExpanded(expanded); // разворачиваем итем
    return top_item;
}

QTreeWidgetItem *DlgSelectCfg::setChildItem(QTreeWidgetItem *parent_item,
                                            QString text, QString prop,
                                            QString icon, bool expanded)
{
    QTreeWidgetItem* child_item = new QTreeWidgetItem;
    child_item->setText (0, text);
    child_item->setText(1, prop);
    child_item->setIcon(0, QIcon(icon));
    parent_item->addChild(child_item);
    child_item->setExpanded(expanded);
    return child_item;
}

void DlgSelectCfg::on_treeWidget_Name_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    m_item4Start = item; // итем для старта конфигурации/редактора
#if DELETE_ON
    m_item4Delete = item; // итем для удаления
#endif

    QString str = item->text(0);
    if(str == STR_CREATEVARFILE) {
        ui->treeWidget_Prop->clear();
        setTopItem(ui->treeWidget_Prop, 0, "Создание файла\n параметров из\n файла *.elf",
                   "", nullptr, true);
        return;
    }
    // перечитаем файл на случай его изменения в редакторе версии
    for(int i = 0; i < m_listCfgNames.size(); ++i) {
        if(m_listCfgNames[i] == str) {
            readXmlFile(m_listPathCfgFiles[i]);
            m_listCfgNames[i]   = m_dlgConfig->m_cfgName;
            m_listUnitsName[i]  = m_dlgConfig->m_cfgProp.unitName;
            m_listUnitsID[i]    = m_dlgConfig->m_cfgProp.ID;
            m_listFileName[i]   = m_dlgConfig->m_cfgProp.varFileName;
            break;
        }
    }

    ui->treeWidget_Prop->clear();
    if(str.contains("/config/")) { // все файлы конфигурации должны располагаться в каталоге "config"
        setTopItem(ui->treeWidget_Prop, 0, "Каталог файлов конфигурации",
                   str, nullptr, true);
        return;
    }

    QString sCfg = "Конфигурация";
    QString sUnits = "Каналы(Узлы)/ID";

    QTreeWidgetItem* propTopItem;
    QTreeWidgetItem* unitsTopItem;
    QString parentStr = item->parent()->text(0);
    QString childStr = item->parent()->child(0)->text(0);
    for(int i = 0; i < m_listCfgNames.size(); ++i) {
        // Файл конфигурации
        if(m_listNameCfgFiles[i] == str) {
            setTopItem(ui->treeWidget_Prop, 0, STR_FILECFGXML,
                       m_listNameCfgFiles[i], nullptr, true);
        } else
        if(m_listCfgNames[i] == str) {
            propTopItem = setTopItem(ui->treeWidget_Prop, 0, sCfg, m_listCfgNames[i], nullptr, true);
            unitsTopItem = setChildItem(propTopItem, sUnits, "", nullptr, true);
            for(int j = 0; j < m_listUnitsName[i].size(); ++j) {
                QString name = m_listUnitsName[i][j];
                QString sID = m_listUnitsID[i][j];
                setChildItem(unitsTopItem, name, sID);
            }
        } else
        // Загрузка из флеш памяти
        if((m_listCfgFlash[i] == str) && (m_listCfgNames[i] == parentStr)) {
            propTopItem = setTopItem(ui->treeWidget_Prop, 0, STR_LOADFLASH,
                       m_listCfgNames[i], nullptr, true);
            unitsTopItem = setChildItem(propTopItem, sUnits, "", nullptr, true);
            for(int j = 0; j < m_listUnitsName[i].size(); ++j) {
                QString name = m_listUnitsName[i][j];
                QString sID = m_listUnitsID[i][j];
                setChildItem(unitsTopItem, name, sID);
            }
        } else
        // Загрузка из файла
        if((m_listCfgFile[i] == str) && (m_listCfgNames[i] == parentStr)) {
            propTopItem = setTopItem(ui->treeWidget_Prop, 0, STR_LOADFILE,
                       m_listCfgNames[i], nullptr, true);
            unitsTopItem = setChildItem(propTopItem, sUnits, "", nullptr, true);
            for(int j = 0; j < m_listUnitsName[i].size(); ++j) {
                QString name = m_listUnitsName[i][j];
                QString fileName = m_listFileName[i][j];
                QString sID = m_listUnitsID[i][j] + "  [ " + fileName + " ]";
                setChildItem(unitsTopItem, name, sID);
            }
        } else
        // Редактор версии
        if((m_listCfgEditors[i] == str) && (m_listCfgNames[i] == childStr)) {
            setTopItem(ui->treeWidget_Prop, 0, STR_VEREDITOR,
                       m_listCfgNames[i], nullptr, true);
        }
    }
}

void DlgSelectCfg::on_treeWidget_Name_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item);
    Q_UNUSED(column);
    on_start();
}

void DlgSelectCfg::on_BTN_START_CFG_clicked()
{
    on_start();
}

void DlgSelectCfg::on_start()
{
    QString str = m_item4Start->text(0);
    if(str == STR_CREATEVARFILE) {
        // m_mainWnd_4DlgSelCfg->m_dlgCreateVarFile->show();
        QString  cmd{};
#ifdef QT_DEBUG
        cmd = "d:\\WORKSPACE_GD\\Qt_proj\\build-QVarFile-Qt_6_7_2_MinGW_64_bit\\debug\\QVarFile.exe";
#else
        cmd = "QVarFile.exe";
#endif
        QProcess *proc = new QProcess(this);
        if(!proc->startDetached(cmd)) {
            delete proc;
            qDebug()<<"Do not start DlgChart!";
            return;
        }
        proc->waitForStarted(200);
        proc->deleteLater();
        return;
    }
    QString parentStr = m_item4Start->parent()->text(0);
    for(int i = 0; i < m_listNameCfgFiles.size(); ++i) {
        if(m_listCfgNames[i] == parentStr) {
            readXmlFile(m_listPathCfgFiles[i]);
            // для выбранной конфигурации: передача имен каналов для светиков в MainWondow
            m_mainWnd_4DlgSelCfg->m_unitProp.sCfgName = m_dlgConfig->m_cfgName;
            // при смене конфигурации загрузки обнулим флаги isOnlyView всех каналов
            for(int j = 0; j < NUMCHANNELMAX; ++j)
                m_mainWnd_4DlgSelCfg->m_unitProp.property[j].isOnlyView = false;
            // определим какие каналы рабочие, какие только для отображения, но не работают
            // и какие не представлены ни в конфигурации, ни подключены
            for(int k =  0; k < m_dlgConfig->m_cfgProp.ID.size(); ++k) {
                for(int j = 0; j < NUMCHANNELMAX; ++j) {
                    int id = m_dlgConfig->m_cfgProp.ID[k].toInt();
                    if((id == j) && (m_mainWnd_4DlgSelCfg->m_unitProp.property[j].isPresent)) {
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].unitID = id;
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].ledsChVec.lbStringCh = QString("[%1]").arg(id);
                        QString name = m_dlgConfig->m_cfgProp.unitName[k];
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].unitName = name;
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].ledsChVec.lbStringName = name;
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].unitVarFile = m_dlgConfig->m_cfgProp.varFilePath[k];
                        break;
                    }
                    else if(id == j) {
                        m_mainWnd_4DlgSelCfg->m_unitProp.property[j].isOnlyView = true;
                    }
                }
            }
            m_mainWnd_4DlgSelCfg->m_unitProp.numChannels = m_dlgConfig->m_unitsNum;
            m_mainWnd_4DlgSelCfg->m_unitProp.listVarFilePath = m_dlgConfig->m_cfgProp.varFilePath;
            m_mainWnd_4DlgSelCfg->m_unitProp.listID = m_dlgConfig->m_cfgProp.ID;
            this->close();
            if(str == STR_LOADFLASH) // загрузка из флеш
                emit signalStartMainWnd(START_LOAD_FLASH, m_mainWnd_4DlgSelCfg->m_unitProp);
            else // загрузка с диска
                emit signalStartMainWnd(START_LOAD_FILE, m_mainWnd_4DlgSelCfg->m_unitProp);
            return;
        }
        if((m_listCfgEditors[i] == str) && (m_listNameCfgFiles[i] == parentStr)) {
            // открыть редактор конфигурации
            m_dlgConfig->show();
            QRect rect = this->geometry();
            m_dlgConfig->move(rect.left() + 100, rect.top() + 50);
            m_dlgConfig->xmlSetFileName(m_listPathCfgFiles[i]);
            return;
        }
    }
}

void DlgSelectCfg::on_BTN_NEWCONF_clicked()
{
    m_dlgConfig->showNewConf();
}

#if DELETE_ON
// for debug
void DlgSelectCfg::DeleteItem (QTreeWidget *treeWidget, QTreeWidgetItem *currentItem)
{
    QTreeWidgetItem *parent = currentItem->parent();
    int index;
    if (parent) {
        index = parent->indexOfChild(treeWidget->currentItem());
        delete parent->takeChild(index);
    }
    else {
        index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());
        delete treeWidget->takeTopLevelItem(index);
    }
}

void DlgSelectCfg::on_BTN_DELETE_clicked()
{
    if (m_item4Delete) {
        DeleteItem (ui->treeWidget_Name, m_item4Delete);
        m_item4Delete = nullptr;
    }
}
#endif


