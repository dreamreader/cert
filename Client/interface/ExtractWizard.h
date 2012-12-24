#ifndef EXTRACTWIZARD_H
#define EXTRACTWIZARD_H

#include <QList>
#include <QWizard>
#include <QDir>
#include <QDesktopWidget>

#include "../nb/Dispatcher.h"

#include "../interface/SchemeVisualiser.h"
#include "../interface/SelectDialog.h"
#include "../interface/gui.h"

#include "../client.h"

using namespace Nb;

namespace Ui {
class ExtractWizard;
}

class ExtractWizard : public QWizard
{
  Q_OBJECT

public:
  ExtractWizard(Nbc &container);
  ~ExtractWizard();

private:
  void _clear ();
  Ui::ExtractWizard *ui;
  void paintEvent (QPaintEvent * event);

  Nbc _nbc;
  InputScheme _input_scheme;

  QString _username;

  QStringList _container_paths;

  Dispatcher _modules;
  Provider _provider;
  Processor _processor;
  Nbcc _nbcc;

  QList <BimBase *> _images;
  QList <Matrix *> _params_own, _params_foreign, _patterns;
  Matrix _etalon, _code;

  bool _extracted;
  bool _result;

public slots:
  void changePage ();
  void addImage ();
  void clearImage ();
};

#endif // EXTRACTWIZARD_H
