@echo off
title YP Studio - Professional 64-bit Builder
echo =========================================
echo    YP CREATION STUDIO - MSYS2 UCRT64
echo =========================================
echo.

:: MSYS2 paths
set MSYS2_ROOT=C:\msys64
set COMPILER_PATH=%MSYS2_ROOT%\ucrt64\bin
set PATH=%COMPILER_PATH%;%PATH%

:: Project root (this .bat location)
cd /d "%~dp0"

:: Output folder
if not exist bin mkdir bin

:: 1) (Optional) Resource compile - only if resource.rc exists
if exist resource.rc (
  echo [1/3] Compiling Resources (x64)...
  "%COMPILER_PATH%\windres.exe" --target=pe-x86-64 resource.rc -O coff -o resource.res
  if %errorlevel% neq 0 (
      echo ERROR: Resource compilation failed!
      pause
      exit /b
  )
) else (
  echo [1/3] Skipping resource.rc (not found)
)

:: 2) Get wxWidgets flags via MSYS2 bash
for /f "usebackq delims=" %%i in (`"%MSYS2_ROOT%\usr\bin\bash.exe" -lc "wx-config --cxxflags"`) do set WX_CXXFLAGS=%%i
for /f "usebackq delims=" %%i in (`"%MSYS2_ROOT%\usr\bin\bash.exe" -lc "wx-config --libs --static"`) do set WX_LIBS=%%i

:: 3) Compile + Link
echo [2/3] Compiling C++ Source (x64)...
"%COMPILER_PATH%\g++.exe" -m64 src\main.cpp resource.res -o bin\wstudio-draw.exe -static %WX_CXXFLAGS% %WX_LIBS% -lws2_32 -lcomctl32 -luser32 -lgdi32 -ldwmapi -lshell32 -mwindows
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b
)

echo [3/3] Build completed.
echo =========================================
echo    SUCCESS: bin\wstudio-draw.exe ready!
echo =========================================
pause
