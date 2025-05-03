#include "common.h"
#include "dlgconfig.h"
#include "ui_dlgconfig.h"
#include "dlgselectcfg.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QXmlStreamReader>
#include "crc32.h"
#include "mainwindow.h"

MainWindow  *m_mainWnd_4DlgConfig{};
DlgSelectCfg *m_parent;

DlgConfig::DlgConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgConfig)
{
    ui->setupUi(this);
    m_parent = static_cast<DlgSelectCfg *>(parent);
    m_mainWnd_4DlgConfig = static_cast<MainWindow *>(m_parent->parent());

    setWindowFlag(Qt::WindowStaysOnTopHint);

    m_cfgProp.unitName.resize(1);
    m_cfgProp.ID.resize(1);
    ui->spinBox->setMinimum(1);
    ui->spinBox->setMaximum(8);

    ui->groupBox_files->setStyleSheet(QString("QGroupBox {color: %1}").arg(myColorBlue.name()));
}

DlgConfig::~DlgConfig()
{
    delete ui;
}

void DlgConfig::showNewConf()
{
    this->show();
    on_BTN_CLEAN_clicked();
    initSize();
}

void DlgConfig::initSize()
{
    QRect rect1 = ui->TABLE_CHANNELS->geometry();
    rect1.setHeight(80);
    ui->TABLE_CHANNELS->setGeometry(rect1);

    QRect rect2 = ui->TABLE_VAR_FILES->geometry();
    rect2.setHeight(m_unitsNum * 20 + 22);
    ui->TABLE_VAR_FILES->setGeometry(rect2);

    QRect rect3 = this->geometry();
    rect3.setHeight(rect1.height() + rect2.height() + 150);
    this->setGeometry(rect3);
}

void DlgConfig::initTableChannels()
{
    ui->TABLE_CHANNELS->clear();
    // Указываем число строк
    ui->TABLE_CHANNELS->setRowCount(2);
    // имена строк заголовка
    QString strHeader = "Имя,ID";
    // Указываем число колонок
    ui->TABLE_CHANNELS->setColumnCount(m_unitsNum);
    // Включаем сетку
    ui->TABLE_CHANNELS->setShowGrid(true);
    // Устанавливаем заголовки строк
    QStringList headers = strHeader.split(",");
    ui->TABLE_CHANNELS->setVerticalHeaderLabels(headers);
    // Разрешаем выделение построчно
    ui->TABLE_CHANNELS->setSelectionBehavior(QAbstractItemView::SelectItems);
    // Ограничим минимальную высоту строк
    ui->TABLE_CHANNELS->verticalHeader()->setMinimumSectionSize(20);
    ui->TABLE_CHANNELS->verticalHeader()->setMaximumSectionSize(20);
    ui->TABLE_CHANNELS->verticalHeader()->setDefaultSectionSize(20);
    // ui->TABLE_CHANNELS->horizontalHeader()->setMinimumSectionSize(30);
    ui->TABLE_CHANNELS->horizontalHeader()->setDefaultSectionSize(40);
    // установим фиксированную высоту заголовка
    ui->TABLE_CHANNELS->horizontalHeader()->setFixedHeight(20);
    // цвет заголовков QTableWidget, цвет выделенной строки и цвет фонта
    ui->TABLE_CHANNELS->setStyleSheet(QString("%1 %2").arg(myHeaderStyle, mySelectedStyle));
    // установим автоподстройку ширины заголовков колонок под размер записей списка
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    initSize();

    connect(ui->TABLE_CHANNELS, &QTableWidget::cellChanged,
            this, &DlgConfig::slotСellChanged);
}

