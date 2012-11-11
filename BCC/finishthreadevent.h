#ifndef FINISHTHREADEVENT_H
#define FINISHTHREADEVENT_H

#include <QEvent>
#include <QThread>

class FinishThreadEvent : public QEvent
{
public:
  static const QEvent::Type FinishThreadEventType;

  FinishThreadEvent(QThread* t);

  QThread* thread();

private:
  QThread* _thread;
};


#endif // FINISHTHREADEVENT_H
