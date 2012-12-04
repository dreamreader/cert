#include "SchemeVisualiser.h"

//TODO сделать свой виджет - наследник QTableWidget
//TODO или не надо, так удобнее гуи рисовать в дизайнере

//------------------------------------------------------------

bool SchemeVisualiser::view (const InputScheme &scheme, QTableWidget &widget)
{
  return false;
}

bool SchemeVisualiser::redraw (QTableWidget *widget)
{
    for (int i = 0; i < widget->rowCount(); i +=2 ) {
        widget->setRowHeight(i,  SCHEMEVISUALISER_HEIGHT);
    }

    for (int i = 1; i < widget->rowCount(); i +=2 ) {
        widget->setRowHeight(i,  SCHEMEVISUALISER_GAP);
    }


    for (int i = 1; i < widget->columnCount(); i += 2) {
        widget->setColumnWidth(i,  SCHEMEVISUALISER_ARROW_SIZE);
    }
    for (int i = 2; i < widget->columnCount()-1; i += 2) {
        widget->setColumnWidth(i,  ( widget->geometry().width()  -  2 * SCHEMEVISUALISER_START_SIZE  -  SCHEMEVISUALISER_ARROW_SIZE * (widget->columnCount() - 1) / 2 )
                                   / ( ( widget->columnCount() + 1 ) / 2  -  2  ) );
    }

    widget->setColumnWidth(0,  SCHEMEVISUALISER_START_SIZE);
    widget->setColumnWidth(widget->columnCount()-1,  SCHEMEVISUALISER_START_SIZE);
    return true;
}

//------------------------------------------------------------

bool SchemeVisualiser::view (int index, QTableWidget *widget)
{
    switch (index) {
    case 0://Ввод рукописного образа
        widget->setRowCount(1);
        widget->setColumnCount(5);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createHorArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 3, createHorArrow ());
        widget->setCellWidget(0, 4, createEnd ());
        break;
    case 1://Ввод отпечатка пальца
        widget->setRowCount(1);
        widget->setColumnCount(5);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createHorArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(0, 3, createHorArrow ());
        widget->setCellWidget(0, 4, createEnd ());
        break;
    case 2://Ввод рукописного образа И отпечатка пальца
        widget->setRowCount(1);
        widget->setColumnCount(7);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createHorArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 3, createHorArrow ());
        widget->setCellWidget(0, 4, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(0, 5, createHorArrow ());
        widget->setCellWidget(0, 6, createEnd ());
        break;
    case 3://Ввод рукописного образа ИЛИ отпечатка пальца
        widget->setRowCount(3);
        widget->setColumnCount(5);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createLeftCrossUpperArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 3, createRightCrossUpperArrow ());
        widget->setCellWidget(0, 4, createEnd ());

        widget->setCellWidget(1, 1, createVerArrow ());
        widget->setCellWidget(1, 3, createVerArrow ());

        widget->setCellWidget(2, 1, createLeftCrossLowerArrow ());
        widget->setCellWidget(2, 2, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(2, 3, createRightCrossLowerArrow ());

        break;
    case 4://Ввод рукописного образа И рукописного образа
        widget->setRowCount(1);
        widget->setColumnCount(7);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createHorArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 3, createHorArrow ());
        widget->setCellWidget(0, 4, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 5, createHorArrow ());
        widget->setCellWidget(0, 6, createEnd ());

        break;

    case 5://Ввод отпечатка пальца И отпечатка пальца
        widget->setRowCount(1);
        widget->setColumnCount(7);

        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createHorArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(0, 3, createHorArrow ());
        widget->setCellWidget(0, 4, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(0, 5, createHorArrow ());
        widget->setCellWidget(0, 6, createEnd ());

        break;


    case 6://Ввод рукописного образа ИЛИ рукописного образа
        widget->setRowCount(3);
        widget->setColumnCount(5);
        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createLeftCrossUpperArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(0, 3, createRightCrossUpperArrow ());
        widget->setCellWidget(0, 4, createEnd ());
        widget->setCellWidget(1, 1, createVerArrow ());
        widget->setCellWidget(1, 3, createVerArrow ());
        widget->setCellWidget(2, 1, createLeftCrossLowerArrow ());
        widget->setCellWidget(2, 2, createTech (UUID_HANDWR_PROVIDER));
        widget->setCellWidget(2, 3, createRightCrossLowerArrow ());
        break;


    case 7://Ввод отпечатка пальца ИЛИ отпечатка пальца
        widget->setRowCount(3);
        widget->setColumnCount(5);
        widget->setCellWidget(0, 0, createStart());
        widget->setCellWidget(0, 1, createLeftCrossUpperArrow ());
        widget->setCellWidget(0, 2, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(0, 3, createRightCrossUpperArrow ());
        widget->setCellWidget(0, 4, createEnd ());
        widget->setCellWidget(1, 1, createVerArrow ());
        widget->setCellWidget(1, 3, createVerArrow ());
        widget->setCellWidget(2, 1, createLeftCrossLowerArrow ());
        widget->setCellWidget(2, 2, createTech (UUID_FINGER_PROVIDER));
        widget->setCellWidget(2, 3, createRightCrossLowerArrow ());
        break;

    default:
        return false;
    }
    redraw (widget);
    return true;
}

