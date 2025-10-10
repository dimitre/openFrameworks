$bash = "${env:ProgramFiles}\Git\bin\bash.exe"
if (!(Test-Path $bash)) {
    Write-Error "Git Bash not found at $bash"; exit 1
}
# $PSScriptRoot = folder that contains this .ps1
& $bash "$PSScriptRoot\libs.sh" @args
