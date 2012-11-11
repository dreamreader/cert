#include "finishthreadevent.h"

const QEvent::Type FinishThreadEvent::FinishThreadEventType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

FinishThreadEvent::FinishThreadEvent(QThread* t): QEvent(FinishThreadEventType), _thread(t)
{ }

QThread* FinishThreadEvent::thread()
{
  return _thread;
}
