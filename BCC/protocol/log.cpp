#include "log.h"

QFile Log::_file;
QMutex Log::_mutex;
unsigned Log::_offset(0);

/* Открыть лог в файле path
 *
 */
bool Log::open(QString path)
{
  _mutex.lock();
  if (_file.isOpen()) _file.close();
  _file.setFileName(path);
  bool res = _file.open(QIODevice::ReadWrite | QIODevice::Truncate);
  _mutex.unlock();
  return res;
}

/* Закрыть лог
 *
 */
bool Log::close()
{
  _mutex.lock();
  _file.close();
  bool res = !_file.isOpen();
  _mutex.unlock();
  return res;
}

/* Написать в лог
 *
 */
void Log::write(QString msg)
{
  _mutex.lock();
  QTextStream stream(&_file);
  if (!_file.isOpen()) goto unlock;
  for (unsigned i=0; i<_offset; i++) stream << " ";
  stream << msg << endl;

#if !defined(QT_NO_DEBUG_STREAM)
  qDebug() << msg;
#endif

unlock:
  _mutex.unlock();
}

void Log::write(QString msg, bool result)
{
  _mutex.lock();
  QTextStream stream(&_file);
  if (!_file.isOpen()) goto unlock;
  for (unsigned i=0; i<_offset; i++) stream << " ";
  stream << msg;
  if (result)
    stream << " ok" << endl;
  else
    stream << " FAIL" << endl;

#if !defined(QT_NO_DEBUG_STREAM)
  qDebug() << msg << " " << (result? "ok" : "FAIL");
#endif

unlock:
  _mutex.unlock();
}

void Log::write(QString msg, int value, bool asHex)
{
  _mutex.lock();
  QTextStream stream(&_file);
  if (!_file.isOpen()) goto unlock;
  for (unsigned i=0; i<_offset; i++) stream << " ";
  if (!asHex)
    stream << msg << " " << value << endl;
  else
    stream << msg << " " << QString::number(value, 16) << endl;

#if !defined(QT_NO_DEBUG_STREAM)
  qDebug() << msg << " " << QString::number(value, asHex?16:10);
#endif

unlock:
  _mutex.unlock();
}

void Log::write(QString msg, nbData data)
{
  Nb::Data d(data);
  QString t = "0123456789ABCDEF";
  for (int i=0; i<d.size(); ++i){
    QString s;
    s.append(t[((uint8_t)((uint8_t*)d.data())[i])>>4]);
    s.append(t[((uint8_t*)d.data())[i] & 0x0F]);
    Log::write(s);
  }
}

// Увеличить уровень вложенности
void Log::goIn()
{
  _mutex.lock();

  QTextStream stream(&_file);
  if (!_file.isOpen()) goto unlock;
  for (unsigned i=0; i<_offset; i++) stream << " ";
  stream << "{" << endl;
  _offset += 4;

unlock:
  _mutex.unlock();
}

// Уменьшить уровень вложенности
void Log::goOut()
{
  _mutex.lock();

  if (_offset >= 4)
    _offset -= 4;
  QTextStream stream(&_file);
  if (!_file.isOpen()) goto unlock;
  for (unsigned i=0; i<_offset; i++) stream << " ";
  stream << "}" << endl;

unlock:
  _mutex.unlock();
}
