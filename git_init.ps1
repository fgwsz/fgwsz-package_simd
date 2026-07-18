$project_path=Split-Path -Parent $MyInvocation.MyCommand.Definition
$project_name=Split-Path -Leaf $project_path
$git_path=Join-Path -Path $project_path -ChildPath ".git"
$user_name="fgwsz"

if(Test-Path -Path $git_path){
    Remove-Item $git_path -Force -Recurse
}
cd $project_path
git init
echo "# $project_name" > README.md
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin "git@github.com:$user_name/$project_name.git"
git push -u origin main
