#ifndef DLGTABLEWORK_H
#define DLGTABLEWORK_H

#include <QDialog>
// #include <QTableWidgetItem>
#include <QTableView>
#include <QComboBox>
#include <QTimer>
#include <QTime>
#include <QMutex>
#include "common.h"
#include "tblworkmodel.h"
#include "comboboxdelegate.h"
#include "common.h"

class MyTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit MyTabBar(QWidget *parent = nullptr);// :
        /*QTabBar( parent ); {
        setExpanding( false );
    }*/

    QRect m_rect;

private:
    // void paintEvent(QPaintEvent *event) {
    //     const int spacing = 2;
    //     int tabWidth = width() / count() - spacing;
    //     setStyleSheet( QString( "QTabBar::tab { width: %1px; }" ).arg( tabWidth ) );
    //     QTabBar::paintEvent( event );
    // }
    void paintEvent(QPaintEvent *event);
};

namespace Ui {
class DlgTableWork;
}

class DlgTableWork : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTableWork(QWidget *parent = nullptr);
    ~DlgTableWork();

    // MyTabBar            *m_tabBar{};
    QTabWidget          *m_tabWidget{};
    TableWorkModel      *m_model{};
    QTableView          *m_table{};           // таблица этой вкладки - в основном для передачи в модель
    myComboBoxDelegate  *m_comboDelegat{};

    QVector<var_t>      m_varSelVector{};		// рабочий список переменных для этой вкладки, выбранных для мониторинга
    QVector<var_t>      m_varSelVectorTmp{};    // временный список переменных для этой вкладки, выбранных для мониторинга
    QString             m_sTableName{};         // имя этой вкладки
    int                 m_globalSys = DECSYS;   // индекс установленной системы счисления для всей таблицы этой вкладки
    bool                m_fOpened = false;      // флаг открытия этой вкладки
    int                 m_index = 0;            // индекс этой вкладки в общем наборе
    bool                m_first = true;
    bool                m_resizing = false;
    QTimer              *m_timerUpdate{};
    QTimer              *m_timerResize{};
    QSize               m_wndSize {};
    QMutex              m_mutexVarData4Model{};

    QStringList         m_list{};


    typedef struct {
        bool newAttr;           // true, если в таблице изменилось число параметров или ее размер
        int  cntParams;         // число параметров(переменных) таблицы
        int  maxRowsByVert;     // макс.число строк таблицы, которое может уложиться по вертикали
        int  nBlocks;           // число параллельных блоков таблицы (подтаблиц)
        int  old_nBlocks;       // то же до изменения размеров таблицы
        int  nRows;             // число строк таблицы по вертикали по всем блокам
        int  old_nRows;         // то же до изменения размеров таблицы
        QVector<uint16_t> sz;   // размеры каждой секции
    } m_tableAttr_t;
    m_tableAttr_t   m_tableAttr;

    QModelIndex     m_indexNotUsed; // индекс начала не имеющей строк части таблицы
                                    // (она появляется при числе блоков таблицы больше 1
                                    // и числе переменных меньше, чем заполнение всего
                                    // последнего блока)

    typedef struct  {
        int maxRow;     // макс.число строк в блоке
        int curBlock;   // текущий блок
        int curRow;     // текущий номер строки в блоке
        int curCol;     // текущий номер колонки
    } tabStr_t;
    tabStr_t    tabStr;

public:
    bool tableClickOut();
    void initConnectsTable();
    void initWorkTableProperty();
    void initWorkTable();
    void fillDataModel();
    void updateDataWorkTable();
    void showBinEditor(QModelIndex &index);
    void showVarAddr();
    int  getTableVSize();
    void resizeHorHeader();
    QModelIndex getCurIndex(int i);

public slots:
    void slotCellSysComboActivated(int pos, QModelIndex &index);
    void slotItemClicked(const QModelIndex &index);

private slots:
    void slotHeaderColumnClicked(int nColumn);
    void slotComboHeaderActivated(int index);
    void slotTimerAlarmUpdate();
    void slotTimerAlarmResize();

signals:
    void signalCreateBinEditDlg(var_t &var, QPoint *pnt);
    void signalCreateVarAddrDlg(uint32_t, QPoint*);

protected:
    bool eventFilter(QObject *, QEvent *event) override;
    // void resizeEvent(QResizeEvent *event);

private:
    Ui::DlgTableWork *ui;
};

#endif // DLGTABLEWORK_H
