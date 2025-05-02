
#include "mystyles.h"

QString StyleHelper::getLabelStyleSheet(QString color)
{
    return QString(
    "QLabel {"
        "color: %1;"
        "border: none;"
    "}").arg(color);
}

QString StyleHelper::getPushButtonStyleSheet()
{
    return
    "QPushButton {"
        "background-color: lightgray;"
        "border-style: outset;"
        "border-width: 2px;"
        "border-radius: 4px;"
        "border-color: beige;"
        "padding: 6px;"
    "}"
    "QPushButton:hover {"
        "background-color: rgb(100, 220, 220);"
        "border-style: outset;"
    "}"
    "QPushButton:pressed {"
        "background-color: rgb(0, 110, 200);"
        "border-style: inset;"
    "}";
}

QString StyleHelper::getComboBoxDelegateStyleSheet()
{
    return
    "QComboBox::drop-down {"
        "subcontrol-position: center;"
    "}"

    "QComboBox QAbstractItemView {"
        "background: rgb(240,240,170);"
        "border: 1px solid rgb(50,50,250);"
        "color: rgb(50,50,250);"
        "selection-background-color: rgb(80, 80, 250);"
        "font-size: 12px;"
        "font-weight: bold;"

    "}";

        //"selection-color: rgb(50,50,250);"
}

QString StyleHelper::getProgressBarStyleSheet(QString color)
{
    return QString(
    "QProgressBar {"
        "border: 1px solid gray;"
        "border-radius: 4px;"
        "text-align: center;"
    "}"

    "QProgressBar::chunk {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 %1, stop: 1 #eeeeee);"
    "}").arg(color);
}

QString StyleHelper::getCheckBoxBarStyleSheet()
{
    return
    "QCheckBox {"
        "spacing: 5px;"
    "}"

    "QCheckBox::indicator {"
        "width: 13px;"
        "height: 13px;"
    "}"

    "QCheckBox::indicator:unchecked {"
        "image: url(:/res/checkbox_unchecked.png);"
    "}"

    "QCheckBox::indicator:unchecked:hover {"
        "image: url(:/res/checkbox_unchecked_hover.png);"
    "}"

    "QCheckBox::indicator:unchecked:pressed {"
        "image: url(:/res/checkbox_unchecked_pressed.png);"
    "}"

    "QCheckBox::indicator:checked {"
        "image: url(:/res/checkbox_checked.png);"
    "}"

    "QCheckBox::indicator:checked:hover {"
        "image: url(:/res/checkbox_checked_hover.png);"
    "}"

    "QCheckBox::indicator:checked:pressed {"
        "image: url(:/res/checkbox_checked_pressed.png);"
    "}";

    //--------------------------------------------
    /*"QCheckBox::indicator:indeterminate:hover {"
        "image: url(:/res/checkbox_indeterminate_hover.png);"
    "}
    "QCheckBox::indicator:indeterminate:pressed {"
        "image: url(:/res/checkbox_indeterminate_pressed.png);"
    "}"*/
}

QString StyleHelper::getGroupBoxStyleSheet()
{
    return
    "QGroupBox { "
        "background-color: transparent; "
        "border-radius: 4px; "
        "border: 1px solid rgb(60%,70%,100%); "
        "margin-top: 6px; "
        "margin-left: 0px; "
    "}"

    "QGroupBox::title { "
        "subcontrol-origin: margin; "
        "subcontrol-position: top center; "
        "padding: 0 2px; "
        "color: rgb(20%,40%,100%); "
    "}"

    "QGroupBox::title:disabled { "
          "subcontrol-origin: margin; "
          "subcontrol-position: top center; "
          "padding: 0 2px; "
          "color: rgb(50%,50%,50%); "
    "}"

    "QGroupBox::indicator { "
        "width: 13px; "
        "height: 13px; "
    "}"

     "QGroupBox::indicator:unchecked {"
        "image: url(:/res/checkbox_unchecked.png);"
    "}"

    "QGroupBox::indicator:unchecked:hover {"
        "image: url(:/res/checkbox_unchecked_hover.png);"
    "}"

    "QGroupBox::indicator:unchecked:pressed {"
        "image: url(:/res/checkbox_unchecked_pressed.png);"
    "}"

    "QGroupBox::indicator:checked {"
        "image: url(:/res/checkbox_checked.png);"
    "}"

    "QGroupBox::indicator:checked:hover {"
        "image: url(:/res/checkbox_checked_hover.png);"
    "}"

    "QGroupBox::indicator:checked:pressed {"
        "image: url(:/res/checkbox_checked_pressed.png);"
    "}";

    //------------------------------------------------
    /*"QCheckBox::indicator:indeterminate:hover {"
        "image: url(:/res/checkbox_indeterminate_hover.png);"
    "}"
    "QCheckBox::indicator:indeterminate:pressed {"
        "image: url(:/res/checkbox_indeterminate_pressed.png);"
    "}"*/
}

