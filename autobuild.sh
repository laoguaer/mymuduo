set -e
if [ ! -d build ]; then
	mkdir build
fi
rm -rf build/*

cd build
cmake ..
make -j 16
cd ..

if [ ! -d /usr/include/mymuduo ]; then
	mkdir /usr/include/mymuduo
fi
rm -f /usr/include/mymuduo/*


for header in `ls include/*.h`
do
	cp $header /usr/include/mymuduo
done


rm -f /usr/lib/libmymuduo.so
cp lib/libmymuduo.so /usr/lib

