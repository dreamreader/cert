#ifndef SCHEMEWIDGET_H
#define SCHEMEWIDGET_H

#include <QLabel>
#include <QTableWidget>

#include "../nb/InputScheme.h"
#include "../nb/ModuleInfo.h"

#define SCHEMEVISUALISER_ARROW_SIZE 32
#define SCHEMEVISUALISER_START_SIZE 96
#define SCHEMEVISUALISER_HEIGHT 32
#define SCHEMEVISUALISER_GAP 4

#define SCHEMEVISUALISER_STATUS_EMPTY 0
#define SCHEMEVISUALISER_STATUS_FULL 1
#define SCHEMEVISUALISER_STATUS_PROGRESS 2

#define SCHEMEVISUALISER_STYLE_EMPTY "background-color: white; color: black; border: 1px solid black;"
#define SCHEMEVISUALISER_STYLE_START "background-color: white; color: black; border: 1px solid black;"
#define SCHEMEVISUALISER_STYLE_FINISH "background-color: white; color: black; border: 1px solid black;"
#define SCHEMEVISUALISER_STYLE_FULL "background-color: white; color: black; border: 1px solid black;"
#define SCHEMEVISUALISER_STYLE_PROGRESS "background-color: white; color: red; border: 1px solid black;"

using namespace Nb;

class SchemeVisualiser
{
public:
  static bool view (const InputScheme &scheme, QTableWidget &widget);
  static bool save (InputScheme &scheme, QTableWidget &widget);

  static bool view (int scheme_index, QTableWidget *widget);
  static bool mark (size_t index, int status, QTableWidget *widget);
  static bool clearMarks(QTableWidget *widget);
  static bool redraw (QTableWidget *widget);

signals:

public slots:

private:
  static QLabel *createStart ();
  static QLabel *createEnd ();
  static QLabel *createHorArrow ();
  static QLabel *createVerArrow ();
  static QLabel *createLeftCrossArrow ();
  static QLabel *createLeftCrossUpperArrow ();
  static QLabel *createLeftCrossLowerArrow ();
  static QLabel *createRightCrossArrow ();
  static QLabel *createRightCrossUpperArrow ();
  static QLabel *createRightCrossLowerArrow ();
  static QLabel *createTech (Uuid tech_id);
  static QIcon getTechIcon (Uuid tech_id);
};
#endif // SCHEMEWIDGET_H
