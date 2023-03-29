@echo off

set path=%~d0%~p0

:start

"%path%astcenc-avx2.exe" -cl %1 "%~d1%~p1%~n1.astc" 6x6 -medium

shift
if NOT x%1==x goto start
