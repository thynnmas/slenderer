PLAT=$(uname)
if [ $PLAT == "Linux" ]; then
	FILES=Makefile.*n*
elif [ $PLAT == "Darwin" ]; then
	FILES=Makefile.osx
else
	echo "Unknown operating system, invoke makefiles manually."
	exit
fi
for f in $FILES
do
	echo "Building" $f "."
	make -f $f clean
	make -f $f all
done
