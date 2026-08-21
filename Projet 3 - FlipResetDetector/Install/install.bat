@echo off
setlocal

title Flip Reset Detector - Installation

echo ============================================
echo        FLIP RESET DETECTOR - INSTALL
echo ============================================
echo.

REM Dossier dans lequel se trouve install.bat
set "INSTALL_DIR=%~dp0"

REM Dossier principal de BakkesMod
set "BAKKES_DIR=%APPDATA%\bakkesmod\bakkesmod"

REM Destination du plugin
set "PLUGIN_DIR=%BAKKES_DIR%\plugins"

REM Destination des ressources du plugin
set "DATA_DIR=%BAKKES_DIR%\data\FlipResetDetector"


REM ============================================================
REM Verification de BakkesMod
REM ============================================================

if not exist "%BAKKES_DIR%" (
    echo [ERREUR] BakkesMod n'a pas ete trouve.
    echo.
    echo Dossier recherche :
    echo %BAKKES_DIR%
    echo.
    echo Verifie que BakkesMod est installe puis relance
    echo cet installateur.
    echo.
    pause
    exit /b 1
)


if not exist "%PLUGIN_DIR%" (
    echo [ERREUR] Le dossier plugins de BakkesMod est introuvable.
    echo.
    pause
    exit /b 1
)


REM ============================================================
REM Verification des fichiers d'installation
REM ============================================================

if not exist "%INSTALL_DIR%FlipResetDetector.dll" (
    echo [ERREUR] FlipResetDetector.dll est introuvable.
    echo.
    echo Le fichier doit etre dans le meme dossier
    echo que install.bat.
    echo.
    pause
    exit /b 1
)


if not exist "%INSTALL_DIR%fennec_under.png" (
    echo [ERREUR] fennec_under.png est introuvable.
    echo.
    echo Le fichier doit etre dans le meme dossier
    echo que install.bat.
    echo.
    pause
    exit /b 1
)


REM ============================================================
REM Installation de la DLL
REM ============================================================

echo [1/3] Installation du plugin...

copy /Y ^
"%INSTALL_DIR%FlipResetDetector.dll" ^
"%PLUGIN_DIR%\FlipResetDetector.dll" >nul


if errorlevel 1 (
    echo.
    echo [ERREUR] Impossible de copier FlipResetDetector.dll.
    echo.
    echo Ferme Rocket League et BakkesMod puis reessaie.
    echo.
    pause
    exit /b 1
)


REM ============================================================
REM Creation du dossier de ressources
REM ============================================================

echo [2/3] Creation du dossier de ressources...

if not exist "%DATA_DIR%" (
    mkdir "%DATA_DIR%"
)


if errorlevel 1 (
    echo.
    echo [ERREUR] Impossible de creer le dossier :
    echo %DATA_DIR%
    echo.
    pause
    exit /b 1
)


REM ============================================================
REM Installation de l'image
REM ============================================================

echo [3/3] Installation de l'image...

copy /Y ^
"%INSTALL_DIR%fennec_under.png" ^
"%DATA_DIR%\fennec_under.png" >nul


if errorlevel 1 (
    echo.
    echo [ERREUR] Impossible de copier fennec_under.png.
    echo.
    pause
    exit /b 1
)


REM ============================================================
REM Termine
REM ============================================================

echo.
echo ============================================
echo          INSTALLATION TERMINEE
echo ============================================
echo.
echo Flip Reset Detector a ete installe.
echo.
echo Plugin :
echo %PLUGIN_DIR%\FlipResetDetector.dll
echo.
echo Ressource :
echo %DATA_DIR%\fennec_under.png
echo.
echo Lance maintenant BakkesMod et Rocket League.
echo.
echo Dans Rocket League :
echo   1. Ouvre la console BakkesMod avec F6
echo   2. Entre la commande :
echo.
echo      plugin load FlipResetDetector
echo.
echo Le plugin sera alors disponible en Freeplay.
echo.

pause
endlocal
