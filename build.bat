@echo off

@rem Создаём локальную копию окружения
setlocal
cls

@rem Узнаём целевую архитектуру
set ARCH=%VSCMD_ARG_TGT_ARCH%

@rem Установка переменных проекта
set WARNLEVEL=4
set SRC=src\*.cc src\game\*.cc src\editor\*.cc ^
imgui\backends\imgui_impl_dx9.cpp imgui\backends\imgui_impl_win32.cpp imgui\imgui*.cpp
set DEFINES=/DWIN32_LEAN_AND_MEAN /DUNICODE /DD3D_DEBUG_INFO
set LIBS=user32.lib d3d9.lib d3dx9.lib dinput8.lib ^
dxguid.lib shell32.lib dbghelp.lib
set OUTNAME=se-%ARCH%.exe

@rem Добавление собственных путей поиска lib и h файлов
set INCLUDE=%INCLUDE%;%DXSDK_DIR%Include;src\;imgui\;imgui\backends\
set LIB=%LIB%;%DXSDK_DIR%Lib\%ARCH%

@rem Настройка переменных окружения компилятора
set _CL_=/Feout/%OUTNAME% /Foobjs\%ARCH%\ /FC /W%WARNLEVEL% %DEFINES%
set CL=/EHsc /MP /MTd /Gm- /Zi /Od /nologo /fp:fast
set LINK=/subsystem:Windows /incremental:no

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
