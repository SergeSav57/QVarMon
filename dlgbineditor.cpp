#include <QBoxLayout>
#include <QLine>
#include <QFontMetrics>
#include <QTimer>
#include "dlgbineditor.h"
#include "ui_dlgbineditor.h"
#include "mainwindow.h"

MainWindow  *m_mainWnd_4BinEdit{};

DlgBinEditor::DlgBinEditor(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgBinEditor)
{
    ui->setupUi(this);
    m_mainWnd_4BinEdit = static_cast<MainWindow *>(parent);
    m_mousePnt = QPoint(0,0);
    m_colorLight = QColor(0, 120, 210);
    m_colorDark = QColor(150, 150, 150);

    m_timerUpdate = new QTimer(this);
    connect(m_timerUpdate, &QTimer::timeout, this, &DlgBinEditor::slotTimerAlarmUpdate);
    m_timerUpdate->start(100); // запустим таймер
}

DlgBinEditor::~DlgBinEditor()
{
    delete ui;
}

// ф-ции мыши для перемещения окна без заголовка
void DlgBinEditor::mousePressEvent(QMouseEvent *event)
{
    m_mousePnt = event->pos();
    m_moveEn = true;
}

void DlgBinEditor::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePnt = event->pos();
    m_moveEn = false;
}

void DlgBinEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if(m_moveEn) {
			QPoint diff = event->pos() - m_mousePnt;
			QPoint newpos = this->pos() + diff;
			this->move(newpos);
		}
    }
}

void DlgBinEditor::slotTimerAlarmUpdate()
{
    UpdateWndBin();
    UpdateWndDecHex();
    this->update();
}

//------------------------------------------------
struct { // линия разделителя BIN кода и номеров разрядов
    QPoint begin1{0,0};
    QPoint end1{0,0};
    QPoint begin2{0,0};
    QPoint end2{0,0};
    // цвета для имитации выпуклости линии
    QColor color1 = QColor(140,140,140,255);
    QColor color2 = QColor(253,253,253,255);
} separator;
//------------------------------------------------

void DlgBinEditor::paintEvent(QPaintEvent */*event*/)
{
    QPainter p(this);
    // зальем всю область окна с обрамляющим прямоугольником (рамка окна)
    QRect rect = this->rect();
    p.setBrush(QBrush(QColor(210,220,220,100), Qt::SolidPattern));
    // обрамляющий прямоугольник (рамка окна)
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.bottom() - 1);
    p.setPen(QColor(150,150,180,255));
    p.drawRect(rect);
    // линия-разделитель
    p.setPen(QColor(separator.color1));
    p.drawLine(separator.begin1, separator.end1);
    p.setPen(QColor(separator.color2));
    p.drawLine(separator.begin2, separator.end2);
}

#define EXPB(x) (31 - x) // исключение незначащих разрядов

inline void DlgBinEditor::UpdateWndBin()
{
    for (uint32_t i = 32 - m_var.varType.typeSize * 8; i < 32; i++) { // незначащие разряды не заполняем
        uint32_t val = (uint32_t) m_var.varData.varValueSD.toUInt();
        if (val & (1 << EXPB(i)))
            m_bitLabelsVector[i].wndBit->setNum(1);
        else
            m_bitLabelsVector[i].wndBit->setNum(0);
    }
}

inline void DlgBinEditor::setBitsColor()
{
    for(quint32 i= 0; i < m_var.varType.typeSize * 8; ++i)
        m_bitLabelsVector[EXPB(i)].wndBit->setStyleSheet(QString("color: %1").arg(m_colorLight.name()));
    for (quint32 i = m_var.varType.typeSize * 8; i < 32; i++)
        m_bitLabelsVector[EXPB(i)].wndBit->setStyleSheet(QString("color: %1").arg(m_colorDark.name()));
}

inline void DlgBinEditor::UpdateWndDecHex()
{   // раздвинем триады десятичного числа (т.е.кратно тысяче)
    QString strDec = m_var.varData.varValueSD;
    int len = strDec.length();
    for (int i = len - 3; i >= 0; i -= 3) {
        strDec.insert(i, QString(" "));
    }
    strDec = QString("DEC :  %1"). arg(strDec);

    // раздвинем тетрады шестнадцатичного значения числа (т.е.кратно 2-м байтам)
    // со вставкой незначащих нулей
    QString strHex = m_var.varData.varValueSH;
    len = strHex.length();
    switch (m_var.varType.typeID) {
    case TYPE_INT8:
    case TYPE_UINT8:
        switch (len) {
        case 1: strHex = QString("0%1").arg(strHex); break;
        case 2: strHex = QString("%1") .arg(strHex); break;
        }
        break;

    case TYPE_INT16:
    case TYPE_UINT16:
        switch (len) {
        case 1: strHex = QString("000%1").arg(strHex); break;
        case 2: strHex = QString("00%1") .arg(strHex); break;
        case 3: strHex = QString("0%1")  .arg(strHex); break;
        case 4: strHex = QString("%1")   .arg(strHex); break;
        }
        break;

    case TYPE_INT32:
    case TYPE_UINT32:
        switch (len) {
        case 1: strHex = QString("0000000%1").arg(strHex); break;
        case 2: strHex = QString("000000%1") .arg(strHex); break;
        case 3: strHex = QString("00000%1")  .arg(strHex); break;
        case 4: strHex = QString("0000%1")   .arg(strHex); break;
        case 5: strHex = QString("000%1")    .arg(strHex); break;
        case 6: strHex = QString("00%1")     .arg(strHex); break;
        case 7: strHex = QString("0%1")      .arg(strHex); break;
        case 8: strHex = QString("%1")       .arg(strHex); break;
        }
        break;
    }
    len = strHex.length();
    for (int i = len - 4; i >= 0; i -= 4) {
        strHex.insert(i, QString("  "));
    }
    strHex = QString("HEX :  %1"). arg(strHex);

    ui->labelDec->setText(strDec);
    ui->labelHex->setText(strHex);
}

