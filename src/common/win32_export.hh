#pragma once

// Standard windows shite

#ifndef APIFUNC

#ifdef _WIN32
#define ALWAYS_EXPORT __declspec(dllexport)
#else
#define ALWAYS_EXPORT
#endif

#ifdef EXPORT_WIN
#ifdef _WIN32
#define APIFUNC __declspec(dllexport)
#else
#define APIFUNC __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define APIFUNC __declspec(dllimport)
#else
#define APIFUNC
#endif
#endif
#endif
