set f1="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
set f2="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
if exist %f1% ( call %f1% ) else ( call %f2% ) 

msbuild.exe /clp:WarningsOnly /m /nologo /p:Configuration=Release /p:Platform=x64 /v:d /t:rebuild vs2017-openslp.sln
