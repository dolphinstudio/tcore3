cd %~dp0
echo off

echo "please input auther : "
set /p auther=
echo "please input module name : "
set /p module_name=
echo "please input group : "
set /p group=

module_creater --name=%module_name% --auther=%auther% --group=%group%
