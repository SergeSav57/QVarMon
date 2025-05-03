#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QDir>
#include <QStyleFactory>
#include <QPalette>
// #include <QSharedMemory>
#include <QLoggingCategory>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QTextCodec>
#else
    #include <QStringConverter> // class provides a base class for encoding and decoding text (Qt6).
#endif

// указатель на файл логирования
QScopedPointer<QFile>   m_logFile;

// Объявление обработчика для файла логирования
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QSharedMemory sharedMemory;
    // sharedMemory.setKey("QVarMon-Key");
    // if(sharedMemory.create(1) == false)
    // {
    //     QMessageBox::warning(NULL, "Warning!", "Another instance already running!");
    //     a.exit(); // exit already a process running
    //     return 0;
    // }

    // a.setStyle(QStyleFactory::create("Fusion"));
    // a.setStyle(QStyleFactory::create("windowsvista"));
    a.setStyle(QStyleFactory::create("Windows"));
    setlocale(LC_ALL, "Russian");

    //// test
    // QPalette palette = a.palette();
    // palette.setColor(QPalette::WindowText, Qt::red);
    // a.setPalette(palette);

    // Устанавливаем файл логирования,
    // определим, какой используем путь для файла
    // QString CurrentDir = QDir::currentPath() + "/QVarMon.log";
    QString CurrentDir = "d:/WORKSPACE_GD/Qt_proj/QVarMon.log";
    m_logFile.reset(new QFile(CurrentDir));
    // Открываем файл логирования
    m_logFile.data()->open(QFile::Append | QFile::Text);
    // Устанавливаем обработчик
    qInstallMessageHandler(messageHandler);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    QTextCodec *codec = QTextCodec::codecForName("CP1251"); //("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
#else
    // вроде пока не требуется - так все выводится корректно
#endif

    MainWindow w;
    w.show();
    return a.exec();
}

// Реализация обработчика
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Открываем поток записи в файл
    QTextStream out(m_logFile.data());
    // Записываем дату записи
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd | hh:mm:ss.zzz | ");
    // По типу определяем, к какому уровню относится сообщение
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    // Записываем в вывод категорию сообщения и само сообщение
    out << context.category << ":" << msg << "\n";//Qt::endl;
    out.flush();    // Очищаем буферизированные данные
}