void DlgConfig::initTableVarFiles()
{
    ui->TABLE_VAR_FILES->clear();
    // Указываем число строк
    ui->TABLE_VAR_FILES->setRowCount(m_unitsNum);
    // имена строк заголовка
    QString strHeader = "Имя, ID, Файл, CRC32(h), >";
    // Указываем число колонок
    ui->TABLE_VAR_FILES->setColumnCount(6);
    // Включаем сетку
    ui->TABLE_VAR_FILES->setShowGrid(true);
    // Устанавливаем заголовки строк
    QStringList headers = strHeader.split(",");
    ui->TABLE_VAR_FILES->setHorizontalHeaderLabels(headers);
    // Разрешаем выделение построчно
    ui->TABLE_VAR_FILES->setSelectionBehavior(QAbstractItemView::SelectItems);
    // Ограничим минимальную высоту строк
    ui->TABLE_VAR_FILES->verticalHeader()->setMinimumSectionSize(20);
    ui->TABLE_VAR_FILES->verticalHeader()->setMaximumSectionSize(20);
    ui->TABLE_VAR_FILES->verticalHeader()->setDefaultSectionSize(20);
    ui->TABLE_VAR_FILES->horizontalHeader()->setMinimumSectionSize(20);
    ui->TABLE_VAR_FILES->horizontalHeader()->setDefaultSectionSize(20);
    // установим фиксированную высоту заголовка
    ui->TABLE_VAR_FILES->horizontalHeader()->setFixedHeight(20);
    // цвет заголовков QTableWidget, цвет выделенной строки и цвет фонта
    ui->TABLE_VAR_FILES->setStyleSheet(QString("%1 %2").arg(myHeaderStyle, mySelectedStyle));
    // установим автоподстройку ширины заголовков колонок под размер записей списка
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->TABLE_VAR_FILES->horizontalHeader()->resizeSection(4, 20);

    const QIcon iconLoad(LOAD_BOARD);
    QTableWidgetItem *headerItem = new QTableWidgetItem(iconLoad,"");
    ui->TABLE_VAR_FILES->horizontalHeader()->setIconSize(QSize(14,14));
    ui->TABLE_VAR_FILES->setHorizontalHeaderItem(5, headerItem);
    ui->TABLE_VAR_FILES->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->TABLE_VAR_FILES->horizontalHeader()->resizeSection(5, 25);
}

void DlgConfig::fillTableChannels()
{
    if(!m_cfgName.isEmpty()) {
        ui->lineEdit_NameCfg->setText(m_cfgName);
        ui->spinBox->setValue(m_unitsNum);
    }
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        if(!m_cfgProp.unitName.isEmpty()) {
            QTableWidgetItem *itemName = new QTableWidgetItem();
            itemName->setData(Qt::BackgroundRole, myItemsColor(0));
            itemName->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemName->setData(Qt::DisplayRole, m_cfgProp.unitName[i]);
            ui->TABLE_CHANNELS->setItem(0, i, itemName);
        }
        if(!m_cfgProp.ID.isEmpty()) {
            QTableWidgetItem *itemID = new QTableWidgetItem();
            itemID->setData(Qt::BackgroundRole, myItemsColor(1));
            itemID->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemID->setData(Qt::DisplayRole, m_cfgProp.ID[i]);
            ui->TABLE_CHANNELS->setItem(1, i, itemID);
        }
    }
}

void DlgConfig::fillTableVarFiles()
{
    const QIcon iconApplay(APPLAY_ICON);
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        if(!m_cfgProp.unitName.isEmpty()) {
            QTableWidgetItem *itemName = new QTableWidgetItem();
            itemName->setData(Qt::BackgroundRole, myItemsColor(i));
            itemName->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemName->setData(Qt::DisplayRole, m_cfgProp.unitName[i]);
            ui->TABLE_VAR_FILES->setItem(i, 0, itemName);
        }
        if(!m_cfgProp.ID.isEmpty()) {
            QTableWidgetItem *itemID = new QTableWidgetItem();
            itemID->setData(Qt::BackgroundRole, myItemsColor(i));
            itemID->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemID->setData(Qt::DisplayRole, m_cfgProp.ID[i]);
            ui->TABLE_VAR_FILES->setItem(i, 1, itemID);
        }
        if(!m_cfgProp.varFileName.isEmpty()) {
            QTableWidgetItem *itemFile = new QTableWidgetItem();
            itemFile->setData(Qt::BackgroundRole, myItemsColor(i));
            itemFile->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            if(m_showFullPath) {
                itemFile->setData(Qt::DisplayRole, m_cfgProp.varFilePath[i]);
            }
            else {
                itemFile->setData(Qt::DisplayRole, m_cfgProp.varFileName[i]);
            }
            ui->TABLE_VAR_FILES->setItem(i, 2, itemFile);
        }
        if(!m_cfgProp.CRC32.isEmpty()) {
            QTableWidgetItem *itemCRC = new QTableWidgetItem();
            itemCRC->setData(Qt::BackgroundRole, myItemsColor(i));
            itemCRC->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemCRC->setData(Qt::DisplayRole, m_cfgProp.CRC32[i]);
            ui->TABLE_VAR_FILES->setItem(i, 3, itemCRC);
        }

        QPushButton* btn = new QPushButton("...");
        connect(btn, SIGNAL(clicked()), SLOT(onBtnLoadVarFileClicked()));
        btn->setProperty("channel", i);
        ui->TABLE_VAR_FILES->setCellWidget(i, 4, btn);

        if(m_cfgProp.downLoad[i] == 1) {
            QTableWidgetItem *itemFirmware = new QTableWidgetItem();
            itemFirmware->setData(Qt::BackgroundRole, myItemsColor(i));
            itemFirmware->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            itemFirmware->setIcon(iconApplay);
            ui->TABLE_VAR_FILES->setItem(i, 5, itemFirmware);
        }
    }
}

