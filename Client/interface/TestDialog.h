#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QList>
#include <QDialog>
#include <QDir>
#include <QHBoxLayout>

#include "../nb/Dispatcher.h"
#include "../client.h"

#include "../interface/SchemeVisualiser.h"
#include "../interface/SelectDialog.h"
#include "../interface/gui.h"

using namespace Nb;

namespace Ui {
class TestDialog;
}

class TestDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TestDialog(Nbc &nbc);
  void getStats (float &all, float &succeed);
  ~TestDialog();

private:
  Ui::TestDialog *ui;
  QLabel _result;
  QHBoxLayout *_layout;
  void paintEvent (QPaintEvent * event);

  void _reloadInput ();

  QList <BimBase *> _images;
  QList <Matrix *> _params_own, _params_foreign, _patterns;
  Dispatcher _modules;
  Provider _provider;
  Processor _processor;
  Nbcc _nbcc;
  InputScheme _input_scheme;
  Nbc _nbc;
  Matrix _code, _etalon;

  float _tries, _successes;

public slots:
  void add ();
  void clear ();
  void test ();
  void onClose ();
  void again ();
};

#endif // TESTDIALOG_H