QString StyleHelper::getDockWidgetStyleSheet()
{
    return QString(
    "QDockWidget {"
        "border: 1px solid red;"
        "border-radius: 4px;"
        "titlebar-normal-icon: url(:/res/settings.png);"
        "icon-size: 20px;"
        "font-weight: bold;"
        "font-size: 8pt;"
        "color: darkmagenta;"
        "height: 25px;"
    "}"

    "QDockWidget::float-button{"
        "padding-left: 14px;"
        "subcontrol-position: left;"
        "subcontrol-origin: margin;"
        "position: absolute;"
        "top: 0px; left: 2px; bottom: 0px;"
        "width: 14px;"
    "}"

    "QDockWidget::title {"
        /*"border-radius: 4px;"*/
        "text-align: center;" /* выравнивание текста */
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #eee, stop: 0.4 #ccc,"
            "stop: 0.7 #aaa, stop: 1.0 #999);"
        "padding-left: 14px;"
    "}");
}

QString StyleHelper::getHSliderStyleSheet()
{
    return QString(
    "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::sub-page:horizontal {"  /* Область между ручкой (ползунком) и левом ПАЗА */
        "background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 1,"
            "stop: 0 #bbf, stop: 1 #55f);"
        "border: 1px solid #777;"
        "height: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::add-page:horizontal {"  /* Область между ручкой (ползунком) и правом ПАЗА */
        "background: #fff;"
        "border: 1px solid #777;"
        "height: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::handle:horizontal {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "stop:0 #eee, stop:1 #ccc);"
        "border: 1px solid #777;"
        "width: 13px;"
        "margin-top: -2px;"
        "margin-bottom: -2px;"
        "border-radius: 4px;"
    "}"

    "QSlider::handle:horizontal:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "stop:0 #fff, stop:1 #ddd);"
        "border: 1px solid #444;"
        "border-radius: 4px;"
    "}"

    "QSlider::sub-page:horizontal:disabled {"
        "background: #bbb;"
        "border-color: #999;"
    "}"

    "QSlider::add-page:horizontal:disabled {"
        "background: #eee;"
        "border-color: #999;"
    "}"

    "QSlider::handle:horizontal:disabled {"
        "background: #eee;"
        "border: 1px solid #aaa;"
        "border-radius: 4px;"
    "}");
}

QString StyleHelper::getVSliderStyleSheet()
{
    return QString(
    "QSlider::groove:vertical {"    /* ПАЗ слайдера */
        "border: 1px solid #bbb;"
        "background: white;"
        "width: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::sub-page:vertical {"  /* Область между ручкой (ползунком) и верхом ПАЗА */
        "background: #fff;"
        "border: 1px solid #777;"
        "width: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::add-page:vertical {"  /* Область между ручкой (ползунком) и низом ПАЗА */
        "background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 1,"
            "stop: 0 #bbf, stop: 1 #55f);"
        "border: 1px solid #777;"
        "width: 10px;"
        "border-radius: 4px;"
    "}"

    "QSlider::handle:vertical {"    /* Ручка (ползунок) */
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "stop:0 #eee, stop:1 #ccc);"
        "border: 1px solid #777;"
        "height: 13px;"
        "margin-left: -2px;"
        "margin-right: -2px;"
        "border-radius: 4px;"
    "}"

    "QSlider::handle:vertical:hover {"  /* Курсор мыши наведен на элемент handle */
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "stop:0 #fff, stop:1 #ddd);"
        "border: 1px solid #444;"
        "border-radius: 4px;"
    "}"

    "QSlider::sub-page:vertical:disabled {"
        "background: #bbb;"
        "border-color: #999;"
    "}"

    "QSlider::add-page:vertical:disabled {"
        "background: #eee;"
        "border-color: #999;"
    "}"

    "QSlider::handle:vertical:disabled {"
        "background: #eee;"
        "border: 1px solid #aaa;"
        "border-radius: 4px;"
    "}");
}

