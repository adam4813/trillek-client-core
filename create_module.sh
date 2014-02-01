#!/bin/bash
# Original script from : http://simplex-engine.com/2013/08/31/simplex-update-1-extreme-modularization-with-cmake-and-git/

#
# This script is used to create a module for simplex engine
# use:
# ./bin/create-module [module-name]
#
# NOTE: the script will add the 'simplex-' prefix to the module name
# so you should not add it or you will end up having a simplex-simplex- prefix.
#
NAME=trillek-$1

#
# We have one repository per module, so after creating the repository in
# BitBucket, I add it as a submodule of my main repository.
# In this way, we can rebuild the whole directory structure for the engine
# using git submodule init, instead of having to download each module by hand.
# This is also useful if you want to avoid using some module and customize what
# gets built into your final application. Just remove or add the modules you want
# to your root repository.
git submodule add git@github.com:trillek-team//$NAME.git modules/$NAME
cd modules/$NAME
git checkout -b master

#
# In order to work consistently with the module, we need to create
# a standard directory structure.
DIRECTORIES="src include tests samples docs third-party"
for DIRECTORY in $DIRECTORIES
do
	mkdir $DIRECTORY
	# Git doesn't include directories if they don't have content inside.
	# So we create a file called .gitkeep in order to create the directories
	# in th remote repository.
	touch $DIRECTORY/.gitkeep
done

#
# Just have something show up on BitBucket, we can create an empty
# README.md
echo -e "Module $NAME\n===" > README.md

#
# This is an empty CMakeLists.txt for the module.
# All the specifics for creating the library and building its
# tests should go here.
echo -e "cmake_minimum_required(VERSION 2.8.5)\nproject($NAME)" > CMakeLists.txt

#
# Now we need to push these default changes as an initial commit.
# After this we can start working on the module.
git add .
git commit -m "[Module] Initial commit for $NAME" -a
git push origin master
cd ..
cd ..

