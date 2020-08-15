# dragonfly
A fast realtime communication and schedule framework

# compile
cd src/asio
./autogen.sh
./configure
make VERBOSE=1
sudo make install
cd dragonfly/build
cmake .. && make -j8
