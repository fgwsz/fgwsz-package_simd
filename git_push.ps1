$project_path=Split-Path -Parent $MyInvocation.MyCommand.Definition

echo "You can input 'q' to abort."
$commit_message=Read-Host -Prompt "Input Git commit message"
if(!($commit_message -eq "q")){
    cd $project_path
    git add ./include/*
    git add ./src/*
    git add ./CMakeLists.txt
    git add ./LICENSE
    git add ./README.md
    git add ./.gitignore
    git add ./*.ps1
    git add ./*.sh
    git commit -m $commit_message
    git push
}
