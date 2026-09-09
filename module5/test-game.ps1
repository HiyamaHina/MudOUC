# 测试在临时目录中创建存档，不会修改真实游戏进度。
$ErrorActionPreference = 'Stop'
$gameExe = (Resolve-Path (Join-Path $PSScriptRoot '../main(测试占位逻辑)/MudOUC.exe')).Path
$testFolder = Join-Path ([IO.Path]::GetTempPath()) ('mud-campus-check-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $testFolder | Out-Null
function Run-Game([string[]] $commands) {
    $result = ((@('') + $commands) -join "`n") | & $gameExe
    if ($LASTEXITCODE -ne 0) { throw 'Game failed' }
    return ($result | Out-String)
}
function Check([bool] $ok,[string] $label) { if(!$ok) { throw "FAIL: $label" }; Write-Host "PASS: $label" }
function Field([string] $name) {
    return [int]((Get-Content save.txt | Where-Object { $_ -like "$name|*" }) -split '\|')[1]
}
function Fixture([int] $room=2,[int] $route=2,[int] $event=0,[int] $hp=0) {
    @('NAME|Hero','HP|100','MAXHP|100','SHIELD|0','BASEATTACK|10','GOLD|100','CR|0','CD|50') | Set-Content save.txt -Encoding utf8
    @("ROOM|$room",("STAGE|"+($room-1)),'VERSION|3',"ROUTE|$route","ENEMYHP|$hp","EVENT|$event") | Set-Content save.txt.progress -Encoding utf8
}
Push-Location $testFolder
try {
    $s=Run-Game @('abc','123','1','2','0','4')
    Check ($s.Contains('学业初航') -and $s.Contains('/ 11') -and $s.Contains('输入无效') -and !$s.Contains('[选择路线]')) 'menu and input validation'
    Check ($s.Contains('前情提要') -and $s.Contains('汝乃丙午年') -and $s.Contains('准备好了吗') -and $s.IndexOf('前情提要') -lt $s.IndexOf('学业初航')) 'original story precedes gameplay'
    Check ($s -notmatch '武器|经验|等级|Lv\.') 'removed equipment and experience display'
    Check ($s.Contains('学海行者') -and $s.Contains('160/160') -and $s.Contains('攻击 20') -and $s.Contains('金币 20') -and $s.Contains('背包 0 件') -and $s.Contains('背包为空')) 'leader initial stats and empty inventory'
    Fixture 2 1 0 45
    (Get-Content save.txt) -replace '^GOLD\|.*','GOLD|0' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','4','1','5','0')
    Check ((Field HP) -eq 92 -and ((Get-Content save.txt.progress) -contains 'ENEMYHP|35') -and $s.Contains('敌人挡住')) 'campus enemy and single round'
    Check (!(Get-Content save.txt | Where-Object { $_ -match '^(LEVEL|EXP|WEAPON)\|' })) 'save excludes removed attributes'
    $s=Run-Game @('6','1','5','0')
    Check ((Field HP) -eq 84 -and ((Get-Content save.txt.progress) -contains 'ENEMYHP|25')) 'battle save/load'
    $s=Run-Game @('6','1','1','1','1','5','0')
    Check ((Field GOLD) -eq 12 -and (Field HP) -eq 68) 'reward once and no counterattack after defeat'

    # 每种事件两条分支，验证实际属性；没有暴击字段。
    $expected=@(
      @(1,1,100,10,80,10,100), @(1,2,100,25,60,10,100),
      @(2,1,100,10,90,10,100), @(2,2,90,0,90,10,100),
      @(3,1,90,0,100,14,100), @(3,2,100,0,110,10,100),
      @(4,1,50,0,100,20,100), @(4,2,100,0,130,10,100),
      @(5,1,100,0,140,7,100), @(5,2,100,0,118,10,100),
      @(6,1,50,50,100,10,100), @(6,2,100,0,100,10,100),
      @(7,1,100,0,95,10,120), @(7,2,100,0,95,11,100),
      @(8,1,100,0,50,10,100), @(8,2,65,0,100,10,100)
    )
    foreach($e in $expected) {
        Fixture 2 2 $e[0] 0
        $s=Run-Game @('6','4',([string]$e[1]),'5','6','5','0')
        Check ((Field HP)-eq $e[2] -and (Field SHIELD)-eq $e[3] -and (Field GOLD)-eq $e[4] -and (Field BASEATTACK)-eq $e[5] -and (Field MAXHP)-eq $e[6]) "event $($e[0]) choice $($e[1]), reload does not repeat effect"
    }
    Fixture 2 2 1 0
    (Get-Content save.txt) -replace '^GOLD\|.*','GOLD|0' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','4','1','5','0')
    Check ((Field SHIELD)-eq 0 -and (Field GOLD)-eq 0) 'insufficient funds'
    $s=Run-Game @('4','2')
    Check ($s.Contains('校园奇遇') -and $s.Contains('输入已结束')) 'random event and EOF'

    Fixture 4 0 0 0
    $s=Run-Game @('6','4','1','1','0','5','0')
    Check ($s.Contains('[教育超市]') -and ((Get-Content save.txt | Where-Object {$_ -like 'ITEM|*|campus|*'}).Count -eq 1)) 'random shop purchase goes into bag'
    $s=Run-Game @('6','3','1','5','0')
    Check (!(Get-Content save.txt | Where-Object {$_ -like 'ITEM|*|campus|*'})) 'campus consumable use'

    # 强化测试角色仅用于遍历全部11个房间，不代表默认角色平衡测试。
    Fixture 1 0 0 0
    (Get-Content save.txt) -replace '^HP\|.*','HP|5000' -replace '^MAXHP\|.*','MAXHP|5000' -replace '^BASEATTACK\|.*','BASEATTACK|200' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','4','1','1','4','1','1','4','1','4','1','0','4','1','1','4','1','4','1','0','4','1','1','4','4','1','5','0')
    Check ($s.Contains('恭喜通关') -and (Field GOLD)-eq 372 -and ((Get-Content save.txt.progress)-contains 'ROOM|11')) 'all 11 rooms, 7 battles, both shops and rest'
    Fixture 10 0 0 0
    (Get-Content save.txt) -replace '^HP\|.*','HP|1' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','4','1','4','3','5','0')
    Check ((Field HP)-eq 0 -and $s.Contains('战败后不能使用物品')) 'defeat restrictions'
    Fixture 2 2 0 0
    (Get-Content save.txt.progress) -replace 'VERSION\|3','VERSION|2' | Set-Content save.txt.progress -Encoding utf8
    $s=Run-Game @('6','0')
    Check ($s.Contains('不兼容')) 'old map save rejected'
    Fixture 2 1 0 45
    (Get-Content save.txt) -replace '^CR\|.*','CR|100' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','1','5','0')
    Check ($s.Contains('灵光乍现') -and ((Get-Content save.txt.progress)-contains 'ENEMYHP|30') -and (Field CR)-eq 100 -and (Field CD)-eq 50) 'guaranteed critical deals 150% and persists'
    Fixture 2 1 0 45
    $s=Run-Game @('6','1','5','0')
    Check (!$s.Contains('灵光乍现') -and ((Get-Content save.txt.progress)-contains 'ENEMYHP|35')) 'zero critical rate'
    Fixture 2 2 3 0
    (Get-Content save.txt) -replace '^CR\|.*','CR|98' | Set-Content save.txt -Encoding utf8
    $s=Run-Game @('6','4','1','5','6','5','0')
    Check ((Field CR)-eq 100) 'event critical rate capped at 100 and restored'
    Fixture 2 2 3 0
    $s=Run-Game @('6','4','2','5','0')
    Check ((Field CD)-eq 60) 'event critical damage increase'
    Fixture 2 2 4 0
    $s=Run-Game @('6','4','1','5','0')
    Check ((Field CR)-eq 10 -and (Field BASEATTACK)-eq 20) 'fire event critical increase'
    Write-Host 'All campus checks passed.'
} finally { Pop-Location }
