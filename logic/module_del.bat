cd %~dp0
echo off

echo "please input module name which u want to delete: "
set /p module_name=

module_creater --del=%module_name%
