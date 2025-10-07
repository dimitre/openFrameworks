chalet bundle
$dist = (Resolve-Path .\dist).Path

# 1. persist to USER scope
$userPath = [Environment]::GetEnvironmentVariable('Path','User')
if ($userPath -split [IO.Path]::PathSeparator -notcontains $dist) {
    [Environment]::SetEnvironmentVariable(
        'Path',
        $userPath + [IO.Path]::PathSeparator + $dist,
        'User'
    )
}

# 2. refresh this process
$env:Path = [Environment]::GetEnvironmentVariable('Path','User')
