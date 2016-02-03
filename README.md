# ItLightens
Repository for UPF-BSM MCV14 ItLightens

How to get this to your computer:

* Create a folder to be used as repository-directory, for example "reports".
* Right Click and Select Git Gui Here
* Select Clone Repository
* Put HTTPS URL on source
* Select current folder and add /ItLightens at the end
* Click Clone
* When clone is finished, then enter the folder and right click, select Git Bash Here.
* Type "git pull origin master"
* Type "git checkout develop"
* Done!!!!!

How to commit changes:

* Enter the folder and right click, select Git Bash Here.
* Type "git status" to see every changed/added files/folders.
* For any modified file/folder type "git add path-to-file/folder".
  * Can use \* for selecting every modified/new file/folder.
* Make a local commit with "git commit -m 'MESSAGE DESCRIPTION'".
  * If you have to include every changed file with no new files/folders, then can ommit add step and use a -a option in the commit order.
* Assure your files to be pushed with another "git status".
* Make a "git push origin develop".
  * Can "git push --set-upstream origin develop" to make from this time on, only need to use "git push"