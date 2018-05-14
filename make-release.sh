HERE=$(pwd)

## for windows
## CPPFLAGS="-DBOOST_SYSTEM_NO_DEPRECATED"

make clean && \
./autogen.sh && \
./configure LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" --without-gui --enable-hardening --without-miniupnpc --with-incompatible-bdb --disable-tests --disable-gui-tests --disable-bench && \
make -j 4 && \
cd $HERE/src && strip giracoind ; strip giracoin-cli ; strip giracoin-tx && \
#sudo make install && \
rm $HERE/release/* && cp $HERE/src/giracoind $HERE/src/giracoin-cli $HERE/src/giracoin-tx $HERE/release 

exit 0
