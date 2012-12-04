#ifndef TRAINWIZARD_H
#define TRAINWIZARD_H

#define MINIMUM_TRAIN_IMAGES 12

#include <math.h>
#include <QList>
#include <QWizard>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>

#include "../model/Dispatcher.h"
#include "../model/ProgramUser.h"
#include "../model/client.h"
#include "../model/SchemeConstructor.h"

#include "../interface/SchemeVisualiser.h"
#include "../interface/TestDialog.h"
#include "../interface/gui.h"

using namespace Nb;

namespace Ui {
class TrainWizard;
}

class TrainWizard : public QWizard
{
  Q_OBJECT

public:
  TrainWizard(QString username);
  ~TrainWizard();

public slots:
  void test ();
  void edit ();
  void txtNameChanged ();
  void txtCommentChanged ();
  void listSchemesChanged ();
  void listImagesChanged ();
  void change ();
  void addImage ();
  void clearImage ();
  void deleteImage ();
  void deleteAllImages ();
  void finishClicked (int id);
  void paintEvent (QPaintEvent *event);
  void clickedKey ();
  void clickedSchemeEditor ();
  void changeKey ();
  void loadBims ();
  void saveBims ();

private:
  Ui::TrainWizard *ui;
  void _reloadImageList ();
  void _makeKey ();

  bool _cleared;

  QString _username;
  QString _path;
  QList <BimBase *> _images;
  QList <Matrix *> _params_own, _params_foreign, _patterns;
  Matrix _code;
  Statistics _stats;
  Dispatcher _modules;
  Provider _provider;
  Processor _processor;
  Nbcc _nbcc;
  InputScheme _input_scheme;
  Nbc _nbc;
};

#endif // TRAINWIZARD_H
