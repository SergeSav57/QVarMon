#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTabWidget>

#include "common.h"
#include "dlgtablework.h"
#include "dlgtablearray.h"
#include "dlgvarsel.h"
#include "dlgixxat.h"
#include "dlguart.h"
#include "dlgbineditor.h"
#include "dlgabout.h"
#include "dlghowtouse.h"
#include "dlgselectcfg.h"
#include "mylocalserver.h"
#include "dlgaddaddress.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

//=================================================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    DlgVarSel           *m_dlgVarSel;
    DlgIXXAT            *m_dlgIxxat;
    DlgUART             *m_dlgUart;
    DlgTableArray       *m_dlgTableArray;
    DlgAbout            *m_dlgAbout;
    DlgSelectCfg        *m_dlgSelectCfg;
    MyLocalServer       *m_myLocalServer;
    DlgHowToUse         *m_dlgHowToUse;
    DlgAddAddress       *m_dlgAddAddress;

    // вектор для хранения всех окрытых окон бинэдитора
    QVector<DlgBinEditor*>  m_dlgBinEditVector{};
    // вектор для хранения всех свойств окрытых окон массивов
    QVector<arrayDlgProp_t> m_arrDlgPropVector{};
    // вектор для хранения всех окрытых вкладок рабочих таблиц
    QVector<DlgTableWork*>  m_dlgTableWorkVector{};
    // рабочий список переменных, выбранных для построения графиков
    QVector<var_t>          m_chartSelVector{};


    QString nameTabWork(int index);

    // для записи данных в *.mon файл
    QFile       m_recFile{};
    QTextStream m_recStream{};

    clock_t     m_startTime{};      // время (в мс) начала отсчета времени записи в файл
    QTimer      *m_recTimer{};      // задающий таймер обновления показаний в индикаторе и мигания кнопки записи (период 250мс)
    QTime       m_recTimeStart{};   // время начала отсчета времени записи для вывода на индикатор
    QTime       m_recTimeEnd{};     // время окончания текущего кванта задающего таймера для расчета итогового времени записи

    bool        m_recFile_opened = false;   // файл открыт
    bool        m_recFile_started = false;  // запись началась

    QTimer      *m_ledsTimer{};     // таймер для перерисовки светиков каналов (период 200 мс)

    QMutex      m_mutexUpdateVarData;
    QMutex      m_mutexUpdateArrData;

    unitProp_t  m_unitProp{};

    QStringList LED_STRING_MENU = {"      < -Закрыть- >",
                                   "Загрузить параметры с борта",
                                   "Загрузить параметры из файла",
                                   "Сохранить файл параметров на борт",
                                   "Сохранить параметры с борта в файл"};
    typedef struct {
        QLineEdit   *editWnd;
        int         index;
    } editTabWnd_t;
    editTabWnd_t  m_editTabWnd{};

    typedef struct {
        uint32_t    ver;    // версия формата (структуры) файла
        uint32_t    fSz;    // размер файла
        uint32_t    cnt;    // число записей параметров (переменных)
        //>>> параметры записываемые при загрузке
        //>>> файла для мониторинга для конкретного канала
        uint32_t    uID;    // ID узла (микроконтроллера) в многопроцессорной системе
        QString     uName;  // имя узла (микроконтроллера) в многопроцессорной системе
    } varFileDesc_t;
    varFileDesc_t m_fileDescTmp{};

//#pragma pack(push, 4)
    typedef struct
    {	// реквизиты переменной
        QString     varName;        // имя переменной
        uint32_t	typeSize  : 4;	// размер типа переменной в байтах
        uint32_t	varTypeID : 4;	// идентификатор типа переменной
        uint32_t	arrCol    : 12;	// число колонок массива или 0, если простая переменная
        uint32_t	arrRow    : 6;	// число строк(рядов) многомерного массива или 0, если простая переменная
        uint32_t	reserved  : 6;  // не используем
        uint32_t	varSize;        // размер переменной в байтах (простых переменных, массивов, структур)
        uint32_t	varAddr;		// адрес переменной в памяти микроконтроллера
        varFileDesc_t desc;
    } varAttr_t;
