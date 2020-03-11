@echo off
setlocal enabledelayedexpansion
set /a c=1
:loop
    echo Counter: !c!
    cmd /c .\out\enmon.exe
    if %errorlevel% neq 0 goto error
    set /a c=c+1
    goto loop
:error
    pause
