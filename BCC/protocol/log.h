#ifndef LOG_H
#define LOG_H

#include <QFile>
#include <QMutex>
#include <QTextStream>
#include "../../nb/nb.h"
#include "../../nb/Nbc.h"

#define   LOG               Log::Tab tab; \
                            if (0) tab = tab;

///Класс потокобезопасного лога для отладки
class Log
{
public:
  class Tab
  {
  public:
    Tab() {Log::goIn();}
    ~Tab(){Log::goOut();}
  };

private:
  static QFile    _file;        ///<файл лога
  static QMutex   _mutex;       ///<мьютекс лога
  static unsigned _offset;      ///<смещение при выводе

public:
  /* Открыть лог в файле path
   *
   */
  static bool open(QString path);

  /* Закрыть лог
   *
   */
  static bool close();

  /* Написать в лог
   *
   */
  static void write(QString msg);
  static void write(QString msg, bool result);
  static void write(QString msg, int value, bool asHex = false);
  static void write(QString msg, nbData data);

  /* Увеличить уровень вложенности
   *
   */
  static void goIn();

  /* Уменьшить уровень вложенности
   *
   */
  static void goOut();
};

#endif // LOG_H
