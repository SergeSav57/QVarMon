#pragma once

#ifndef COMMON_H
#define COMMON_H

#define WIN32_LEAN_AND_MEAN

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVector>
#include <QFile>
#include <QListWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTableView>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtMinMax>
#include <QLocalSocket>
#include <QProcess>

// мои ID для своих органов управления
#define ID_MENU_STRING_DEC	5000
#define ID_MENU_STRING_HEX	5001
#define ID_EDIT_VALUE		5002
#define ID_LISTBOX_SYS		5003
#define ID_LISTBOX_SYSALL	5004
#define ID_MENU_LDR_PAR		5005
#define ID_MENU_LDR_CONF	5006
#define ID_BOX_RENAME_TAB	5007
#define ID_MY_LIST			5050
// окна размещения значений битов диалога двочного редактора
#define ID_STATIC_BIT_0		6000
#define ID_STATIC_BIT_1		6001
#define ID_STATIC_BIT_2		6002
#define ID_STATIC_BIT_3		6003
#define ID_STATIC_BIT_4		6004
#define ID_STATIC_BIT_5		6005
#define ID_STATIC_BIT_6		6006
#define ID_STATIC_BIT_7		6007
#define ID_STATIC_BIT_8		6008
#define ID_STATIC_BIT_9		6009
#define ID_STATIC_BIT_10	6010	
#define ID_STATIC_BIT_11	6011	
#define ID_STATIC_BIT_12	6012	
#define ID_STATIC_BIT_13	6013	
#define ID_STATIC_BIT_14	6014	
#define ID_STATIC_BIT_15	6015
#define ID_STATIC_BIT_16	6016
#define ID_STATIC_BIT_17	6017
#define ID_STATIC_BIT_18	6018
#define ID_STATIC_BIT_19	6019
#define ID_STATIC_BIT_20	6020
#define ID_STATIC_BIT_21	6021
#define ID_STATIC_BIT_22	6022
#define ID_STATIC_BIT_23	6023
#define ID_STATIC_BIT_24	6024
#define ID_STATIC_BIT_25	6025
#define ID_STATIC_BIT_26	6026
#define ID_STATIC_BIT_27	6027
#define ID_STATIC_BIT_28	6028
#define ID_STATIC_BIT_29	6029
#define ID_STATIC_BIT_30	6030
#define ID_STATIC_BIT_31	6031

// какой выбран режим в данный момент работы
#define	MODE_PARAM          0x4000		// режим выбора параметров для мониторинга
#define	MODE_ARRAY          0x8000		// режим выбора всех массивов для мониторинга
#define	MODE_CHART          0x10000		// режим выбора параметров для построения графиков
#define	MODE_RECORD         0x20000		// режим выбора параметров для записи *.mon файла
#define	MODE_FIND           0x40000
#define	MODE_CLEAR(reg)     (reg &=~(MODE_PARAM  | \
                                     MODE_ARRAY  | \
                                     MODE_CHART  | \
                                     MODE_RECORD | \
                                     MODE_FIND))	// очистка выбора параметров
// иконки
#define MAIN_ICON           (":/res/displayChip.ico")
#define DCHIP_ICON          (":/res/displayChip.png") // *.png не отображается в проводнике Windows и иконке ярлыка программы
#define LED_GRAY_ICON       (":/res/grayled.png")
#define LED_RED_ICON        (":/res/redled.png")
#define LED_GREEN_ICON      (":/res/greenled.png")
#define LED_YELLOW_ICON     (":/res/yellowled.png")
#define CANCEL_ICON         (":/res/cancel.png")
#define OPEN_ICON           (":/res/open.png")
#define LOAD_BOARD          (":/res/download_32.png")
#define START_ICON          (":/res/start.png")
#define OFF_ICON            (":/res/off.png")
#define SAVE_ICON           (":/res/save.png")
#define SAVEAS_ICON         (":/res/save_as.png")
#define HELP_ICON           (":/res/help.png")
#define CREATVAR_ICON       (":/res/varfile.png")
#define TABLE_ICON          (":/res/table.png")
#define CHART_ICON          (":/res/display_chart.png")
#define TOCENTER_ICON       (":/res/tocenter.png")
#define PAUSE_ICON          (":/res/pause.png")
#define CLEAR_ICON          (":/res/stop.png")
#define PLAY_ICON           (":/res/play.png")
#define RECORD_ICON         (":/res/video.png")
#define RECPAUSE_ICON       (":/res/target.png")
#define TABOPEN_ICON        (":/res/tab_open.png")
#define TABNEW_ICON         (":/res/tab_new1.png")
#define SPISOK_ICON         (":/res/spisok_32.png")
#define FOLDER_LOAD_ICON    (":/res/folder_load-48.png")
#define SUN_ON_ICON         (":/res/sun-30.png")
#define SUN_OFF_ICON        (":/res/sun-32.png")
#define ENGINEERING_ICON    (":/res/engineering.png")
#define XMLSOURCE_ICON      (":/res/xmlsource3.png")
#define ADDVAR_ICON         (":/res/plusshield24.png")
#define APPLAY_ICON         (":/res/apply.png")