//#pragma pack(pop)
    QVector<varAttr_t> m_varAttrVector[NUMCHANNELMAX];
    QVector<varAttr_t> m_varAttrVectorTmp{};


    typedef struct
    {	// для парсинга elf-файла и создания varfile
        QString		varType;        // тип данных переменной (int8_t, uint32_t, unsigned int и т.д.)
        QString		varAltType;     // альтернативное имя типа данных переменной (int8, uint32 и т.д.)
        QString		at_type;		// ссылка дерева на тип переменной (вида <1><2cd>:)
        int			dw_tag_str;     // тег структуры
        int			dw_tag_arr;     // тег массива
        int			dw_tag_vlt;     // тег volatile
        int			dw_tag_pnt;     // тег указателя
        int			member_loc;		// позиция (в байтах) члена структуры относительно базового адреса
        int			szBaseStruct;	// размер (в байтах) базовой структуры (для расчетов, если в нее входят другие структуры)
    } parsing_t;
    QVector<parsing_t> m_parsVector{};

    typedef struct
    {	//
        varAttr_t   varAttr;
        parsing_t   parsing;
    } varFileParsing_t;
    QVector<varFileParsing_t> m_varFileParsingVector{};

    // типы переменных
    typedef struct _typesVar_t
    {
        QString		varType;		// имя переменной (int8_t, uint32_t и т.д.)
        QString		varAltType;		// альтернативное имя (int8, uint32 и т.д.)
        uint32_t	varTypeID;		// тип переменных для идентификации
        uint32_t	varSize;		// размер переменной в байтах
    } typesVar_t;

public:
    // ф-ции для работы с вкладками
    void setCurrentTabWork(int index);
    void createNewTabWork(int index, bool opened);
    void setTabWorkName(int index, const QString &name);
    void tabWorkActivated(int index);
    int  countTabWork();
    int  currentTabWork();
    //----------------------------
    void saveRecData();
    void Trans2SysType(var_t *var);
    void DeleteDlgBinEdit(DlgBinEditor* pDlg);
    void saveGr();
    void ledCh_pressed(int pos);
    void initChannelLeds();
    void setChannelLeds();
    void startDlgChart();
    void startDlgArray(var_t &var, QPoint *pnt);
    void clearAllVectors();

    // XML
    void xmlSaveFile();
    void xmlOpenFile();

    // VarFile
    void readVarFile(uint32_t unit, QString fileName, bool list);
    void readVarFileFromCfg(unitProp_t &prop);
    void fillVarSelGlobalTable(uint32_t unit);
public slots:
    void slotReadVarFileFlash(uint32_t unit, QByteArray fileData);
    friend QDataStream &operator << (QDataStream &stream, const varFileDesc_t &desc);
    friend QDataStream &operator >> (QDataStream &stream, varFileDesc_t &desc);
    friend QDataStream &operator << (QDataStream &stream, const varAttr_t &var);
    friend QDataStream &operator >> (QDataStream &stream, varAttr_t &var);


public slots:
    void slotStartMainWnd(uint32_t startType, unitProp_t &prop);
    void slotTabWorkChanged(int index);
    void slotTabWorkClosed(int index);
    void slotBtnStartRecEn(bool en);
    void slotBtnSelRecParamEn(bool en);
    void slotCreateBinEditDlg(var_t &var, QPoint *pnt);
    void slotCreateVarAddrDlg(uint32_t pos, QPoint *pnt);
    void slotRecTimerAlarm();
    void slotLedsTimerAlarm();
    void slotDevIsConnected(int devId);
    void slotUpdateVarData(uint32_t unit, QVector<curData_t> &vec);
    void slotSendSelParamToDevice();
    void slotUpdateArrData(uint32_t unit, arrNewData_t &data);
    void slotLedChMenu(QListWidgetItem *item);
    void slotCloseDlgArr(int index);

private slots:
    void on_action_FILE_OPEN_TAB_triggered();
    void on_action_SELECT_CONF_triggered();
    void on_action_SELECT_PARAM_triggered();
    void on_action_ADDADDRESS_triggered();
    void on_action_FILE_CREATE_NEW_TAB_triggered();
    void on_action_FILE_SAVE_TAB_triggered();
    void on_action_APP_EXIT_triggered();
    void on_action_SHOW_CHART_triggered();
    void on_action_ABOUT_triggered();
    void on_action_HOWTOUSE_triggered();
    void on_BTN_START_RECORD_clicked();
    void on_BTN_SEL_REC_PARAM_clicked();

    void on_tabWidget4Work_tabBarDoubleClicked(int index);
    void setTabNewName();
signals:
    void signalChangeDevice(MSG *msg);
    void signalSendMainWndHandle(void *);
    void signalTableClear();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent * e) override; // закрытие окна
    void initMainWndInterface();
    bool nativeEvent(const QByteArray & eventType ,void *message, qintptr *result) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void RegDevice();


};
#endif // MAINWINDOW_H
