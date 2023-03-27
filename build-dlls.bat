@echo off
"C:\Program Files\CMake\bin\cmake.EXE" --build c:/Code/SpaceEngine/build --target space_opengl
CALL :CHECK

"C:\Program Files\CMake\bin\cmake.EXE" --build c:/Code/SpaceEngine/build --target space_code
CALL :CHECK

"C:\Program Files\CMake\bin\cmake.EXE" --build c:/Code/SpaceEngine/build --target all -- -d explain -n | find /i "no work"

if NOT ["%errorlevel%"]==["0"] (
    echo ==================================
    echo Source of main executable changed!
    pause
)
GOTO :EOF


:CHECK
if NOT ["%errorlevel%"]==["0"] (
    pause
)