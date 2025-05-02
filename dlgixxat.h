#ifndef DLGIXXAT_H
#define DLGIXXAT_H

#include <QMutex>
#include <QSemaphore>
#include "common.h"
#include "IXXATCmd.h"

namespace Ui {
class DlgIXXAT;
}

class DlgIXXAT;

class ReadThread : public QObject
{   // класс потока для чтения от девайса
    Q_OBJECT

public:
    ReadThread(QString name, void *dlg);
    ~ReadThread();

    QThread *thread;
    bool    stop;
    // bool    fArrDataStop;

public slots:
    void doWorkReadThread();

signals:
    void signalDevIsConnected(int devId);
    void signalUpdateVarData(uint32_t unit, QVector<curData_t> &vec);
    // void signalUpdateArrData(uint32_t unit, QVector<arrNewData_t> &vec);
    void signalUpdateArrData(uint32_t unit, arrNewData_t &data);
    void signalReadVarFileFlash(uint32_t unit, QByteArray data);
    void signalVarFileError(int unit);
    void signalChannelChange(bool en);
    // void signalChannelChange(uint32_t unit, bool en);
    void signalChannelDone(uint32_t unit);

private:
    QString name;
    DlgIXXAT *pIxxat;
};


static const GUID GuidInterfaceList[]
{   // GUID класса VCI4 {a5442f91-d976-41a3-a49d-2dd90798d69b}
    { 0xa5442f91, 0xd976, 0x41a3, { 0xa4, 0x9d, 0x2d, 0xd9, 0x07, 0x98, 0xd6, 0x9b } },
};

class DlgIXXAT : public QDialog
{
    Q_OBJECT

public:
    explicit DlgIXXAT(QWidget *parent = nullptr);
    ~DlgIXXAT();

