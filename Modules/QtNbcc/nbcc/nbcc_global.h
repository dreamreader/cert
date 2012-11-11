#ifndef NBCC_GLOBAL_H
#define NBCC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NBCC_LIBRARY)
#  define NBCCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NBCCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NBCC_GLOBAL_H
