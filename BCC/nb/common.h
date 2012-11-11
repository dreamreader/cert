#ifndef PCH_H
#define PCH_H

#include <QString>
#include "nb.h"
#include "Data.h"
#include "Nbc.h"

#define DIR_MODULES ".\\"

class Gui
{
public:
    static void Error (QString string)
    {
        qDebug() << QString::fromUtf8 ("Ошибка ") << string;
    }

    static void Warning (QString string)
    {
        qDebug() << "Предупреждение " << string;
    }
};

#endif // PCH_H
