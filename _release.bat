@echo off
if not "%~1"=="" goto PSUBUILD
set "PTL_BUILD_DIR=..\PTLIBMSBUILDIR"
set "PTL_STORE_DIR=%CD%"
if exist "%PTL_BUILD_DIR%" rd /s /q "%PTL_BUILD_DIR%\"
if exist "%PTL_BUILD_DIR%" pause&goto :EOF
md "%PTL_BUILD_DIR%"
if not exist "%PTL_BUILD_DIR%" goto :EOF
set PROT_SRC_DIR=%CD%
pushd "%PTL_BUILD_DIR%"
set PROT_BUILD_DIR=%CD%
if not errorlevel 0 goto :EOF
if "%PROT_SRC_DIR%"=="%PROT_BUILD_DIR%" goto :EOF

set VCVAR_FILE=%MSVC_ROOT%\vcvarsall.bat
if exist "%VCVAR_FILE%" goto CHECKCMAKE
echo ============================================
echo, vcvarsall.bat can not found !
echo, please set MSVC_ROOT to VS\VC Install dir.
echo ============================================
pause
goto :EOF
exit

:CHECKCMAKE
if exist "%CMAKE_HOME%\bin" set PATH=%CMAKE_HOME%\bin;%PATH%
set PATH=%PROT_BUILD_DIR%;%PATH%
set CMAKE_VERSTR=
for /f "tokens=*" %%i in ('cmake --version 2^>NUL ^| find "version"') do set CMAKE_VERSTR=%%i
if not "%CMAKE_VERSTR%"=="" goto STARTBEGIN
echo =============================================
echo, cmake can not be located !
echo, please set CMAKE_HOME to cmake Install dir.
echo =============================================
pause
goto :EOF
exit

:STARTBEGIN
echo %CMAKE_VERSTR%
set _PRESAVE_PATH=%PATH%
call "%VCVAR_FILE%" x86
echo #include ^<windows.h^> >sleep.c
echo int main(int argc, char* argv[]) {if (argc ^> 1) Sleep(1000*atof(argv[1]));} >>sleep.c
cl /MT sleep.c
if not exist sleep.exe goto EXITREL

start "build_x86_dbg" "%~f0" x86 Debug
start "build_x86_rel" "%~f0" x86 Release
start "build_x64_dbg" "%~f0" x64 Debug
start "build_x64_rel" "%~f0" x64 Release
start "build_x86_pack" "%~f0" x86 Package
start "build_x64_pack" "%~f0" x64 Package
:REWAITBUILD
sleep.exe 3
set COUNT=0
for /f "tokens=*" %%i in ('dir /b *_pack.log') do set /a COUNT+=1
if not "%COUNT%"=="0" goto REWAITBUILD
set FAILED=0
for /f %%i in (build_result.log) do if not "%%~i"=="OK" set FAILED=1
set PATH=%_PRESAVE_PATH%
if %FAILED%==1 pause
for /f "tokens=*" %%i in ('dir /b *.zip 2^>NUL') do if exist "%PROT_SRC_DIR%\%%~i" del "%PROT_SRC_DIR%\%%~i"
echo "Moving zip files ..."
move *.zip "%PROT_SRC_DIR%\"
popd
if exist "%PTL_BUILD_DIR%" rd /s /q "%PTL_BUILD_DIR%\"
goto :EOF
exit

:PSUBUILD
set "BUILD_ARCH=%~1"
set "BUILD_TYPE=%~2"
if "%BUILD_TYPE%"=="Package" goto DOPACKAGE
if not "%BUILD_ARCH%"=="x86" if not "%BUILD_ARCH%"=="x64" goto :EOF
set PATH=%_PRESAVE_PATH%
set INCLUDE=
set LIB=
set LIBPATH=
echo %BUILD_ARCH%_%BUILD_TYPE% 1>"%PROT_BUILD_DIR%\%BUILD_ARCH%_%BUILD_TYPE%_run.log"
set BUILD_RESULT=FAILED
md build_%BUILD_ARCH%
cd build_%BUILD_ARCH%
md %BUILD_TYPE%
cd %BUILD_TYPE%
call "%VCVAR_FILE%" %BUILD_ARCH%
cmake "%PROT_SRC_DIR%" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DBUILD_STANDALONE=OFF -DBUILD_STATIC_LIB=ON -DBUILD_UNITTEST=OFF
if not errorlevel 0 goto EXITBUILD
nmake
if not errorlevel 0 goto EXITBUILD
REM nmake test
REM if not errorlevel 0 goto EXITBUILD
set BUILD_RESULT=OK
:EXITBUILD
echo %BUILD_RESULT%>> "%PROT_BUILD_DIR%\build_result.log"
del "%PROT_BUILD_DIR%\%BUILD_ARCH%_%BUILD_TYPE%_run.log"
exit

:DOPACKAGE
echo %BUILD_ARCH% 1>"%PROT_BUILD_DIR%\build_%BUILD_ARCH%_pack.log"
set PACK_RESULT=FAILED
:REWAITPACK
sleep.exe 3
set COUNT=0
for /f "tokens=*" %%i in ('dir /b "%BUILD_ARCH%_*_run.log"') do set /a COUNT+=1
if not "%COUNT%"=="0" goto REWAITPACK
if not exist build_%BUILD_ARCH% goto EXITPACK
cd build_%BUILD_ARCH%
if "%BUILD_ARCH%" == "x86" cmake "%PROT_SRC_DIR%" -G "NMake Makefiles" -DBUILD_TWOPACK=ON -DBUILD_TWOPACK_NAME=win32
if not errorlevel 0 goto EXITPACK
if "%BUILD_ARCH%" == "x64" cmake "%PROT_SRC_DIR%" -G "NMake Makefiles" -DBUILD_TWOPACK=ON -DBUILD_TWOPACK_NAME=win64
if not errorlevel 0 goto EXITPACK
nmake package
if not errorlevel 0 goto EXITPACK
set PACK_RESULT=OK
:EXITPACK
if "%PACK_RESULT%"=="FAILED" pause
del "%PROT_BUILD_DIR%\build_%BUILD_ARCH%_pack.log"
move *.zip "%PROT_BUILD_DIR%"
exit