void DlgBinEditor::CreateInterface()
{
    QFont fnt11("Arial", 11, QFont::Medium);
    QFont fnt10("Arial", 10, QFont::Normal);
    QFont fnt9("Arial", 9, QFont::Normal);
    QFont fnt8("Arial", 8, QFont::Normal);

    ui->labelParamName->setFont(fnt11);
    ui->labelDec->setFont(fnt9);
    ui->labelHex->setFont(fnt9);
    ui->labelBin->setFont(fnt9);

    ui->labelParamName->setStyleSheet(QString("color: %1").arg(m_colorLight.name()));

    const int MAXLEN = 300;
    QFontMetricsF fm(fnt11);
    const qreal width = fm.horizontalAdvance(m_var.varID.varName);
    QString str;
    if(width > MAXLEN) { // если текст очень длинный, ставим точки ("...")
        QString cutString = fm.elidedText(m_var.varID.varName, Qt::ElideRight, MAXLEN);
        str = QString("%1 [%2]").arg(cutString, m_var.varType.altType);
    }
    else
        str = QString("%1 [%2]").arg(m_var.varID.varName, m_var.varType.altType);
    ui->labelParamName->setText(str);

    // текущие координаты
    int x0 = 8;		// начальная координата левой стороны QLabel
    int y0 = 26;	// начальная координата начала вывода по вертикали окна "DEC:"
    // постоянные размеры и координаты
    int dx = 40;	// приращение гориз.координаты для начала вывода символов битов
    int dy = 20;	// приращение верт.координаты для следующих QLabel
    // размеры символа бита
    int w = 8;		// ширина символа бита
    int h = 16;		// высота символа бита
    int s = 14;     // ширина пробела между тетрадами
    int dxPos = 2;
    int dyPos = 16;
    int wPos = 12;

    QRect r = ui->labelDec->rect();
    ui->labelDec->setGeometry(x0, y0, r.width(), r.height());
    r = ui->labelHex->rect();
    y0 += dy;
    ui->labelHex->setGeometry(x0, y0, r.width(), r.height());
    r = ui->labelBin->rect();
    y0 += dy;
    ui->labelBin->setGeometry(x0, y0, r.width(), r.height());
    r = this->rect();
    x0 += dx; // начало вывода символов битов
    // рисуем линию-разделитель
    r = this->rect();
    separator.begin1.setX(x0 - dxPos);
    separator.begin1.setY(y0 + h);
    separator.end1.setX(r.width() - 13);
    separator.end1.setY(y0 + h);
    separator.begin2.setX(separator.begin1.x());
    separator.begin2.setY(separator.begin1.y() -1);
    separator.end2.setX(separator.end1.x());
    separator.end2.setY(separator.end1.y() -1);

    m_bitLabelsVector.clear();
    bits_t bits;
    for(int i=31, j=0; i>=0; --i, ++j) {
        if(j > 0) { // разбивка двоичного отображения по тетрадам
            if ((j % 4) == 0) x0 += s;
            else              x0 += w;
        }
        bits.x = x0;
        bits.y = y0;
        bits.cx = w;
        bits.cy = h;

        bits.wndBit = new QLabel("",this);
        bits.wndBit->setFont(fnt10);
        bits.wndBit->setNum(0);
        bits.wndBit->setGeometry(QRect(bits.x, bits.y, bits.cx, bits.cy));

        bits.wndPos = new QLabel("",this);
        bits.wndPos->setFont(fnt8);
        if(i < 10) str = QString(" %1").arg(i);
        else       str = QString("%1").arg(i);
        if((j % 4 == 0) || ((j + 1) % 4 == 0)) {
            bits.wndPos->setText(str);
            bits.wndPos->setGeometry(QRect(bits.x - dxPos, bits.y + dyPos, wPos, bits.cy));
        }
        m_bitLabelsVector.emplace_back(bits);
    }
    setBitsColor();
    setUpdatesEnabled(true);
    UpdateWndBin();
    UpdateWndDecHex();
}

void DlgBinEditor::ShowBinEditor(var_t &var)
{
    m_var = var;
    CreateInterface();
    this->show();
}

void DlgBinEditor::on_BTN_CLOSE_clicked()
{
    this->destroy();
    // удалим окно из списка MainWindow->m_dlgBinEditVector
    m_mainWnd_4BinEdit->DeleteDlgBinEdit(this);
    delete this;
}

void DlgBinEditor::UpdateBinEditData(var_t &var)
{
    if(m_bitLabelsVector.empty())
        return;
    QMutexLocker locker(&m_mutexBlink); // блокируем, если мигаем окном
    m_var = var;
    // UpdateWndBin();
    // UpdateWndDecHex();
    // this->update();
}

void DlgBinEditor::Blink()
{
    QMutexLocker locker(&m_mutexBlink);
    QTimer::singleShot(0, this, &QWidget::hide);
    QTimer::singleShot(40, this, &QWidget::show);
    QTimer::singleShot(80, this, &QWidget::hide);
    QTimer::singleShot(120, this, &QWidget::show);
}