void DlgConfig::slotСellChanged(int row, int column)
{
    if(row == 0){
        m_cfgProp.unitName[column] = ui->TABLE_CHANNELS->item(0,column)->text();
    }
    if(row == 1){
        m_cfgProp.ID[column] = ui->TABLE_CHANNELS->item(1,column)->text();
    }

    fillTableVarFiles();
}

void DlgConfig::on_spinBox_valueChanged(int arg1)
{
    m_unitsNum = arg1;
    cgfResize(arg1);

    initTableChannels();
    fillTableChannels();
    initTableVarFiles();
    fillTableVarFiles();
}

void DlgConfig::onBtnLoadVarFileClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QVariant var = btn->property("channel");
    if (!var.isValid())
        return;
    int ch = var.toInt(); // номер строки таблицы, где нажата кнопка
    // путь к файлу, записанному в конфигурации
    QString curDir = m_cfgProp.varFilePath[ch];
    if(curDir.isEmpty())
        curDir = QDir::currentPath();
    QString filePath = QFileDialog::getOpenFileName(this, STR_OPENVARFILE, curDir, STR_OPENVAREXP);
    // полный путь
    m_cfgProp.varFilePath[ch] = filePath;
    QFileInfo fi1(filePath);
    // только имя файла с расширением
    m_cfgProp.varFileName[ch] = fi1.fileName();
    fillTableVarFiles();
}

void DlgConfig::cgfResize(int num)
{   // аргумент num = 0 очищает структуру
    m_cfgProp.unitName.resize(num);
    m_cfgProp.ID.resize(num);
    m_cfgProp.varFilePath.resize(num);
    m_cfgProp.varFileName.resize(num);
    m_cfgProp.CRC32.resize(num);
    m_cfgProp.loadEn.resize(num);
    m_cfgProp.downLoad.resize(num);
    for(int i = 0; i < num; ++i) {
        m_cfgProp.downLoad[i] = 0;
        m_cfgProp.loadEn[i] = 0;
    }
}

void DlgConfig::on_BTN_CLEAN_clicked()
{
    m_cfgName.clear();
    m_cfgEditor.clear();
    ui->lineEdit_NameCfg->clear();
    ui->TABLE_CHANNELS->clear();

    cgfResize(0);
    cgfResize(1);

    ui->spinBox->setValue(1);

    initTableChannels();
    fillTableChannels();
    initTableVarFiles();
    fillTableVarFiles();
}

void DlgConfig::on_BTN_SAVE_clicked()
{
    m_cfgMyVersion = CFG_VERSION;
    m_cfgName = ui->lineEdit_NameCfg->text();
    if(m_cfgName.isEmpty()) {
        QMessageBox::warning(nullptr, "",
                             "Введите имя конфигурации", QMessageBox::Ok);
        return;
    }
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        if(m_cfgProp.varFilePath[i].isEmpty()) {
            QMessageBox::warning(nullptr, "",
                                 QString("Введите путь файла переменных канала %1 [%2]")
                                 .arg(m_cfgProp.unitName[i], m_cfgProp.ID[i]),
                                 QMessageBox::Ok);
            return;
        }
    }
    xmlSaveFile();
    this->close();
    m_parent->findCfgFiles();
}

