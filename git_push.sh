#!/bin/bash
project_path=$(dirname "$(readlink -f "$0")")

echo "You can input 'q' to abort."
read -p "Input Git commit message: " commit_message
if [ "$commit_message" != "q" ]; then
    cd "$project_path"
    git add ./include/*
    git add ./src/*
    git add ./CMakeLists.txt
    git add ./LICENSE
    git add ./README.md
    git add ./.gitignore
    git add ./*.ps1
    git add ./*.sh
    git commit -m "$commit_message"
    git push
fi
