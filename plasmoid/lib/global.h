// Created via CMake from template global.h.in
// WARNING! Any changes to this file will be overwritten by the next CMake run!

#ifndef SYNCTHINGPLASMOID_GLOBAL
#define SYNCTHINGPLASMOID_GLOBAL

#include <c++utilities/application/global.h>

#ifdef SYNCTHINGPLASMOID_STATIC
#define SYNCTHINGPLASMOID_EXPORT
#define SYNCTHINGPLASMOID_IMPORT
#else
#define SYNCTHINGPLASMOID_EXPORT LIB_EXPORT
#define SYNCTHINGPLASMOID_IMPORT LIB_IMPORT
#endif

/*!
 * \def SYNCTHINGPLASMOID_EXPORT
 * \brief Marks the symbol to be exported by the syncthingplasmoid library.
 */

/*!
 * \def SYNCTHINGPLASMOID_IMPORT
 * \brief Marks the symbol to be imported from the syncthingplasmoid library.
 */

#endif // SYNCTHINGPLASMOID_GLOBAL
