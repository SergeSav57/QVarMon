#ifndef DLGVARSEL_H
#define DLGVARSEL_H

#include <QDialog>
#include <QTableWidgetItem>

#include "common.h"
// #include "tblallmodel.h"
#include "tblselmodel.h"

namespace Ui {
class DlgVarSel;
}

class DlgVarSel : public QDialog
{
    Q_OBJECT

public:
    explicit DlgVarSel(QWidget *parent = nullptr);
    ~DlgVarSel();

    // таблица "Все параметры"
    TableSelModel   *m_modelAll{};
    QTableView      *m_tableAll{};
    QStringList     m_listAll{};    // для списка имен итемов в каждой ячейке таблицы
    // таблица "Выбранные параметры"
    TableSelModel   *m_modelSel{};
    QTableView      *m_tableSel{};
    QStringList     m_listSel{};


    // списки всех параметров для мониторинга
    QVector<var_t>	m_VarAllGlobalVector;   // список всех переменных, загруженных из файла
    QVector<var_t>	m_VarSelGlobalVector;	// список переменных, выбранных для мониторинга для всех вкладок
                                            // (для каждой вкладки есть свое хранилище в ее классе)
    // списки параметров для массивов
    QVector<var_t>	m_ArrAllGlobalVector;   // список всех массивов переменных, загруженных из файла
    QVector<var_t>	m_ArrSelGlobalVector;   // рабочий список всех массивов переменных, выбранных для мониторинга
    QVector<var_t>	m_ArrSelGlobalVectorTmp;// временный список всех массивов переменных, выбранных для мониторинга

    // списки параметров для графиков
    QVector<var_t>	m_chartSelVectorTmp;    // временный список переменных, выбранных для построения графиков

    // списки параметров для записи
    QVector<var_t>	m_recSelVector;         // рабочий список переменных, выбранных для записи в *.mon файл
    QVector<var_t>	m_recSelVectorTmp;      // временный список переменных, выбранных для записи в *.mon файл

    // списки параметров при поиске
    QVector<var_t>  m_findVector;           // список переменных, выбранных в процессе поиска

    // bool            m_fSetNewVar = false;       // флаг блокировки слота обработки данных
    bool            m_fButtonsUpdateEn = true;  // флаг разрешения на перерисовку кнопок

    // регистр флагов текущих операций (флаги в файле "common.h")
    //===========================================================
    // (загрузка нового файла, работа со старыми параметрами,
    // вставка выбранных параметров в лист и т.д.)
    uint32_t        m_regOPERATION = 0;
    // для определения выделенных строк используем QItemSelectionModel
    QItemSelectionModel *itemSelectionModelAll{};
    QItemSelectionModel *itemSelectionModelSel{};


#define ALL_PARAM    0
#define ALL_ARRAY    1
#define CHART_PARAM  2
#define REC_PARAM    3

#define CHECK_FLAG(f)    ((f == true)?(Qt::Checked):(Qt::Unchecked))


    typedef struct { // структура для получения текущих списков ALL и SEL
        QVector<var_t> *vAll;
        QVector<var_t> *vSel;
    } DestSrc_t;

    void showDlgSel();
    // Работа с таблицами (инициализация, заполнение)
    void initAllParamTable();
    void fillAllParamTable();
    void unCheckedAllParamTable(QVector<var_t> &delVec);
    void initSelParamTable();
    void fillSelParamTable();
    void checkStateAllParamTable();
    // Фильтр
    void filterClr();
    // Установки режимов выбора параметров
    void getDestSrcVectors(DestSrc_t *ds);
    void setAllParam();
    void setAllArray();
    void setRecordParam();
    void setChartParam();
    void setButtonsEn();
    // Управление ComboBox'сами
    void setComboTabsName(QString tabName);
    void updateComboTabsName(int index, QString tabName);
    void setComboTabsIndex(int index);
    void removeComboTabsIndex(int index);
    void setComboParamItemsEn();
    void setComboParamIndex(int index);
    void setComboParam(int index);
    // снятие/установка выбора строки
    void selectTableRow(QTableView *table, bool select, const QModelIndex &index);
    // Удаление параметра из списка (вектора) выбранных для вкладок
    void eraseParam(QVector<var_t> &vDest,var_t vDel);
    // Установка и снятие признаков выбора параметров
    // для мониторинга для разных режимов
    void setSelectedFlagParam(QVector<var_t> *vec, int index, bool flag);
    bool getSelectedFlagParam(var_t &var);
    bool getSelectedStateParam(QTableView *table, const QModelIndex &index);

private:

public slots:
    void slotSortByColumn(int nColumn);
    void slotTableClear();
    void on_BTN_OK_clicked();

private slots:
    void checkSelParamGlobal();
    // Кнопки перемещения
    void on_BTN_SELALL_clicked();
    void on_BTN_DESELALL_clicked();
    void on_BTN_SELPAR_clicked();
    void on_BTN_DESELPAR_clicked();
    void on_BTN_UP_clicked();
    void on_BTN_DOWN_clicked();
    // Фильтр
    void on_EDIT_FILTER_textEdited(const QString &arg1);
    void on_BTN_CLR_FILTER_clicked();
    void on_COMBOBOX_PARAM_activated(int index);
    void on_COMBOBOX_TABS_activated(int index);
    // Действия мыши в таблицах
    void on_TABLE_ALLPAR_clicked(const QModelIndex &index);
    void on_TABLE_ALLPAR_pressed(const QModelIndex &index);
    void on_TABLE_SELPAR_pressed(const QModelIndex &index);
    // Работа с таблицами (инициализация, заполнение)
    void on_TABLE_ALLPAR_itemSelectionChanged();

signals:
    void signalBtnStartRecEn(bool en);
    void signalBtnSelRecParamEn(bool en);
    void signalSendSelParamToDevice();

protected:
    bool eventFilter(QObject *, QEvent *event) override;

private:
    Ui::DlgVarSel *ui;
};

#endif // DLGVARSEL_H
