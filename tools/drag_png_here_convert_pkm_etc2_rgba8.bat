@echo off

set path=%~d0%~p0

:start

"%path%etcpak.exe" --rgba %1 "%~d1%~p1%~n1.pkm"

shift
if NOT x%1==x goto start
