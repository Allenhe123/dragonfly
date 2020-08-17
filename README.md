# dragonfly
A fast realtime communication and schedule framework

# compile
 - cd src/asio
 - ./autogen.sh
 - ./configure
 - make VERBOSE=1
 - sudo make install
 - cd dragonfly/build
 - cmake .. && make -j8



# test latency
 - sync send & async recv:
recv: 0 sync hello server, 0 times 1597677304392695551, delta: 80.014999 us
size: 40, type:1
recv: 1 sync hello server, 1 times 1597677304392726499, delta: 68.367996 us
size: 40, type:1
recv: 2 sync hello server, 2 times 1597677304392736045, delta: 68.589996 us
size: 40, type:1
recv: 3 sync hello server, 3 times 1597677304392742660, delta: 74.127998 us
size: 40, type:1
recv: 4 sync hello server, 4 times 1597677304392748957, delta: 77.352997 us
size: 40, type:1
recv: 5 sync hello server, 5 times 1597677304392756923, delta: 79.030998 us
size: 40, type:1
recv: 6 sync hello server, 6 times 1597677304392767450, delta: 76.082001 us
size: 40, type:1
recv: 7 sync hello server, 7 times 1597677304392774727, delta: 78.252998 us
size: 40, type:1
recv: 8 sync hello server, 8 times 1597677304392780764, delta: 81.664001 us
size: 40, type:1
recv: 9 sync hello server, 9 times 1597677304392789835, delta: 82.420998 us


 - async send & async recv:
recv: 0 async hello server, 0 times 1597677304392924142, delta: 89.660004 us
size: 41, type:1
recv: 1 async hello server, 1 times 1597677304392929940, delta: 100.263000 us
size: 41, type:1
recv: 2 async hello server, 2 times 1597677304392931355, delta: 113.252998 us
size: 41, type:1
recv: 3 async hello server, 3 times 1597677304392932497, delta: 124.765999 us
size: 41, type:1
recv: 4 async hello server, 4 times 1597677304392933672, delta: 135.923004 us
size: 41, type:1
recv: 5 async hello server, 5 times 1597677304392934750, delta: 149.289001 us
size: 41, type:1
recv: 6 async hello server, 6 times 1597677304392935850, delta: 160.916000 us
size: 41, type:1
recv: 7 async hello server, 7 times 1597677304392937128, delta: 173.207001 us
size: 41, type:1
recv: 8 async hello server, 8 times 1597677304392938299, delta: 185.279007 us
size: 41, type:1
recv: 9 async hello server, 9 times 1597677304392939455, delta: 198.847000 us


