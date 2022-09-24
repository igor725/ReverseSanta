@echo off

@rem Создаём локальную копию окружения
setlocal
cls

@rem Узнаём целевую архитектуру
set ARCH=%VSCMD_ARG_TGT_ARCH%
set DEBUG=1

@rem Установка переменных проекта
set WARNLEVEL=4
set SRC=src\*.cc src\runners\*.cc src\runners\editor\*.cc src\runners\game\*.cc ^
imgui\backends\imgui_impl_dx9.cpp imgui\backends\imgui_impl_win32.cpp imgui\imgui*.cpp
set DEFINES=/DWIN32_LEAN_AND_MEAN /DUNICODE /DDIRECTINPUT_VERSION=0x0800
set LIBS=user32.lib d3d9.lib dinput8.lib dxguid.lib shell32.lib dbghelp.lib
set OUTNAME=se-%ARCH%.exe

@rem Добавление собственных путей поиска lib и h файлов
set INCLUDE=%INCLUDE%;%DXSDK_DIR%Include;src\;imgui\;imgui\backends\
set LIB=%LIB%;%DXSDK_DIR%Lib\%ARCH%

@rem Настройка переменных окружения компилятора
set _CL_=/Feout/%OUTNAME% /Foobjs\%ARCH%\ /FC /W%WARNLEVEL% %DEFINES%
set CL=/EHsc /MP /nologo /wd4127
set LINK=/subsystem:Windows /incremental:no

IF "%DEBUG%"=="1" (
	set CL=%CL% /MTd /Zi /Od /DD3D_DEBUG_INFO
	set LIBS=%LIBS% d3dx9d.lib
) else (
	set CL=%CL% /MT /O2
	set LINK=%LINK% /RELEASE
	set LIBS=%LIBS% d3dx9.lib
)

@rem Проверка наличия папок для объектных и скомпилированных файлов
mkdir out 2> nul
mkdir objs\%ARCH% 2> nul

@rem Запуск процесса компиляции
cl %SRC% %LIBS%

@rem Проверка успешности компиляции и запуск выходной программы
@rem в случае отсутствия ошибок
IF NOT "%ERRORLEVEL%" == "0" goto error
echo Compilation OK

IF "%1" == "run" (
	pushd out
	%OUTNAME%
)

:done
endlocal
EXIT /B 0

:error
endlocal
EXIT /B 1