void DlgConfig::on_BTN_LOAD_clicked()
{
    xmlOpenFile();
}

void DlgConfig::on_BTN_FLASH_clicked()
{
    // толкнем переключатель данных каналов
    m_mainWnd_4DlgConfig->m_dlgIxxat->getDevId();
    delayMs(5);
    // определим подключены ли нужные каналы
    for(int n = 0; n < m_cfgProp.ID.size(); ++n) {
        for(int i = 0; i < NUMCHANNELMAX; ++i) {
            if(m_cfgProp.ID[n] == QString::number(m_mainWnd_4DlgConfig->m_unitProp.property[i].unitID)) {
                if(m_mainWnd_4DlgConfig->m_unitProp.property[i].isPresent) {
                    qDebug()<<"Ch:"<<i<<"isPresent";
                    m_cfgProp.loadEn[n] = 1;

                }
                else {
                    qDebug()<<"Ch:"<<i<<"is Not Present";
                    m_cfgProp.loadEn[n] = 0;
                }
            }
        }
    }
    // Сохраним файлы параметров на борт
    for(int i = 0; i < m_cfgProp.varFilePath.size(); ++i) {
        if(m_cfgProp.loadEn[i]) {
            QFile file;
            file.setFileName(m_cfgProp.varFilePath[i]);
            // в ф-ции saveVarFileToUnitBoard() есть ожидание завершения с таймаутом
            if(!m_mainWnd_4DlgConfig->m_dlgIxxat->saveVarFileToUnitBoard(m_cfgProp.ID[i].toInt(), file)) {
                showSelfClosedMessageBox(2000,
                    QString("Не удалось записать во флеш канала %1 файл параметров: Таймаут")
                    .arg(i));
                m_cfgProp.downLoad[i] = 0;
            }
            else {
                m_cfgProp.downLoad[i] = 1;
                fillTableVarFiles();
            }
        }
    }
}

void DlgConfig::on_checkBox_toggled(bool checked)
{
    m_showFullPath = checked;
    fillTableVarFiles();
}


// ********* XML ************************************************************

void DlgConfig::xmlSetFileName(QString filename)
{
    xmlReadFile(filename);
}

void DlgConfig::xmlSaveFile()
{
    QString preName = "./config/conf_" + m_cfgName.toLower();
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Save XML", preName,
                                                    "XML files (*.xml)");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    // Создаем объект, с помощью которого осуществляется запись в файл
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);  // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();     // Запускаем запись в документ
    xmlWriter.writeStartElement(CFG_FILE);   // стартовый идентификатор файла конфигурации

    xmlWriter.writeStartElement("Version");   // имя версии файла
    xmlWriter.writeAttribute("ver", m_cfgMyVersion);
    xmlWriter.writeEndElement(); // закрываем "Version"

    xmlWriter.writeStartElement("Config");   // имя конфигурации
    xmlWriter.writeAttribute("cfg", m_cfgName);
    xmlWriter.writeEndElement(); // закрываем "Config"

    xmlWriter.writeStartElement("Editor"); // имя редактора версии
    m_cfgEditor = "Редактор версии " + m_cfgName;
    xmlWriter.writeAttribute("edit", m_cfgEditor);
    xmlWriter.writeEndElement(); // закрываем "Editor"

    xmlWriter.writeStartElement("Number");   // тег с числом узлов
    xmlWriter.writeAttribute("num", QString::number(m_unitsNum));
    xmlWriter.writeEndElement(); // закрываем "Number"

    xmlWriter.writeStartElement("UnitsID");   // тег ID узлов
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        xmlWriter.writeStartElement(QString("DevID").arg(i));   // тег ID для первого узла
        xmlWriter.writeAttribute(QString("id"), m_cfgProp.ID[i]);
        xmlWriter.writeEndElement(); // "DevID"
    }
    xmlWriter.writeEndElement(); // закрываем "UnitsID"

    xmlWriter.writeStartElement("UnitsName");   // тег имен узлов
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        xmlWriter.writeStartElement("Dev");   // тег имени каждого узла
        xmlWriter.writeAttribute("name", m_cfgProp.unitName[i]);
        xmlWriter.writeEndElement(); // "Dev"
    }
    xmlWriter.writeEndElement(); // закрываем "UnitsName"

    xmlWriter.writeStartElement("VarFile");   // тег путей varfile'ов
    for(uint32_t i = 0; i < m_unitsNum; ++i) {
        xmlWriter.writeStartElement("File");   // тег имени каждого узла
        xmlWriter.writeAttribute("path", m_cfgProp.varFilePath[i]);
        xmlWriter.writeEndElement(); // "Dev"
    }
    xmlWriter.writeEndElement(); // закрываем "VarFile"

    // Завершаем запись в документ
    xmlWriter.writeEndDocument();   // закрываем cfgName

    file.close();
}

