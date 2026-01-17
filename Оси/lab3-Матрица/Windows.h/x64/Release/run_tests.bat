@echo off
echo n = 100:
Windows.h.exe 100 100
Windows.h.exe 100 64
Windows.h.exe 100 32
Windows.h.exe 100 16
Windows.h.exe 100 8
Windows.h.exe 100 4
Windows.h.exe 100 2

echo.
echo n = 300:
Windows.h.exe 300 300
Windows.h.exe 300 256
Windows.h.exe 300 128
Windows.h.exe 300 64
Windows.h.exe 300 32
Windows.h.exe 300 16
Windows.h.exe 300 8



echo.
echo n = 500:
Windows.h.exe 500 500
Windows.h.exe 500 256
Windows.h.exe 500 128
Windows.h.exe 500 64
Windows.h.exe 500 32
Windows.h.exe 500 16
Windows.h.exe 500 8


echo.
echo Finish
pause