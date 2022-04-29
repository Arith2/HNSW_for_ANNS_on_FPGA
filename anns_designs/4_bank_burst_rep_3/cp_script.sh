# $1 -> dir of the new folder, e.g. ../new_folder
mkdir -p $1
cp -r cp_script.sh xrt.ini Makefile design.cfg .gitignore src $1
