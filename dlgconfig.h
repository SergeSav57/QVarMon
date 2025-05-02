#ifndef DLGCONFIG_H
#define DLGCONFIG_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QLabel>

namespace Ui {
class DlgConfig;
}

class DlgConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConfig(QWidget *parent = nullptr);
    ~DlgConfig();

#define CFG_FILE        "configFileQVarMon"
#define CFG_VERSION     "VerEdit_v.1.0"

#define STR_CREATEVARFILE   "Создать файл переменных"
#define STR_LOADFLASH       "Загрузка из флеш-памяти"
#define STR_LOADFILE        "Загрузка из файла конфигурации"
#define STR_VEREDITOR       "Редактор версии & Прошивка"
#define STR_FILECFGXML      "Файл конфигурации XML"
#define STR_OPENVARFILE     "Открыть файл переменных"
#define STR_OPENVAREXP      "Файлы переменных (*.svar)"

    uint32_t    m_unitsNum = 1;
    QString     m_cfgMyVersion{};
    QString     m_cfgName{};
    QString     m_cfgEditor{};
    bool        m_showFullPath = false;

    typedef struct {
        QStringList unitName;
        QStringList ID;
        QStringList varFilePath;
        QStringList varFileName;
        QStringList CRC32;
        QList<int>  loadEn;
        QList<int>  downLoad;
    } cfgProp_t;
    cfgProp_t m_cfgProp{};

    void initSize();
    void initTableChannels();
    void fillTableChannels();
    void initTableVarFiles();
    void fillTableVarFiles();
    void xmlSetFileName(QString filename);
    void xmlSaveFile();
    void xmlReadFile(QString path);
    void xmlOpenFile();
    void showNewConf();

public slots:
    void slotСellChanged(int row, int column);

private slots:
    void on_spinBox_valueChanged(int arg1);
    void on_checkBox_toggled(bool checked);
    void on_BTN_SAVE_clicked();
    void on_BTN_LOAD_clicked();
    void on_BTN_CLEAN_clicked();
    void on_BTN_FLASH_clicked();
    void onBtnLoadVarFileClicked();
    void cgfResize(int num);

private:
    Ui::DlgConfig *ui;
};

#endif // DLGCONFIG_H
