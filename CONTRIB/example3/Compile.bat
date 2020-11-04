@echo off

rem IMPORTANT
set path=C:\CPCDOS\Cpcdos-DEV\DJGPP\bin;%path%

rem [FR] Si vous utilisez un IDE qui utilise les chemin absolue, utilisez ceci :
rem [EN] If you use a IDE, and it uses absolute path, please use this :
rem cd REPERTOIRE\Cpcdos SDK\CONTRIB\REPERTOIRE

rem [FR] Supprimer les anciens fichiers
rem [EN] Delete older files

del *.o
del *.a

echo [FR] Compilation de EXAMPLE3 en cours / [EN] EXAMPLE3 is in compile progress
gpp @example3.args -c example3.cpp -o example3.o
echo example3.o - OK

copy example3.o ..\obj\example3.obj

echo.
echo [FR] Operation termine! / [EN] Operations ended!
pause

