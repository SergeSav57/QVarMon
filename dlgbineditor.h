#ifndef DLGBINEDITOR_H
#define DLGBINEDITOR_H

#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QMutex>
#include "common.h"

namespace Ui {
class DlgBinEditor;
}

class DlgBinEditor;

class DlgBinEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgBinEditor(QWidget *parent = nullptr);
    ~DlgBinEditor();

public:
    var_t           m_var{};        // копия структуры аттрибутов переменной в рабочем листе WORK класса DlgVarSel
    DlgBinEditor    *m_dlg{};       // фактически это тот же самый диалог (this), но для идентификации его в списке открытых
    int             m_varPos = -1;  // номер позиции переменной в векторе параметров

    QPoint      m_mousePnt{};           // позиция мыши при перемещении окна
    bool        m_moveEn = false;       // разрешить/запретить перемещение окна мышью
    QColor      m_colorLight;
    QColor      m_colorDark;
    QMutex      m_mutexBlink;
    QTimer      *m_timerUpdate;


    typedef struct
    {
        uint32_t    x;          // координата left
        uint32_t    y;          // координата top
        uint32_t    cx;         // ширина
        uint32_t    cy;         // высота
        QLabel      *wndBit;	// окна вывода состояние битов
        QLabel      *wndPos;	// окна вывода позиции битов
    } bits_t;
    QVector<bits_t> m_bitLabelsVector{};

    void UpdateWndBin();
    void setBitsColor();
    void UpdateWndDecHex();
    void CreateInterface();
    void Blink();
    void ShowBinEditor(var_t &var);
    void UpdateBinEditData(var_t &var);

private slots:
    void on_BTN_CLOSE_clicked();
    void slotTimerAlarmUpdate();

private:
    Ui::DlgBinEditor *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // DLGBINEDITOR_H
