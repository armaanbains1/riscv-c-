@echo off
SET VCPKG_PATH=C:\vcpkg\installed\x64-windows

:: /MD is the key here to match the vcpkg libraries
:: We also removed the double /out flag
cl /EHsc /std:c++17 /MD ^
    /I"%VCPKG_PATH%\include" ^
    ui.cpp bus.cpp cpu.cpp registers.cpp ^
    /link /LIBPATH:"%VCPKG_PATH%\lib" ^
    ftxui-screen.lib ftxui-dom.lib ftxui-component.lib ^
    /out:emulator.exe

if %errorlevel% neq 0 (
    echo [!] Build Failed with code %errorlevel%
    exit /b %errorlevel%
)

echo [!] Build Successful! Running emulator...
emulator.exe