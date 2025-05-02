#include "comboboxdelegate.h"
#include "LoggingCategories.h"
#include "common.h"

#include <QComboBox>
#include <QAbstractItemView>
#include <QPainter>
#include <QLineEdit>
#include <QListView>
#include <QTableView>
#include <QStyleFactory>
#include "mystyles.h"

// bool myComboBoxDelegate::eventFilter(QObject *obj, QEvent *event)
// {
//     // QEvent::Type type = event->type();
//     // static int cnt = 0;
//     // qDebug(logInfo()) << cnt++ << "ComboBoxDelegate: event_type ="<<type;
//     // if(type == QEvent :: UpdateLater) {
//     //     qDebug(logInfo()) << "setHidden()";
//     //     // m_editor->setHidden(true);
//     // }
//     return QStyledItemDelegate::eventFilter(obj, event);
// }

myComboBoxDelegate::myComboBoxDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // qDebug(logInfo()) << "ComboBoxDelegate constructor";
}

QWidget *myComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{   // создаем наше поле ввода типа QComboBox

    // считываем текст в текущей ячейке
    QVariant currentText = index.model()->data(index, Qt::DisplayRole);
    if(currentText == SMB_FLT) // если текст "F", выход: переменная типа float
        return nullptr;

    QComboBox *editor = new QComboBox(parent);
    editor->setStyle(QStyleFactory::create("Windows"));
    QListView *view = new QListView(editor); // окно выпадающего списка
    view->setStyleSheet("QListView::item{height: 18px}");
    editor->setView(view);
    editor->addItems(QStringList() << "D" << "H" << "B");
    editor->setStyleSheet(StyleHelper::getComboBoxDelegateStyleSheet());
    editor->setEditable(false);
    editor->setGeometry(option.rect);
    editor->setMaxVisibleItems(3);
    editor->showPopup(); // сразу развернем комбобокс

    connect(editor, &QComboBox::textActivated,
            this, &myComboBoxDelegate::slotSetModelText);

    return editor;
}

void myComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{   // в этой функции на входе данные из модели и указатель на виджет редактора
    // qDebug(logInfo()) << "ComboBoxDelegate::setEditorData";
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString currentText = index.data(Qt::EditRole).toString();
    int pos = comboBox->findText(currentText);
    comboBox->setCurrentIndex(pos);
}

void myComboBoxDelegate::slotSetModelText(const QString &/*text*/)
{   // вызывается после выбора  пункта выпадающего списка
    QComboBox *comboBox = (QComboBox *)sender();
    // int pos = comboBox->findText(text);
    // comboBox->setCurrentIndex(pos);
    comboBox->clearFocus(); // комбо теряет фокус и активизируется метод setModelData()
}

void myComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{   // сюда попадаем когда редактор делегата (QComboBox) теряет фокус/закрывается
    // и здесь данные из редактора передаем уже в модель
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int pos = comboBox->currentIndex();
    // посылаем в модель выбранную позицию комбо
    model->setData(index, pos, Qt::EditRole);
    // debug
    // // мои данные (для тестов)
    // model->setData(index, 123, Qt::UserRole);
}

void myComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &/*index*/) const
{
    // qDebug(logInfo()) << "ComboBoxDelegate::updateEditorGeometry";
    editor->setGeometry(option.rect);
}
