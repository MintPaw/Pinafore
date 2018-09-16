pushd .
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64 >NUL:
popd
msys2_shell.cmd -where . -full-path