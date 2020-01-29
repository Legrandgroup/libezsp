#!/bin/sh

GCNO_FILES=$(find . -name "*.gcno" | grep CMakeFiles)
LAST=$PWD
for GCNO in $GCNO_FILES
do
  GCDA=$(echo $GCNO | sed 's/\.gcno/\.gcda/g')
  OBJDIR=$(dirname $GCNO)
  SRCDIR=$(dirname $GCNO | sed 's/\/CMakeFiles\/.*\.dir//g')
  GCNOFILE=$(basename $GCNO)
  SRCFILE=$(echo $GCNOFILE | sed 's/\.gcno/\.cpp/')

  cd $OBJDIR; gcov $GCNOFILE ; cd $LAST
  GCOV=$OBJDIR/$SRCFILE.gcov
  #cp $GCOV $SRCDIR
done
if [ "$1" =  "--lcov" ]; then
	lcov --directory . -c -o rapport.info
	genhtml -o ./rapport -t "couverture de code des tests" rapport.info
	firefox ./rapport/index.html
fi
