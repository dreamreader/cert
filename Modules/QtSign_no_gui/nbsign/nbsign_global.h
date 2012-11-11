#ifndef NBSIGN_GLOBAL_H
#define NBSIGN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NBSIGN_LIBRARY)
#  define NBSIGNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NBSIGNSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NBSIGN_GLOBAL_H
