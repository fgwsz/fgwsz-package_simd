#!/usr/bin/env pwsh
$project_path=if($PSScriptRoot){$PSScriptRoot}else{Split-Path -Parent $MyInvocation.MyCommand.Path}
$project_name=Split-Path -Leaf $project_path
$target_path=Join-Path $project_path "target/windows"
$debug_output_path=Join-Path $target_path "debug"
$release_output_path=Join-Path $target_path "release"
$debug_build_type="Debug"
$release_build_type="Release"

function build{
    Set-Location $project_path
    cmake -B $debug_output_path -D CMAKE_BUILD_TYPE=$debug_build_type
    cmake --build $debug_output_path --config $debug_build_type
}

function build_release{
    Set-Location $project_path
    cmake -B $release_output_path -D CMAKE_BUILD_TYPE=$release_build_type
    cmake --build $release_output_path --config $release_build_type
}

function run{
    build
    &"$debug_output_path/Debug/$project_name.exe" @args
}

function run_release{
    build_release
    &"$release_output_path/Release/$project_name.exe" @args
}

function clean{
    if(Test-Path $target_path){
        Remove-Item -Recurse -Force $target_path
    }
}

function print_help{
    $script_name=Split-Path -Leaf $PSCommandPath
    @"
Usage: .\$script_name {build|build_release|run|run_release|clean|help}

Commands:
    build               Build Debug version
    build_release       Build Release version
    run [args]          Build Debug version first, then run the executable (optional arguments)
    run_release [args]  Build Release version first, then run the executable (optional arguments)
    clean               Delete the entire target/windows directory
    help                Show this help message

Examples:
    .\$script_name build
    .\$script_name build_release
    .\$script_name run
    .\$script_name run_release
    .\$script_name clean
    .\$script_name help
"@
}

function main{
    param(
        [string]$command,
        [Parameter(ValueFromRemainingArguments=$true)]$remaining_args
    )
    switch($command){
        "build"         {build}
        "build_release" {build_release}
        "run"           {run @remaining_args}
        "run_release"   {run_release @remaining_args}
        "clean"         {clean}
        "help"          {print_help}
        default         {print_help;exit 1}
    }
}

if($MyInvocation.InvocationName -ne'.'){
    main @args
}