    ReadThread          *m_readThread;

// ID скоростей CAN
#define BR_1000K		0
#define BR_800K			1
#define BR_500K			2
#define BR_250K			3
#define BR_125K			4
#define BR_100K			5
#define BR_50K			6
#define BR_20K			7
#define BR_10K			8

//!!! макросы работы с битами команд одинаковы для девайса и хоста
// ------------------------------------------------------------
// позиции битов начала полей команд в ID кадра
#define BIT_FILE_VAR            27      // бит определяет работу с файлом переменных  (бит 27)
#define BIT_ARR                 26      // бит определяет работу с массивами  (бит 26)
#define BIT_DAT                 25      // бит данных (бит 25)
#define BIT_CMD                 24      // бит команды (биты 24)
#define BIT_CMD_NAME            0       // идентификатор имени(номера) команды (биты 0..3)
#define BIT_ID_DEV              20      // ID узла в многопроцессорном девайсе (биты 20..23)
#define BIT_VAR_NUM             4       // число переменных (биты 4..19)
#define BIT_VAR_EX          	19      // бит признака ID параметра с добавленным адресом (бит 19)
#define BIT_VAR_ID              0		// индекс переменной (биты 0..19)
#define BIT_ARR_NUM             4       // общее число массивов (биты 4..19)
#define BIT_ARR_SZ          	4       // размер массива (биты 4..19)
#define BIT_ARR_ID          	4       // индекс массива для разрешения/запрета сканирования (биты 4..18)
#define BIT_ARR_EN          	19      // разрешить/запретить сканирование адресов массива (бит 19)
#define BIT_FILE_SZ             4       // размер файла переменных (биты 4..19)

// ширина полей
#define LEN_CMD                 0x1F	// поле команды/данных/массива (биты 24..28)
#define LEN_ID_DEV				0xF		// поле узла девайса (биты 20..23)
#define LEN_CMD_NAME			0xF		// поле идентификатора имени(номера) команды (биты 0..3)
#define LEN_VAR_NUM				0xFFFF	// поле числа переменных (биты 4..19)
#define LEN_VAR_ID				0xFFFFF	// поле индекса переменной (биты 0..19)
#define LEN_VAR_EX			    0x1     // поле установки признака ID параметра с добавленным адресом (бит 19)
#define LEN_ARR_NUM		    	0xFFFF	// поле общего числао сканируемых массивов (биты 4..19)
#define LEN_ARR_SZ				0xFFFF	// поле размера массива (биты 4..19)
#define LEN_ARR_ID				0x7FFF	// поле индекса массива (биты 4..18)
#define LEN_ARR_EN              0x1     // поле разрешения/запрета сканирования адресов массива (бит 19)
#define LEN_FILE_SZ				0xFFFF	// поле размера файла переменных (биты 4..19)

// идентификаторы команды/данных в 5и старших разрядах ID CAN расширенного пакета (биты 24..28)
#define CAN_CMD					1		// послать/принять команду (бит 24)
#define VAR_DAT					2		// послать/принять данные  (бит 25)
#define ARR_DAT					4 		// работа с массивом (бит 26)
#define VAR_FILE				8 		// работа с файлом параметров (бит 27)
#define PC_REQ                  16 		// запрос параметров от PC (бит 28)

// идентификаторы команд от РС (выставлен бит PC_REQ = 1) - (биты 3..0 ID CAN кадра)
#define CMD_NEW_VAR				0x1		// {команда PC} - посланы новые переменные и старые надо удалить
#define CMD_NEW_ARR				0x2		// {команда PC} - посланы новые массивы и старые надо удалить
#define CMD_SCAN_EN             0x3     // {команда PC} - разрешить/запретить сканирование адресов массива(номер массива)
#define CMD_GET_DATA            0x4     // {запрос PC}  - послать данные массивов;
#define CMD_FILE_SAVE			0x5		// {команда PC} - сохранить во флеш файл переменных
#define CMD_FILE_LOAD			0x6		// {команда PC} - загрузить в хост файл переменных из флеш
#define CMD_GET_DEV_ID			0x7		// {команда PC} - послать ID канала

// идентификаторы ответов и посылок от девайса (бит PC_REQ = 0) - (биты 3..0 ID CAN кадра)
#define	DEV_IS_PRESENT			0x1		// {посылка девайса} - метка о присутствии девайса на шине (1 раз в сек)
#define NEW_VAR_OK				0x2		// {ответ девайса}   - старые переменные удалены / новые переменные приняты
#define CMD_VAR_NUM         	0x3     // {посылка девайса} - число переменных в пакете
#define NEW_ARR_OK				0x4		// {ответ девайса}   - старые массивы удалены / новые массивы приняты
#define CMD_ARR_NUM         	0x5     // {посылка девайса} - число массивов в пакете
#define CMD_ARR_SZ         		0x6     // {посылка девайса} - размер текущего массива в пакете
#define SCAN_EN_OK         		0x7     // {ответ девайса}   - разрешено/запрещено сканирование массива
#define FILE_SAVE_OK			0x8		// {ответ девайса}   - девайс готов на прием файла переменных
#define FILE_LOAD_OK			0x9		// {ответ девайса}   - девайс готов передавать файл переменных
#define SYNC_DATA_BEGIN			0xA		// {посылка девайса} - сигнал синхронизации для РС: начала передачи данных
#define SYNC_DATA_END			0xB		// {посылка девайса} - сигнал синхронизации для РС: конец передачи данных

// макросы установки битов команд\параметров в ID кадра расширенного пакета:
// первая строка макросов : очистка битовых полей
// вторая : установка битов по маске длины поля для предотвращения переполнения
#define	SET_ID_CMD(reg, id)		(reg &= ~(LEN_CMD << BIT_CMD));    \
                                (reg |= ((id) & LEN_CMD) << BIT_CMD) // ID команды/данных/массива
#define SET_ID_DEV(reg, id)		(reg &= ~(LEN_ID_DEV << BIT_ID_DEV));    \
                                (reg |= (id & LEN_ID_DEV) << BIT_ID_DEV)	// ID узла девайса
#define	SET_VAR_NUM(reg, par)	(reg &= ~(LEN_VAR_NUM << BIT_VAR_NUM));  \
                                (reg |= (par & LEN_VAR_NUM) << BIT_VAR_NUM)	// значение параметра команды
#define	SET_CMD_NAME(reg, id)   (reg &= ~(LEN_CMD_NAME << BIT_CMD_NAME));  \
                                (reg |= (id & LEN_CMD_NAME) << BIT_CMD_NAME)// имя(номер) команды
#define	SET_VAR_ID(reg, addr)   (reg &= ~(LEN_VAR_ID << BIT_VAR_ID));\
                                (reg |= (addr & LEN_VAR_ID) << BIT_VAR_ID)	// индекс переменной
#define	SET_VAR_EX(reg, en) 	(reg &= ~(LEN_VAR_EX << BIT_VAR_EX)); \
                                (reg |= (en & LEN_VAR_EX) << BIT_VAR_EX)	// разр./запр. сканирование массива
#define	SET_ARR_SZ(reg, size)  	(reg &= ~(LEN_ARR_SZ << BIT_ARR_SZ)); \
                                (reg |= (size & LEN_ARR_SZ) << BIT_ARR_SZ) 	// размер массива (байт)
#define	SET_ARR_NUM(reg, num)   (reg &= ~(LEN_ARR_NUM << BIT_ARR_NUM)); \
                                (reg |= (num & LEN_ARR_NUM) << BIT_ARR_NUM) // число сканируемых массивов
#define	SET_ARR_ID(reg, idx) 	(reg &= ~(LEN_ARR_ID << BIT_ARR_ID)); \
                                (reg |= (idx & LEN_ARR_ID) << BIT_ARR_ID) 	// индекс массива
#define	SET_ARR_EN(reg, en) 	(reg &= ~(LEN_ARR_EN << BIT_ARR_EN)); \
                                (reg |= (en & LEN_ARR_EN) << BIT_ARR_EN)	// разр./запр. сканирование массива
#define	SET_FILE_SZ(reg, size)	(reg &= ~(LEN_FILE_SZ << BIT_FILE_SZ));  \
                                (reg |= (size & LEN_FILE_SZ) << BIT_FILE_SZ)// размер переменных файла (байт)

// макросы получения битов команд\параметров в ID кадра расширенного пакета:
#define	GET_ID_CMD(reg)			((reg >> BIT_CMD) & LEN_CMD)                // идентификатор команды/данных/массива
#define	GET_ID_DEV(reg)			((reg >> BIT_ID_DEV) & LEN_ID_DEV)          // идентификатор узла
#define	GET_VAR_NUM(reg)		((reg >> BIT_VAR_NUM) & LEN_VAR_NUM)   		// номер параметра команды
#define	GET_CMD_NAME(reg)		((reg >> BIT_CMD_NAME) & LEN_CMD_NAME)      // имя(номер) команды
// #define	GET_ADDR_PAR(reg) 		((reg >> BIT_ADDR_PAR) & LEN_ADDR_PAR) 		// адрес переменной
#define	GET_VAR_ID(reg) 		((reg >> BIT_VAR_ID) & LEN_VAR_ID)          // индекс переменной
#define	GET_ARR_SZ(reg) 		((reg >> BIT_ARR_SZ) & LEN_ARR_SZ) 			// размер массива
#define	GET_ARR_NUM(reg)        ((reg >> BIT_ARR_NUM) & LEN_ARR_NUM)        // число сканируемых массивов
#define	GET_ARR_ID(reg)         ((reg >> BIT_ARR_ID) & LEN_ARR_ID)        	// индекс массива
#define	GET_ARR_EN(reg)         ((reg >> BIT_ARR_EN) & LEN_ARR_EN)          // разр./запр. сканирование массива
#define	GET_FILE_SZ(reg)        ((reg >> BIT_FILE_SZ) & LEN_FILE_SZ)        // размер файла переменных (байт)

// #define	BASE_ADDRESS    0x20000000   // базовый адрес переменных в оперативной памяти процессора (RAM)

// имя и VID_PID USB-CAN преобазователя
#define	m_sIXXAT_vidpid "VID_08D8&PID_0002"
#define	m_sIXXAT_name   "VCI4 USB-to-CAN compact"

