## Applications

This folder contains various applications of the trading platform:
1. Market data capture apps for popular exchanges (Binance, Deribit, Okex, Huobi, ...);
2. Feeder - diagnostic and test tool to check market data and build order book;
3. Example trading strategies
4. ...
5. Sandbox area

## Sandbox area

In order to simplify a creation of new apps, we have a "sandbox" area, see apps/sandbox. You can use it by running `add_sandbox_app.py` script, e.g.:
```
alex@ubun:~/src/trading-platform/build$ ../apps/sandbox/add_sandbox_app.py hello_world
Added sandbox app in folder hello_world
alex@ubun:~/src/trading-platform/build$ make
...
[ 50%] Building CXX object apps/sandbox/hello_world/CMakeFiles/hello_world.dir/main.cpp.o
[ 66%] Linking CXX executable ../../../bin/hello_world
[ 66%] Built target hello_world
...
alex@ubun:~/src/trading-platform/build$ ./bin/hello_world 
Hello, hello_world!
```
Typical use case - when you want to create a new application and you want to have CMakeLists.txt to be generated atomatically for you together with essential dependencies. Please note this folder is exempt from `git commit` (in .gitignore) so you do not accidentelly commit/push your local experiments. You can start your app in sandbox and then move it to more approapriate location.

