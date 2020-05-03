// Copyright 2009-2020, Scott MacDonald.
#include "console.h"
#if _WIN32
#   include "windows/windows_console.h"
#else
#   include "unix/unix_console.h"
#endif

using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<Console> Brainfreeze::CommandLineApp::CreateConsole()
{
#if _WIN32
    return std::make_unique<WindowsConsole>();
#else
    return std::make_unique<UnixConsole>();
#endif
}
