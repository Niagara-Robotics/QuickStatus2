# run with powershell -ExecutionPolicy Bypass -File package.ps1
$build = "$PSScriptRoot\build"

Compress-Archive -Force -Path `
    "$build\platforms", `
    "$build\styles", `
    "$build\ntcore.dll", `
    "$build\qt.conf", `
    "$build\Qt6Core.dll", `
    "$build\Qt6Gui.dll", `
    "$build\Qt6Svg.dll", `
    "$build\Qt6Widgets.dll", `
    "$build\QuickStatus2.exe", `
    "$build\README.txt", `
    "$build\wpiHal.dll", `
    "$build\wpilibc.dll", `
    "$build\wpimath.dll", `
    "$build\wpinet.dll", `
    "$build\wpiutil.dll" `
    -DestinationPath "$build\QuickStatus2.zip"

Write-Host "Done! QuickStatus2.zip created."