void DlgConfig::xmlOpenFile()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                "Open Xml", ".",
                                                "Xml files (*.xml)");
    xmlReadFile(path);
}

void DlgConfig::xmlReadFile(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Ошибка файла XML",
                             "Не удалось открыть файл",
                             QMessageBox::Ok);
        return;
    }

    QXmlStreamReader xmlReader(&file);

    cgfResize(0);

    bool first = true;
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if (token == QXmlStreamReader::StartDocument) { // Начало документа
            // qDebug() << "Start Document";
        }
        else if (token == QXmlStreamReader::StartElement) { // Начало элемента
            QString elementName = xmlReader.name().toString();
            // qDebug() << "Start Element:" << elementName;
            if(first) {
                first = false;
                if(elementName != CFG_FILE) { // стартовый идентификатор файла конфигурации
                    QMessageBox::critical(nullptr, "",
                                          "Файл конфигурации не соответствует программе QVarMon",
                                          QMessageBox::Ok);
                    return;
                }
            }
            if (elementName == "Version") { // имя моей версии файла
                m_cfgMyVersion = xmlReader.attributes().value("ver").toString();
                if(CFG_VERSION != m_cfgMyVersion) {
                    QMessageBox::critical(nullptr, "",
                                "Версия файла конфигурации не соответствует текущей версии программы",
                                QMessageBox::Ok);
                    return;
                }
            }
            if(elementName == "Config") { // имя конфигурации
                m_cfgName = xmlReader.attributes().value("cfg").toString();
            }
            if (elementName == "Editor") {
                m_cfgEditor = xmlReader.attributes().value("edit").toString();
            }
            if (elementName == "Number") {
                QString num = xmlReader.attributes().value("num").toString();
                m_unitsNum = num.toUInt(); // число узлов
                for(uint32_t i = 0; i < m_unitsNum; ++i) {
                    m_cfgProp.downLoad.append(0);
                    m_cfgProp.loadEn.append(0);
                }
            }
            else if (elementName == "DevID") {
                QString id = xmlReader.attributes().value("id").toString();
                m_cfgProp.ID.append(id);
            }
            else if (elementName == "Dev") {
                QString name = xmlReader.attributes().value("name").toString();
                m_cfgProp.unitName.append(name);
            }
            else if (elementName == "File") {
                QString path = xmlReader.attributes().value("path").toString();
                // m_cfgProp.resize(m_cfgProp.size() + 1);
                m_cfgProp.varFilePath.append(path);
                QFileInfo fi(path);
                // только имя файла с расширением
                m_cfgProp.varFileName.append(fi.fileName());
                QFile file(path);
                if(file.open(QIODevice::ReadOnly)) {
                    QByteArray buf = file.readAll();
                    uint32_t crc32 = getCRC32(buf, buf.size());
                    QString str = QString::number(crc32, 16).toUpper();
                    m_cfgProp.CRC32.append(str);
                    file.close();
                }
            }
        }
        else if (token == QXmlStreamReader::EndElement) {
            // Конец элемента
        }
        else if (token == QXmlStreamReader::Characters) {
            // Текстовые данные (если есть)
            if (!xmlReader.isWhitespace()) {
                // qDebug() << "Text:" << xmlReader.text().toString();
            }
        }
    }

    file.close();

    if (xmlReader.hasError()) {
        qWarning() << "XML error:" << xmlReader.errorString();
    }

    initTableChannels();
    fillTableChannels();
    initTableVarFiles();
    fillTableVarFiles();
}

