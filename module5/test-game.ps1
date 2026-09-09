# 测试现有游戏程序，不增加任何 main.cpp。存档全部放在临时目录。
$ErrorActionPreference = 'Stop'
$gameExe = (Resolve-Path (Join-Path $PSScriptRoot '../main(测试占位逻辑)/MudOUC.exe')).Path
$testFolder = Join-Path ([IO.Path]::GetTempPath()) ('mud-game-check-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $testFolder | Out-Null
function Run-Game([string[]] $commands) {
    $result = ($commands -join "`n") | & $gameExe
    if ($LASTEXITCODE -ne 0) { throw 'Game exited with an error' }
    return ($result | Out-String)
}
function Check([bool] $condition, [string] $label) {
    if (!$condition) { throw "FAIL: $label" }
    Write-Host "PASS: $label"
}
Push-Location $testFolder
try {
    $result = Run-Game @('abc','123','1','2','0','4')
    Check ($result.Contains('输入无效') -and $result.Contains('当前没有敌人') -and $result.Contains('[背包]') -and !$result.Contains('[选择路线]')) 'invalid input, safe room, inventory and exit'
    Check ($result.Contains('Hero  Lv.1  生命 100/100') -and $result.Contains('1 攻击') -and $result.Contains('2 查看背包')) 'room header and numbered menu'

    $result = Run-Game @('4','1','4','1','5','0')
    Check ($result.Contains('敌人挡住了出口')) 'cannot leave during battle'
    Check ((Get-Content save.txt) -contains 'HP|95') 'enemy counterattack damages player'
    Check ((Get-Content save.txt.progress) -contains 'ENEMYHP|20') 'attack damages enemy and save records it'

    $result = Run-Game @('6','1','5','0')
    Check (((Get-Content save.txt) -contains 'HP|90') -and ((Get-Content save.txt.progress) -contains 'ENEMYHP|10')) 'load resumes the same battle'
    $result = Run-Game @('6','1','1','5','0')
    Check (((Get-Content save.txt) -contains 'HP|90') -and ((Get-Content save.txt) -contains 'GOLD|10')) 'dead enemy cannot counterattack or reward twice'

    $result = Run-Game @('4','1','1','3','1','5','0')
    Check (((Get-Content save.txt) -contains 'HP|100') -and !((Get-Content save.txt) -match '^ITEM\|治疗药水\|')) 'healing applies and consumes item'
    $result = Run-Game @('3','2','4','1','1','5','0')
    Check (((Get-Content save.txt) -contains 'HP|100') -and ((Get-Content save.txt) -contains 'SHIELD|10')) 'shield absorbs enemy damage'

    # 完整路线：普通敌人3次、小Boss5次、补给、休整、最终Boss8次。
    $commands = @('4','1') + (@('1') * 3) + @('4') + (@('1') * 5) + @('4','2','4','4') + (@('1') * 8) + @('5','0')
    $result = Run-Game $commands
    Check ($result.Contains('恭喜通关') -and ((Get-Content save.txt.progress) -contains 'ENEMYHP|0')) 'full game reaches victory'
    Check ((Get-Content save.txt) -contains 'GOLD|95') 'battle and event rewards applied once'

    # 第二条分支：补给路线、小Boss、商店购买。
    $commands = @('4','2','4') + (@('1') * 5) + @('4','1','1','5','0')
    $result = Run-Game $commands
    Check ($result.Contains('[商店]') -and ((Get-Content save.txt) -contains 'GOLD|25')) 'alternate route and shop purchase'

    # 用独立测试存档构造濒死状态，验证死亡不能继续行动。
    $null = Run-Game @('4','1','1','5','0')
    (Get-Content save.txt) -replace '^HP\|.*$', 'HP|1' | Set-Content save.txt -Encoding utf8
    $result = Run-Game @('6','1','4','3','5','0')
    Check ($result.Contains('你已战败') -and $result.Contains('战败后不能使用物品') -and ((Get-Content save.txt) -contains 'HP|0')) 'defeat blocks movement and item use'
    $result = Run-Game @('4')
    Check ($result.Contains('输入已结束')) 'end of input does not loop forever'
    $result = Run-Game @('4','0','5','0')
    Check ((Get-Content save.txt.progress) -contains 'ROOM|1') 'cancel branch keeps current room'
    Write-Host 'All game checks passed.'
} finally { Pop-Location }
