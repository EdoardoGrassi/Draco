#pragma once
#ifndef DRAKO_API_WIN_HPP
#define DRAKO_API_WIN_HPP

/// @file
/// @brief  Core header with Drako definitions.
/// @author Grassi Edoardo

#include "drako/core/preprocessor/platform_macros.hpp"

// Version number format: [ 10-bit MAJOR ] [ 10-bit MINOR ] [ 12-bit PATCH ]
#define DRAKO_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))

// Extracts major number from api version number.
#define DRAKO_VERSION_MAJOR(version) ((version) >> 22)

// Extracts minor number from api version number.
#define DRAKO_VERSION_MINOR(version) (((version) >> 12) & 0x3ff)

// Extracts patch number from api version number.
#define DRAKO_VERSION_PATCH(version) ((version) & 0xfff)

// Drako version number.
#define DRAKO_API_VERSION DRAKO_MAKE_VERSION(, , )


// Drako base namespace.
#define DRAKO_HPP_NAMESPACE drako


#if defined (DRAKO_PLT_WIN32)
// #define NOMINMAX definition moved to cmake file
# include <Windows.h>
#endif

#endif // !DRAKO_API_WIN_HPP