//------------------------------------------------------------

bool SchemeVisualiser::mark (size_t index, int status, QTableWidget *widget)
{
    int i = 0, j = 2;
    for (uint k = 0; k != index; k++) {
        if (i < widget->rowCount()-1) {//переход по вертикали
            i += 2;
            if ( !widget->cellWidget(i, j) ) {
                k--;
            }
        } else if (j < widget->columnCount()-3) {//переход по горизонтали
            i = 0;
            j += 2;
        } else {//элементы кончились
            return false;
        }
    }

    if ( widget->cellWidget(i, j) ) {
        switch (status) {
        case SCHEMEVISUALISER_STATUS_EMPTY:
            break;
        case SCHEMEVISUALISER_STATUS_FULL:
            break;
        case SCHEMEVISUALISER_STATUS_PROGRESS:
            widget->cellWidget(i, j)->setStyleSheet(SCHEMEVISUALISER_STYLE_PROGRESS);
            break;
        default:
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------

bool SchemeVisualiser::clearMarks(QTableWidget *widget)
{
    for (int i = 0; i < widget->rowCount(); i+=2) {
        for (int j = 2; j < widget->columnCount()-2; j+=2) {
            if ( widget->cellWidget(i, j) ) {
                widget->cellWidget(i, j)->setStyleSheet(SCHEMEVISUALISER_STYLE_EMPTY);
            }
        }
    }
    return true;
}

//------------------------------------------------------------

QIcon SchemeVisualiser::getTechIcon (Uuid tech_id)
{
    if (tech_id == (Uuid)UUID_FINGER_PROVIDER) {
        return QIcon (":/res/finger_tech.png");
    } else if (tech_id == (Uuid)UUID_HANDWR_PROVIDER){
        return QIcon (":/res/handwr_tech.png");
    } else {
        return QIcon (":/res/unknown_tech.png");
    }
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createStart ()
{
    QLabel *label = new QLabel ();
    label->setText(QString::fromUtf8("Начало"));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(SCHEMEVISUALISER_STYLE_START);
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createEnd ()
{
    QLabel *label = new QLabel ();
    label->setText(QString::fromUtf8("Конец"));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(SCHEMEVISUALISER_STYLE_FINISH);
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createHorArrow () // ->
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createVerArrow () // |
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/varrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createLeftCrossArrow () // |->
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/left-cross-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createLeftCrossUpperArrow () // г->
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/left-cross-upper-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createLeftCrossLowerArrow () // |_>
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/left-cross-lower-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createRightCrossArrow () // -|
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/right-cross-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createRightCrossUpperArrow () // -|-
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/right-cross-upper-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createRightCrossLowerArrow () // _|
{
    QLabel *label = new QLabel ();
    label->setPixmap(QPixmap(":/res/right-cross-lower-arrow.png"));
    return label;
}

//------------------------------------------------------------

QLabel *SchemeVisualiser::createTech (Uuid tech_id)
{
    QLabel *label = new QLabel ();
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(SCHEMEVISUALISER_STYLE_EMPTY);

    if (tech_id == (Uuid)UUID_HANDWR_PROVIDER) {
        label->setText(QString::fromUtf8("Рукописный ввод"));
    } else if (tech_id == (Uuid)UUID_FINGER_PROVIDER){
        label->setText(QString::fromUtf8("Отпечаток пальца"));
    } else {
        label->setText(QString::fromUtf8("Неизвестная технология"));
    }
    return label;
}
