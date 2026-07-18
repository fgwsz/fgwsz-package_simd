#!/bin/bash
project_path=$(dirname "$(readlink -f "$0")")
project_name=$(basename "$project_path")
git_path="$project_path/.git"
user_name="fgwsz"

if [ -d "$git_path" ]; then
    rm -rf "$git_path"
fi
cd $project_path
git init
echo "# $project_name" > README.md
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin "git@github.com:$user_name/$project_name.git"
git push -u origin main