// поля (столбцы) окон листов выбора параметров (ALL & SEL)
#define NAME_SEL		0
#define UNIT_SEL		1
#define TYPE_SEL        2
#define ARR_SEL			3

// поля (столбцы) окон листа мониторинга параметров (WORK)
#define NUM_EL			0
#define UNIT_EL			1
#define NAME_EL			2
#define VAL_EL			3
#define SYS_EL			4
#define TYPE_EL			5
#define MAX_COLUMN		6 // число колонок таблицы (для построения дополнительных таблиц справа)

// поля (столбцы) листа графиков
#define NAME_CH			0
#define UNIT_CH			1
#define VAL_CH			2
#define RANGE_CH		3
#define MIDDLE_CH		4

#define	STR_DEC			("Dec")
#define	STR_HEX			("Hex")
#define	STR_BIN			("Bin")

#define	SMB_DEC			("D")
#define	SMB_HEX			("H")
#define	SMB_FLT			("F")
#define	SMB_BIN			("B")
#define	SMB_ARR			("#")

// переключатели систем счисления (sysID)
#define DECSYS			0
#define HEXSYS			1
#define BINSYS			2
#define CHARSYS			3
#define DECCHAR			4
#define HEXCHAR			5
#define FLOATSYS		6

// тип переменных для идентификации (varTypeID)
#define TYPE_INT8		1
#define TYPE_UINT8		2
#define TYPE_INT16		3
#define TYPE_UINT16		4
#define TYPE_INT32		5
#define TYPE_UINT32		6
#define TYPE_FLOAT		7

#define NUMCHANNELMAX   8

// преобразование big-endian в little-endian и обратно (для uint32)
#define BE2LE(var)  ( ((var & 0x000000FF) << 24) |  \
                      ((var & 0x0000FF00) <<  8) |  \
                      ((var & 0x00FF0000) >>  8) |  \
                      ((var & 0xFF000000) >> 24) )

// Порядок байтов little-endian (младший байт в конце)
#define BYTES2INT_LE(byte, n)  (byte[n+0]|byte[n+1]<<8|byte[n+2]<<16|byte[n+3]<<24)
// Порядок байтов big-endian (старший байт в конце)
#define BYTES2INT_BE(byte, n)  (byte[n+3]|byte[n+2]<<8|byte[n+1]<<16|byte[n+0]<<24)

