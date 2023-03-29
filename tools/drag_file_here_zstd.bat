@echo off

set path=%~d0%~p0

:start

"%path%zstd.exe" --ultra -22 %1

shift
if NOT x%1==x goto start
