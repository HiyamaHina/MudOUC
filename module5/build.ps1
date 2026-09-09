$ErrorActionPreference = 'Stop'
Push-Location (Join-Path $PSScriptRoot '../main(测试占位逻辑)')
try {
    & g++ -static -std=c++20 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 -I ../module5 main.cpp game.cpp room.cpp ../module5/CommandParser.cpp ../Player.cpp ../Item.cpp ../SaveManager.cpp -o MudOUC.exe
    if ($LASTEXITCODE -ne 0) { throw 'Game build failed' }
    Write-Host 'Game build passed.'
} finally { Pop-Location }
