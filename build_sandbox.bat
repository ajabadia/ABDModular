@echo off
REM ModularSandbox Build Script
REM Based on user provided robust template

echo ========================================
echo ModularSandbox Build Script
echo ========================================

REM 0. Inicializar entorno VS2022
echo.
echo 0. Inicializando entorno Visual Studio 2022...

set "CMAKE_GEN=-G "Visual Studio 17 2022" -A x64"

if defined VSCMD_VER goto :EnvReady

REM Check VS 18 first (Insiders/Community)
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" (
    echo Found VS 18 Community
    call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
    set "CMAKE_GEN="
    goto :EnvReady
)
if exist "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" (
    echo Found VS 18 Insiders
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
    set "CMAKE_GEN="
    goto :EnvReady
)

REM Check VS 2022 (17)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
    goto :EnvReady
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
    goto :EnvReady
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
    goto :EnvReady
)

echo Warning: VS2022 no encontrado. Usando entorno actual...

:EnvReady

REM 1. Buscar CMake
echo.
echo 1. Buscando CMake...

set "CMAKE_PATH="

where cmake >nul 2>nul
if %errorlevel% equ 0 (
    set "CMAKE_PATH=cmake"
    goto :FoundCMake
)

REM Check VS 18 Insiders CMake (vcpkg location)
if exist "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\vcpkg\scripts\cmake\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\vcpkg\scripts\cmake\cmake.exe"
    goto :FoundCMake
)

REM Check VS 18 Community CMake
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto :FoundCMake
)

REM Check Standard VS2022 Paths
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto :FoundCMake
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto :FoundCMake
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto :FoundCMake
)
if exist "C:\Program Files\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\CMake\bin\cmake.exe"
    goto :FoundCMake
)

echo Error: CMake no encontrado
echo Instala CMake o ejecuta desde Developer Command Prompt for VS 2022
pause
exit /b 1

:FoundCMake
echo    CMake encontrado: %CMAKE_PATH%

REM 2. Verificar JUCE (Flexble C: or D:)
echo.
echo 2. Verificando JUCE...

if exist "C:\JUCE\CMakeLists.txt" (
    echo    JUCE encontrado en C:\JUCE
) else if exist "D:\JUCE\CMakeLists.txt" (
    echo    JUCE encontrado en D:\JUCE
) else (
    echo Error: JUCE no encontrado en C:\JUCE ni D:\JUCE
    pause
    exit /b 1
)

REM 3. Limpiar build anterior
echo.
echo 3. Limpiando build anterior...

if exist build_sandbox (
    echo    Eliminando directorio build_sandbox...
    rmdir /s /q build_sandbox
)

REM 4. Configurar proyecto
echo.
echo 4. Configurando proyecto con CMake...

"%CMAKE_PATH%" -B build_sandbox -S . %CMAKE_GEN%
if %errorlevel% neq 0 (
    echo Error: Configuracion fallida
    pause
    exit /b 1
)

echo    Configuracion completada

REM 5. Compilar proyecto
echo.
echo 5. Compilando ModularSandbox (Release)...

"%CMAKE_PATH%" --build build_sandbox --config Release --target ModularSandbox
if %errorlevel% neq 0 (
    echo Error: Compilacion fallida
    pause
    exit /b 1
)

echo    Compilacion exitosa!

REM 6. Copiar y Ejecutar
echo.
echo 6. Buscando ejecutable...

REM Try standard JUCE artifact path
set "EXE_PATH=build_sandbox\ModularSandbox_artefacts\Release\Standalone\ModularSandbox.exe"

if not exist "%EXE_PATH%" (
    REM Try Artifacts Release path (seen in log)
    set "EXE_PATH=build_sandbox\ModularSandbox_artefacts\Release\ModularSandbox.exe"
)

if not exist "%EXE_PATH%" (
    REM Try Flat path just in case
    set "EXE_PATH=build_sandbox\Release\ModularSandbox.exe"
)

if exist "%EXE_PATH%" (
    copy "%EXE_PATH%" "ModularSandbox.exe"
    echo    Ejecutable copiado a: %CD%\ModularSandbox.exe
    
    echo.
    echo ========================================
    echo LANZANDO APLICACION...
    echo ========================================
    ModularSandbox.exe
) else (
    echo Warning: Ejecutable no encontrado en rutas esperadas.
    echo Busca en build_sandbox directory.
)

pause
