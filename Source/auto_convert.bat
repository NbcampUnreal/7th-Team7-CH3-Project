@echo off
setlocal

echo.
echo ===========================================
echo   C/C++ 파일 인코딩 변환 시작 (재귀적)
echo   CP949 - UTF-8 BOM 포함 (utf8b)
echo ===========================================
echo.

REM .cpp 파일 재귀적 변환
echo -------------------------------------------
echo .cpp 파일 변환 중...
FOR /R . %%i IN (*.cpp) DO (
    echo 변환: "%%i"
    encoding.exe convert "%%i" utf8b
)

echo.

REM .h 파일 재귀적 변환
echo -------------------------------------------
echo .h 파일 변환 중...
FOR /R . %%i IN (*.h) DO (
    echo 변환: "%%i"
    encoding.exe convert "%%i" utf8b
)

echo.
echo ===========================================
echo   모든 파일 변환 완료.
echo ===========================================
echo.

pause
endlocal