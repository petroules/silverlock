#ifndef SILVERLOCKLIB_GLOBAL_H
#define SILVERLOCKLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SILVERLOCKLIB_EXPORTS)
#  define SILVERLOCK_API Q_DECL_EXPORT
#elif defined(SILVERLOCKLIB_IMPORTS)
#  define SILVERLOCK_API Q_DECL_IMPORT
#else
#  define SILVERLOCK_API
#endif

#endif // SILVERLOCKLIB_GLOBAL_H
