#include "dlghowtouse.h"
#include "ui_dlghowtouse.h"
#include <QFile>
#include <QtPrintSupport>
#include <QPageSize>
#include <QPainter>

/*!!!!!!!!!!!!!!!!!!!!!!!!!!
 * в *.pro файл прописать
 * QT += printsupport
 *!!!!!!!!!!!!!!!!!!!!!!!!*/

DlgHowToUse::DlgHowToUse(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgHowToUse)
{
    ui->setupUi(this);

    this->setWindowTitle(TITLE);

    ui->listWidget->setWrapping(false); // если true, вертикального скролла нет
    ui->listWidget->setWordWrap(true);  // при скролле перенос слов
    ui->listWidget->setFocusPolicy(Qt::NoFocus); // не показывать обрамляющий прямоугольник

    QFile file(":/res/Manual.txt");
    QTextStream stream(&file);
    if(file.open(QIODevice::ReadOnly)) {
        QString sBuff = file.readAll();
        QStringList list = QStringList() << sBuff;
        ui->listWidget->addItems(list);
        file.close();
    }
}

DlgHowToUse::~DlgHowToUse()
{
    delete ui;
}

void DlgHowToUse::printing()
{
    int cnt = ui->listWidget->count();
    QString str;
    QTextStream streamT(&str);
    QString	buff;
    for(int i = 0; i < cnt; ++i)
        streamT << ui->listWidget->item(i)->text();

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName("QVarMon_Manual.pdf");
    // разрешение принтера (пиксела на мм)
    // (при разрешении 1200 точек/дюйм => 47.2 пиксела на мм)
    double res = (double)(printer.resolution() / 25.4);
    // высота страницы
    int pageHight = printer.pageRect(QPrinter::Unit::DevicePixel).height();
    // высота страницы
    int pageWidht = printer.pageRect(QPrinter::Unit::DevicePixel).width();

    QPainter painter(&printer);
    QFont fnt("Arial", 11, QFont::Medium);
    painter.setFont(fnt);
    QFontMetrics fm = painter.fontMetrics();
    const double hight = fm.height();

    int rowX = (int)(20 * res);
    int rowY = (int)(2 * hight);
    painter.begin(&printer);
    painter.drawText(rowX * 2, rowY, TITLE + "\r\n");
    rowY += hight;
    painter.drawText(rowX, rowY, "\r\n");
    rowY += hight;

    int page = 0;
    while(!streamT.atEnd()) {
        buff = streamT.readLine();
        painter.drawText(rowX, rowY, buff);
        rowY += hight;
        if(rowY >= pageHight - 2 * hight) {
            ++page;
            painter.drawText(pageWidht / 2, pageHight - hight, "- " + QString::number(page) + " -");
            rowY = 2 * hight;
            printer.newPage();
        }
    }
    // номер последней страницы
    painter.drawText(pageWidht / 2, pageHight - hight, "- " + QString::number(++page) + " -");
    painter.end();
}

void DlgHowToUse::on_BTN_PRINT_clicked()
{
    printing();
}