#define START_LOAD_FLASH    0
#define START_LOAD_FILE     1
#define START_NOLOAD        2

    typedef struct { // идентификация переменной
        uint32_t	varID;			// идентификатор параметра
        QString		varName;		// имя переменной
        uint32_t	unitID;         // ID узла (микроконтроллера) в многопроцессорной системе
        QString		unitName;       // имя узла (микроконтроллера) в многопроцессорной системе
    } varID_t;

    typedef struct { // значения переменной
        uint32_t	memData;		// значение переменной (в памяти микроконтроллера)
        uint32_t	varTime;		// (для графика) время фиксации величины переменной из микроконтроллера (в мс)
        QString		varValueSD;		// значение переменной (в текстовом формате в DEC форме) -(используется только в VarMon)
        QString		varValueSH;     // значение переменной (в текстовом формате в HEX форме) -(используется только в VarMon)
        QString		varValueCh;     // значение переменной (в символьном формате) -(используется только в VarMon)
        float		varValueF;		// значение переменной, приведенная к типу float (с плавающей точкой)
    } varData_t;

    typedef struct { // свойства переменной
        uint32_t	varSize;		// размер переменной в байтах (простых переменных, массивов, структур)
        uint32_t	varAddr;		// адрес переменной в памяти микроконтроллера
        uint32_t	varID;          // ID переменной
        uint32_t	arrCol;			// размер массива (число колонок) или 1, если простая переменная
        uint32_t	arrRow;			// число строк(рядов) многомерного массива
        uint32_t	sysID;          // индекс выбора показа системы счисления (Dec, Hex, Char)
    } varProp_t;

    typedef struct { // тип переменной
        QString		altType;		// альтернативное (строковое) имя типа данных переменной (int, uint32 и т.д.)
        uint32_t	typeID;         // идентификатор типа переменной для пересчета
        uint32_t	typeSize;		// размер типа переменной (в байтах)
    }varType_t;

    typedef struct { // флаги переменной
        bool		f_Monitor;		// выбранный параметр для мониторинга
        bool		f_Record;		// выбранный параметр для записи в файл
        bool		f_Chart;		// выбранный параметр для графика
        bool		f_Array;		// выбранный параметр для массива
    } varFlags_t;

    typedef struct // основная структура для хранения всех параметров переменной
    {	// реквизиты переменной
        varID_t         varID;
        // значения переменной
        varData_t       varData;
        // свойства переменной
        varProp_t       varProp;
        // тип переменной
        varType_t       varType;
        // флаги переменной
        varFlags_t      varFlags;
    } var_t;

    typedef struct {
        uint32_t    varID;
        // 2*4 байта во фрейме
        uint32_t    data0;
        uint32_t    data1;
    } curData_t;

    typedef struct {
        uint32_t    numVarsHost;        // число переменных в посылке хоста
        uint32_t    numVarsDev;         // число переменных в посылке девайса
        QVector<curData_t> vecCurData;  // данные всех переменных
    } varNewData_t;

    typedef struct {
        uint32_t    numArrs;    // число массивов узла
        uint32_t    arrSz;      // размер текущего массива
        uint32_t    nArrFrames; // число кадров в текущем массиве (в дампе памяти под массив) -(определяет первый фрейм перед данными в посылке массивов)
        QVector<curData_t> vecCurData; // данные текущего массива
    } arrNewData_t;

    typedef struct { // для отрисовки светиков каналов и их атрибутов
        QString     lbStringName;   // имя канала
        QString     lbStringCh;     // номер канала
        QLabel      *lbLed;         // лейбл светика канала
        QLabel      *lbTextName;    // лейбл текста имени канала
        QLabel      *lbTextCh;      // лейбл текста номера канала
        QListWidget *listWnd;       // всплывающее окно меню
    } ledsChVec_t;

    typedef struct {
        int             unitID;             // ID узла
        bool            arrEnable;
        QString         unitName;           // имя узла
        QString         unitVarFile;        // путь к файлу переменных узла
        ledsChVec_t     ledsChVec;          // список свойств светиков каналов
        varNewData_t    varNewData;         // новые данные для переменных
        arrNewData_t    arrNewData;         // новые данные для массивов
        QVector<arrNewData_t> arrAllVector; // вектор всех массивов узла
        bool            isPresent;          // флаг: девайс подключен
        bool            isOnlyView;         // флаг: девайс не подключен, но представлен в конфигурации
        bool            isSaveVarFile;      // флаг: сохранение файла переменных из девайса в файл
        bool            isLoadVarFileFlash; // флаг: файл переменных загружен из девайса
        bool            isLoadVarFileDisk;  // флаг: файл переменных загружен с диска
        bool            fReloadFile;        // флаг: перезагрузить файл переменных
    } unitsProperty_t;

    typedef struct {
        uint32_t        numChannels;        // число каналов (узлов м/к)
        QString         sCfgName;           // имена конфигураций
        QStringList     listVarFilePath;    // список путей файлов переменных
        QStringList     listID;             // список каналов в данной конфигурации
        unitsProperty_t property[NUMCHANNELMAX];
    } unitProp_t;
    // unitProp_t  m_unitProp{};

    // стр-ра данных, передаваемых в приложение QVarChart
    typedef struct {
        QString     varName;
        QString     unitName;
        uint32_t    varID;
        uint32_t    unitID;
        uint32_t	typeID;
        uint32_t    varTime;
        float       varValF;
    } chartData_t;

    typedef struct {
        QLocalSocket *socket;       // сокет в локальной сети, через который идет обмен данными
        QString     unitName;       // имя узла (микроконтроллера) в многопроцессорной системе
        QString     varName;        // имя массива
        QString     varAltType;     // альтернативное имя типа данных массива (int, uint32 и т.д.)
        uint32_t	unitID;         // ID узла (микроконтроллера) в многопроцессорной системе
        uint32_t    varAddr;        // адрес массива в памяти микроконтроллера
        uint32_t    varID;
        uint32_t    varSize;		// размер массива (в байтах)
        uint32_t    varTypeID;      // идентификатор типа переменной
        uint32_t    typeSize;       // размер типа переменной (в байтах)
        uint32_t    arrRow;         // число строк(рядов) многомерного массива
        uint32_t    arrCol;         // число колонок массив
        QProcess    *proc;
    } arrayDlgProp_t;

// моя ф-ция округления double to int
#define ROUND(x)	((x>0)?((int)(x+0.5)):((int)(x-0.5)))
// получить размах значений
#define GET_RANGE(ymin, ymax)	(ymax - ymin)
// получить среднее значение
#define GET_MIDDLE(ymin, ymax)	(ymin + (ymax - ymin) / 2.0)

