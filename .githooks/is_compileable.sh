#!/bin/sh

git stash --keep-index
cd src
make
ret=$?
cd ..
git stash apply
exit $ret
