#!/bin/sh

GCNO_FILES=$(find . -name "*.gcno" | grep CMakeFiles)
LAST=$PWD
for GCNO in $GCNO_FILES
do
  GCDA=$(echo $GCNO | sed 's/\.gcno/\.gcda/g')
  OBJDIR=$(dirname $GCNO)
  SRCDIR=$(dirname $GCNO | sed 's/\/CMakeFiles\/.*\.dir//g')
  SRCFILE=$(basename $GCNO | sed 's/\.gcno/.cpp/')
  if [ "$SRCDIR" != $(dirname $GCNO) ]; then
    mkdir -p $SRCDIR
  fi
  cp $GCNO $SRCDIR
  if [ -e $GCDA ]; then
    cp $GCDA $SRCDIR
  fi
  
  if [ "$1" =  "--gcov" ]; then
    cd $SRCDIR; gcov -o $LAST/$GCNO $SRCFILE; cd $LAST
  fi
done
if [ "$1" =  "--lcov" ]; then
	lcov --directory . -c -o rapport.info
	genhtml -o ./rapport -t "couverture de code des tests" rapport.info
	firefox ./rapport/index.html
fi