#define myColorBlue (QColor(0, 100, 200))
#define myColorDarkBlue (QColor(0, 70, 150))
// цвета символов и фона выделенных строк таблиц
#define myItemsColor(n) ((n%2)==0 ? QColor(250, 250, 250) : QColor(230, 240, 250))
// цвет заголовков QTableWidget
#define myHeaderStyle ("QHeaderView::section {background:rgb(230,230,230);" \
                              "color:rgb(0, 100, 200); font-weight:medium;}")
// цвет выделенной строки и цвет фонта
#define mySelectedStyle ("QTableWidget::item:selected {background-color:rgb(0,120,215); color:white;}")
// строка версии формата файла переменных
#define myVarFileVer_1_0    (uint32_t)(0x0010) //"VarFile.Ver_1.0"
#define myCfgFileVer_1_0    (uint32_t)(0x0010) //"CfgFile.Ver_1.0"

inline static bool checkVarId(var_t var1, var_t var2)
{
    if(var1.varID.varID == var2.varID.varID)
        return true;
    else
        return false;
}

// === компараторы для сравнения строк таблицы при сортировке =============
// ========================================================================
inline static bool compareUpName(const var_t &var1, const var_t &var2)
{   //! сортировка по возрастанию
    // сравнение ведем в верхнем регистре строк,
    // иначе прописные буквы перевешивают строчные
    QString s1 = var1.varID.varName.toUpper();
    QString s2 = var2.varID.varName.toUpper();
    QString s3 = var1.varID.unitName.toUpper();
    QString s4 = var2.varID.unitName.toUpper();
    // сначала анализируем varName и если они равны, то принимаемся за unitName.
    if(s1 < s2)
        return true;
    if(s1 > s2)
        return false;
    return s3 < s4;
}

inline static bool compareDwnName(const var_t &var1, const var_t &var2)
{   //! сортировка по убыванию
    // сравнение ведем в верхнем регистре строк,
    // иначе прописные буквы перевешивают строчные
    QString s1 = var1.varID.varName.toUpper();
    QString s2 = var2.varID.varName.toUpper();
    QString s3 = var1.varID.unitName.toUpper();
    QString s4 = var2.varID.unitName.toUpper();
    // сначала анализируем varName и если они равны, то принимаемся за unitName.
    if(s1 > s2)
        return true;
    if(s1 < s2)
        return false;
    return s3 > s4;;
}

inline static bool compareUpUnit(const var_t &var1, const var_t &var2)
{   //! сортировка по возрастанию
    // сравнение ведем в верхнем регистре строк,
    // иначе прописные буквы перевешивают строчные
    QString s1 = var1.varID.unitName.toUpper();
    QString s2 = var2.varID.unitName.toUpper();
    QString s3 = var1.varID.varName.toUpper();
    QString s4 = var2.varID.varName.toUpper();
    // сначала анализируем unitName и если они равны, то принимаемся за varName.
    if(s1 < s2)
        return true;
    if(s1 > s2)
        return false;
    return s3 < s4;
}

inline static bool compareDwnUnit(const var_t &var1, const var_t &var2)
{   //! сортировка по убыванию
    // сравнение ведем в верхнем регистре строк,
    // иначе прописные буквы перевешивают строчные
    QString s1 = var1.varID.unitName.toUpper();
    QString s2 = var2.varID.unitName.toUpper();
    QString s3 = var1.varID.varName.toUpper();
    QString s4 = var2.varID.varName.toUpper();
    // сначала анализируем unitName и если они равны, то принимаемся за varName.
    if(s1 > s2)
        return true;
    if(s1 < s2)
        return false;
    return s3 > s4;;
}
// ========================================================================

inline static void delayMs(int ms)
{   // организация задержек (в мсек)
    QEventLoop loop;
    QTimer::singleShot(ms, Qt::TimerType::PreciseTimer, &loop, &QEventLoop::quit);
    loop.exec();
}

inline static bool waitFlagEn(std::atomic<bool> &flag, uint32_t ms)
{   // ожидает установки флага (flag) с таймаутом (ms)
    QEventLoop loop;
    // Таймер для истечения времени ожидания
    QTimer timer;
    timer.setSingleShot(true);
    QTimer::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(ms);

    // Ожидание сигнала готовности
    while (!flag) {
        loop.processEvents(); // Обработка событий
        if (!timer.isActive()) {
            qDebug() << "Таймаут истек!";
            return false; // Таймаут истек
        }
    }
    // qDebug() << "waitFlagEn() : Сигнал готовности получен!";
    flag = false;   // установим флаг в исходное состояние
    return true;    // Сигнал готовности получен
}

inline static void showSelfClosedMessageBox(int msec, QString msg)
{   // выводит в окошке сообщение с текстом msg на время msec
    QMessageBox Text;
    Text.setText(msg); //записываем строку в messagebox
    QTimer::singleShot(msec, &Text, &QMessageBox::close);
    Text.exec();
}


#endif // COMMON_H
