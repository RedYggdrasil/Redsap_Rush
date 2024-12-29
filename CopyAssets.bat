@ECHO OFF
SETLOCAL

REM Define source and target directories
SET SRC_DIR=.\App\Assets
SET TARGET_DIR=.\Build\App\Debug\Assets
SET TARGET_DIR_2=.\Build\App\Release\Assets

REM Create target directory if it does not exist
IF NOT EXIST "%TARGET_DIR%" (
    MKDIR "%TARGET_DIR%"
) ELSE (
    REM Delete existing content in the target directory
    RMDIR /S /Q "%TARGET_DIR%"
    MKDIR "%TARGET_DIR%"
)


REM Move all contents from source to target directory
ROBOCOPY "%SRC_DIR%" "%TARGET_DIR%" /E

REM Check if the operation was successful
IF %ERRORLEVEL% GEQ 8 (
    ECHO An error occurred during the copy operation to Debug.
) ELSE (
    ECHO All files have been copied successfully to Debug.
)


REM Create target directory if it does not exist
IF NOT EXIST "%TARGET_DIR_2%" (
    MKDIR "%TARGET_DIR_2%"
) ELSE (
    REM Delete existing content in the target directory
    RMDIR /S /Q "%TARGET_DIR_2%"
    MKDIR "%TARGET_DIR_2%"
)

REM Move all contents from source to target directory
ROBOCOPY "%SRC_DIR%" "%TARGET_DIR_2%" /E

REM Check if the operation was successful
IF %ERRORLEVEL% GEQ 8 (
    ECHO An error occurred during the copy operation to Release.
) ELSE (
    ECHO All files have been copied successfully to Release.
)

PAUSE
ENDLOCAL
