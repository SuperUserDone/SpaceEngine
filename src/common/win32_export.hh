#pragma once

#ifndef APIFUNC

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