    // структура для расчета битрейта
    typedef struct {
        uint8_t BT0;
        uint8_t BT1;
        QString btName;
    } BRS_t;

#define CAN_BRS_NUM	9
    BRS_t	m_CAN_BRS[CAN_BRS_NUM];
    int     m_curBitRates = 0;

    IXXATCmd m_ixxatCmd;

    typedef struct {
        VCIDEVICEINFO   deviceInfo;
        HANDLE          hEnum;
        HANDLE          hDevice;
        HANDLE          hCanChn;
        HANDLE          hCanCtrl;
    } devHandle_t;

    devHandle_t  m_devHandles;
    CANMSG       m_CanMsg;

    typedef struct {
        uint32_t    set;
        bool        isPresent;
    } chan_t;

    typedef struct {
        uint32_t    preUnit;    // предыдущий подключенный канал
        uint32_t    curUnit;    // текущий подключенный канал
        uint32_t    nUnits;     // число подключенных каналов
        // bool        first;      // флаг первого включения для определения свойств канала
        chan_t      chan[NUMCHANNELMAX];    // все каналы
    } chProp_t;
    chProp_t    chProp;

    std::atomic<bool>	m_portIsOpened      = false; // флаг: порт CAN открыт
    std::atomic<bool>	m_devIsConnected    = false; // флаг: девайс подключен
    std::atomic<bool>	m_isNewVarOK        = false; // флаг:
    std::atomic<bool>   m_isNewArrOK        = false; // флаг:
    std::atomic<bool>   m_isScanOK          = false; // флаг:
    std::atomic<bool>   m_isVarSave2FlashOK = false; // флаг:
    std::atomic<bool>   m_isVarLoad2HostOK  = false; // флаг:

    uint32_t    m_varFileSize    = 0;

    QByteArray m_bufVarFileData{};
    uint32_t m_varFileCRC32 = 0;

    void RegDevice(HANDLE hwnd);
    void initDlg();
    void ixxat_deinit();
    bool checkIXXATConnect();
    void setMainWindowEvent(void *message);
    void showErrorBox(LONG err, QString op);
    bool ixxat_init(bool fExtend);
    bool saveVarFileToUnitBoard(uint32_t unit, QFile &file);
    void loadVarFileFromUnitBoard(uint32_t unit);
    void loadVarFileFromAllBoard(unitProp_t &prop);
    void clearRequestUnit(uint32_t unit);
    void clearAllRequest();
    void getDevId();
    bool sendCommand(uint8_t cmdID, uint32_t unitID, uint32_t par1, uint32_t par2);
    void sendData(uint8_t cmdID, uint32_t unitID);
    bool sendMsg(CANMSG *CanMsg);
    void getMessageError();
    void createReadThread();
    void closeReadThread();

signals:
    void signalSendSelParamToDevice();

public slots:
    void slotChangeDevice(MSG* msg);
    void slotVarFileError(int unit);
    // void slotChannelChange();
    void slotChannelChange(bool en);
    void slotChannelDone(uint32_t unit);

private slots:
    void on_BTN_CHECK_CAN_ON_clicked();

private:
    Ui::DlgIXXAT *ui;
    // bool nativeEvent(const QByteArray & eventType ,void *message, long *result);
};

#endif // DLGIXXAT_H
