#ifndef DLL_H
#define DLL_H

#include <QLibrary>
#include <QStringList>
#include "nb/nb.h"

#include "qtnb/Data.h"
#include "qtnb/Uuid.h"

#include "interface/ExtractWizard.h"
#include "interface/TrainWizard.h"

#include <Qt/qapplication.h>
#include <QtCore/qglobal.h>

#include "interface/gui.h"
#include "model/ProgramUser.h"
#include "model/client.h"

#if defined(UNTITLED_LIBRARY)
#  define UNTITLEDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UNTITLEDSHARED_EXPORT Q_DECL_IMPORT
#endif

#define SIGNATURE_SIZE 256
#define CONTAINER_NAME "default"

using namespace Nb;

extern "C"
{
    Q_DECL_EXPORT nbResult sign(const nbData username, const nbData data, nbData signature);
    Q_DECL_EXPORT nbResult registrate(const nbData username, const nbData key);
}


#endif // DLL_H
