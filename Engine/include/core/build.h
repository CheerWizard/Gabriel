#pragma once

// dynamic library
// defined for engine library
#ifdef GABRIEL_DLL_BUILD
#define GABRIEL_API __declspec(dllexport)
// defined for client
#elif defined(GABRIEL_DLL)
#define GABRIEL_API __declspec(dllimport)
#else
// static library
#define GABRIEL_API
#endif

// dynamic library
// defined for script library
#ifdef SCRIPT_DLL
#define SCRIPT_API __declspec(dllexport)
// defined for client
#else
#define SCRIPT_API __declspec(dllimport)
#endif