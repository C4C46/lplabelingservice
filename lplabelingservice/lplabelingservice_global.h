#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LPLABELINGSERVICE_LIB)
#  define LPLABELINGSERVICE_EXPORT Q_DECL_EXPORT
# else
#  define LPLABELINGSERVICE_EXPORT Q_DECL_IMPORT
# endif
#else
# define LPLABELINGSERVICE_EXPORT
#endif
