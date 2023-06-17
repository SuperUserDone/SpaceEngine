#pragma once

// Standard windows shite

#ifndef PYROAPI

#ifdef PYRO_EXPORT_WIN
#ifdef _WIN32
#define PYROAPI __declspec(dllexport)
#else
#define PYROAPI __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define PYROAPI __declspec(dllimport)
#else
#define PYROAPI 
#endif
#endif
#endif

