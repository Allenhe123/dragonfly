# dragonfly
A fast realtime communication and schedule framework

# compile
 - 1. install asio
 - cd src/asio
 - ./autogen.sh
 - ./configure
 - make VERBOSE=1
 - sudo make install
 - 2. install gtest
 - 3. compile
 - cd dragonfly/build
 - cmake .. && make -j8



# test latency
 - inter-process, local machine, sync send & async recv: 60.x - 80.x us
 - inter-process, local machine, async send & async recv: 80.x - 200 us


