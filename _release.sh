#!/bin/sh

BUILD_DIR=tmp_build

rm -f prot-*.tar.gz
if [[ -d "${BUILD_DIR}" ]] ; then
	rm -rf "${BUILD_DIR}"
fi
mkdir "${BUILD_DIR}"

cd "${BUILD_DIR}"
mkdir Debug
cd Debug
cmake ../.. -DBUILD_STANDALONE=OFF -DBUILD_UNITTEST=OFF -DCMAKE_BUILD_TYPE=Debug
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 1
fi
make
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 2
fi
cd ..

mkdir Release
cd Release
cmake ../.. -DBUILD_STANDALONE=OFF -DBUILD_UNITTEST=OFF -DCMAKE_BUILD_TYPE=Release
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 3
fi
make
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 4
fi
# make test
# if [[ 0 -ne $? ]]; then
# echo "- make init failed."
# exit 5
# fi
cd ..

if [[ ! -d Debug || ! -d Release ]]; then
exit 7
fi

BUILD_OS_NAME=`head -n 1 /etc/issue | cut -d " " -f1`
BUILD_PROCESS=`uname -p`
if [[ ! -z "${BUILD_PROCESS}" ]]; then
BUILD_OS_NAME="${BUILD_OS_NAME}-${BUILD_PROCESS}"
fi

cmake .. -DBUILD_TWOPACK=ON -DBUILD_TWOPACK_NAME=${BUILD_OS_NAME}
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 8
fi
make package
if [[ 0 -ne $? ]]; then
echo "- make init failed."
exit 9
fi

mv *.tar.gz ../
cd ..
if [[ -d "${BUILD_DIR}" ]]; then
	rm -rf "${BUILD_DIR}"
fi