QString StyleHelper::getTableWidgetStyleSheet()
{
    //        "QTableWidget { "
    //        "selection-background-color: qlineargradient(x1: 0, y1: 0, x2: 0.8, y2: 0.8,"
    //          "stop: 0 #aaaaaa, stop: 1 #eeeeee);"
    //        "}"

    return
    "QTableWidget QTableCornerButton::section {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "stop:0 #ddd, stop: 0.4 #999,"
            "stop: 0.6 #777, stop:1 #666);"
    "}"

    "QHeaderView::section {"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "stop:0 #ddd, stop: 0.4 #999,"
            "stop: 0.6 #777, stop:1 #666);"
        "color: white;"
        "padding-left: 4px;"
        "border: 1px solid #666;"
    "}";
}

QString StyleHelper::getTabWidgetStyleSheet(int pos)
{
    QString str1,str2,str3;
    switch(pos)
    {
    case HORT:
        str1 = "border-top-left-radius";
        str2 = "border-top-right-radius";
        str3 = "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
               "stop: 0 #cacaca, stop: 0.4 #f4f4f4,"
               "stop: 0.5 #e7e7e7, stop: 1.0 #aaaaaa);";
        break;
    case VERL:
        str1 = "border-top-left-radius";
        str2 = "border-bottom-left-radius";
        str3 = "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
               "stop: 0 #cacaca, stop: 0.4 #f4f4f4,"
               "stop: 0.5 #e7e7e7, stop: 1.0 #aaaaaa);";
        break;
    case VERR:
        str1 = "border-top-right-radius";
        str2 = "border-bottom-right-radius";
        str3 = "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
               "stop: 0 #cacaca, stop: 0.4 #f4f4f4,"
               "stop: 0.5 #e7e7e7, stop: 1.0 #aaaaaa);";
        break;
    }

    return QString(
    "QTabWidget::pane {" /* The tab widget frame */
                "border-top: 1px solid #C2C7CB;"
                "border-bottom: 2px solid #C2C7CB;"
                "border-left: 1px solid #C2C7CB;"
                "border-right: 2px solid #C2C7CB;"
    "}"

    "QTabWidget::tab-bar {"
        "left: 0px;" /* move to the right by 5px */
    "}"

    "QTabBar::tab:top, QTabBar::tab:bottom {"
        "min-width: 8ex;"
        "margin-right: 0px;"
        "padding: 2px 2px 2px 2px;"
    "}"

    "QTabBar::tab:left, QTabBar::tab:right {"
        "min-height: 8ex;"
        "margin-bottom: 0px;"
        "padding: 0px 0px 0px 0px;"
    "}"

    "QTabBar::tab {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                    "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                                    "stop: 0.5 #D8D8D8, stop: 1.0 #a3a3a3);"
        "border: 1px solid #C4C4C3;"
        "border-bottom-color: #C2C7CB;" /* same as the pane color */
        "%1: 8px;"
        "%2: 8px;"
        "min-width: 8ex;"
        "padding: 1px;"
    "}"

    "QTabBar::tab:selected, QTabBar::tab:hover {"
        "%3"
    "}"

        "QTabBar::tab:left:selected {"
                "margin-right: 3px;"
        "}"

        "QTabBar::tab:left:!selected {"
                "margin-right: 6px;"
        "}"

        "QTabBar::tab:right:selected {"
                "margin-left: 3px;"
        "}"

        "QTabBar::tab:right:!selected {"
                "margin-left: 6px;"
        "}"

    "QTabBar::tab:selected {"
        "border-color: #9B9B9B;"
        "border-bottom-color: #C2C7CB;" /* same as pane color */
    "}"

    "QTabBar::tab:!selected {"
        "margin-top: 2px;" /* make non-selected tabs look smaller */
    "}").arg(str1).arg(str2).arg(str3);
}

QString StyleHelper::getToolBoxStyleSheet(QString color)
{
    return QString(
    "QToolBox::tab {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #cde, stop: 0.4 #bbb,"
            "stop: 0.7 #999, stop: 1.0 #777);"
        "border-radius: 6px;"
        "color: %1;"
    "}"

    "QToolBox::tab:selected {"
        "font: bold;"
        "color: %1;"
    "}").arg(color);
}

void StyleHelper::setLabelEnable(QLabel *l, QColor color, bool en)
{
    QString style;
    if(en)
        style = getLabelStyleSheet(color.name());
    else
        style = getLabelStyleSheet("gray");
    l->setStyleSheet(style);
}
