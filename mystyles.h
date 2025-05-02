#ifndef MYSTYLES_H
#define MYSTYLES_H

#include <QColor>
#include <QString>
#include <QLabel>

#define HORT 0
#define HORB 1
#define VERL 2
#define VERR 3

class StyleHelper
{
public:
    static QString getLabelStyleSheet(QString color);
    static QString getComboBoxDelegateStyleSheet();
    static QString getPushButtonStyleSheet();
    static QString getProgressBarStyleSheet(QString color);
    static QString getCheckBoxBarStyleSheet();
    static QString getGroupBoxStyleSheet();
    static QString getDockWidgetStyleSheet();
    static QString getHSliderStyleSheet();
    static QString getVSliderStyleSheet();
    static QString getTableWidgetStyleSheet();
    static QString getTabWidgetStyleSheet(int pos);
    static QString getToolBoxStyleSheet(QString color);

    void setLabelEnable(QLabel *l, QColor color, bool en);
};

#endif // MYSTYLES_H
