#ifndef GUI_H
#define GUI_H

#include <QString>
#include <QMessageBox>

class Gui
{
public:
    static void error (QString string)
    {
        QMessageBox box(QMessageBox::Critical, QString::fromUtf8 ("Ошибка"), string);
        box.exec();
    }

    static void warning (QString string)
    {
        QMessageBox box(QMessageBox::Warning, QString::fromUtf8 ("Предупреждение"), string);
        box.exec();
    }
};

#endif // GUI_H
