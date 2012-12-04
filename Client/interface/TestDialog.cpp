#include "TestDialog.h"
#include "ui_TestDialog.h"

//------------------------------------------------------------

TestDialog::TestDialog(Nbc &nbc) :
  QDialog(0), ui(new Ui::TestDialog), _result(NULL), _layout(NULL)
{
  ui->setupUi(this);
  _result.setAlignment(Qt::AlignHCenter);

  _nbc = nbc;
  _modules.loadAllModules ((QString)"." + QDir::separator () + (QString)"modules" + QDir::separator ());
  _input_scheme.extractFrom (_nbc);
  _nbc.getCode(_etalon);
  SchemeVisualiser::view(_nbc.getSchemeNumber(), ui->scheme_2);

  for (uint i = 0; i != _input_scheme.size (); i++) {
    BimBase *bims = new BimBase;
    Matrix *params_own = new Matrix;
    Matrix *params_foreign = new Matrix;
    Matrix *pattern = new Matrix;
    _images.push_back(bims);
    _params_own.push_back(params_own);
    _params_foreign.push_back(params_foreign);
    _patterns.push_back (pattern);
  }

  _reloadInput ();

  _tries = 0;
  _successes = 0;
  ui->btnAccept->setFocus();
}

//------------------------------------------------------------

TestDialog::~TestDialog()
{
  for (uint i = 0; i != _input_scheme.size (); i++) {
    delete _params_own[i];
    delete _params_foreign[i];
    delete _patterns[i];
    _images[i]->clear ();
    delete _images[i];
  }

  delete ui;
}

//------------------------------------------------------------

void TestDialog::paintEvent (QPaintEvent * event)
{
  QDialog::paintEvent(event);
  SchemeVisualiser::redraw (ui->scheme_2);
}

//------------------------------------------------------------

void TestDialog::add ()
{
  try {
    if (!_provider.capture()) {
      return;
    }
  } catch (QString error) {
    Gui::error(error);
  }

  if (!_input_scheme.nextStep()) {
    test ();
    for (uint i = 0; i != _input_scheme.size (); i++) {
      delete _params_own[i];
      delete _params_foreign[i];
      delete _patterns[i];
      _images[i]->clear ();
      delete _images[i];
    }
    _params_own.clear ();
    _params_foreign.clear ();
    _patterns.clear ();
    _images.clear ();
    for (uint i = 0; i != _input_scheme.size (); i++) {
      BimBase *bims = new BimBase;
      Matrix *params_own = new Matrix;
      Matrix *params_foreign = new Matrix;
      Matrix *pattern = new Matrix;
      _images.push_back(bims);
      _params_own.push_back(params_own);
      _params_foreign.push_back(params_foreign);
      _patterns.push_back (pattern);
    }
    _input_scheme.toStart ();
  } else {
    _reloadInput ();
  }
}

//------------------------------------------------------------

void TestDialog::clear ()
{
  try {
    _provider.resetInput();
  } catch (QString error) {
    Gui::error(error);
  }
}

//------------------------------------------------------------

void TestDialog::test ()
{
  try { //Преобразование образов
    _input_scheme.toStart();
    do {
      if (_images[_input_scheme.index()]->size() < 1) continue;
      if (!_modules.getProcessor(_input_scheme.getCurrentProcessor(), _processor)) {
        throw QString::fromUtf8("Модуль процессора не найден!");
      }
      if (_processor.getId() == (Uuid) UUID_FINGER_PROCESSOR) {
        Matrix pattern;
        if (_nbc.getBimPattern(_input_scheme.getCurrentSlotId(), _images[_input_scheme.index()]->getBimType(0), pattern)) {
          _processor.setPattern(pattern);
        }
      }
      if (_images[_input_scheme.index()]->size() > 0) {
        _processor.process (*_params_own[_input_scheme.index()], *_images[_input_scheme.index()]);
      }
    } while (_input_scheme.next ());
  } catch (QString error) {
    Gui::error (error);
  }

  int dif = 128;

  try { //Извлечение

    switch (int errc = Client::testData(_params_own))
    {
    case nbS_OK:
      break;
    default:
      qDebug() << "failed to testData, errc=" << Result (errc);
      throw QString::fromUtf8 ("Невозможно провести тестирование");
    }
  } catch (QString error) {
    Gui::error (error);
  }

  _tries++;
  _provider.reset();
  _layout = new QHBoxLayout(ui->widget);

  delete ui->widget->layout();
  ui->widget->setLayout(_layout);
  ((QHBoxLayout *)ui->widget->layout())->addWidget(&_result, 1);
  QString result;
  if (dif == 0) {
    result = QString::fromUtf8("Пользователь определен как СВОЙ");
    _successes++;
  } else {
    result = QString::fromUtf8("Пользователь определен как ЧУЖОЙ");
    result += QString::fromUtf8("\r\nКоличество несовпавших разрядов: ") + QString::number (dif);
  }
  _result.setText (result);
  _result.show();
  ui->_btnAgain->show();
  ui->scheme_2->hide();
  ui->btnAccept->hide();
  ui->btnClear->hide();
  this->resize(QSize (this->geometry().x(), 180));
}

//------------------------------------------------------------

void TestDialog::_reloadInput ()
{
  _result.hide();
  ui->_btnAgain->hide ();
  ui->scheme_2->show();
  ui->btnAccept->show();
  ui->btnClear->show();
  if (_input_scheme.branchesCount () > 1) {
    QStringList choices;
    for (int i = 0; i != _input_scheme.branchesCount (); i++) {
      Provider null;
      ModuleInfo info;
      _modules.getProvider(_input_scheme.getCurrentProvider(), null, &info);
      choices << info.name;
      _input_scheme.next ();
    }
    SelectDialog dialog (this, choices);
    dialog.exec ();
    int res = dialog.getSelection ();
    res == -1 ? res = 0 : res;
    _input_scheme.toFirstBranch();
    for (int i = 0; i < res; i++) _input_scheme.next ();
  }
  SchemeVisualiser::clearMarks(ui->scheme_2);
  SchemeVisualiser::mark(_input_scheme.index(), SCHEMEVISUALISER_STATUS_PROGRESS, ui->scheme_2);
  try {
    if (!_modules.getProvider(_input_scheme.getCurrentProvider(), _provider)) {
      throw QString::fromUtf8("Модуль провайдера не найден!");
    }
    if (_provider.getId() == (Uuid) UUID_FINGER_PROVIDER) {
      this->resize(QSize (640, 648));
    } else {
      this->resize(QSize (720, 520));
    }
    _provider.attachDevice();
    this->QWidget::setVisible(false);
    this->QWidget::setVisible(true);
    _provider.attachWindow(ui->widget);
    this->QWidget::setVisible(false);
    this->QWidget::setVisible(true);
    ui->btnAccept->setDefault(true);
    _provider.startCapturing(_images[_input_scheme.index()]);
  } catch (QString error) {
    Gui::error (error);
  }
}

//--------------------------------------------------------------

void TestDialog::onClose ()
{

}

//--------------------------------------------------------------

void TestDialog::again ()
{
  _reloadInput ();
}

void TestDialog::getStats (float &all, float &succeed)
{
    all = _tries;
    succeed = _successes;
}
