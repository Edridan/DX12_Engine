:: copy data to the right direction
if not exist "..\DX12_Engine" (
goto :Error_wrongplace
)

if "%project_dir%" NEQ "" (	
if not exist "%project_dir%\resources" mkdir "%project_dir%\resources"

@echo on
echo Copy the resource to the build folder
xcopy /Y /H /E ..\DX12_Engine\resources\*  %project_dir%\resources
@echo off

) else (
goto :Error_projectdir
)


:: correct end of execution
goto :eof


:: Errors
:Error_wrongplace
echo Error the script file is not a the right place
goto :eof

:Error_projectdir
echo Error the project_dir var is not set correctly
goto :eof