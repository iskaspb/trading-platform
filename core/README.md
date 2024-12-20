# Core framework structure

## Configuration
Configuration is an important part of a trading strategy and related applications (e.g. MD capture). The key component of a configuration framework is a mapping between the serialized form of configuration (e.g. JSON) and the internal structures that represent configuration for the C++ code. The answer for the question - "why can't we use directly DOM produced by preferred JSON parser?" is usability. JSON (or any other generic hierarchical data format) may need some data conversion and/or validation before it can be used by the internal components.

There are a number of options for JSON/C++ mapping that are available as public libraries or frameworks. Our default choice is the [nlohmann](https://github.com/nlohmann/json) JSON parser that is both simple to use and feature rich.

## Integration
This is a collection of facilities for internal integration/binding. There are a number of ways to integrate C++ components, starting from basic function calls or object oriented interfaces with virtual functions and continue with generic std::function etc.

Normally these essential ways of code binding is enough on component level. However, on a system level the component integration with basic tools might become a burden. There are two design patterns that help solving this task:
1. "Dynamic interface" or "multi std::function". It's also known by other names : Delegate, [multilambda](https://www.scs.stanford.edu/~dm/blog/param-pack.html#multilambda). In this platform we use name "Gate" (also "GateRef" for its lightweight counterpart);
2. "Assembly/Relay". It's also known as "Pipeline" or "Workflow". This pattern is a deep review or an extension of a popular design pattern [Transaction Script](https://martinfowler.comb/eaaCatalog/transactionScript.html) (unfortunately, every time I see a description of TS on the Web I find that ppl don't understand it and at best just repeat Martin Fowler description).

## Logging
There is no single best logger when you try to reach several goals (high performance, functionality and convenience). For this reason we use differentiated approach to logging.

On general application level we can use any suitable logger which serves as a "structured std::cout". Typical area of application of general logger is initialization code, auxillary functions, and non performance critical code.

We need to be more careful with "critical path" logging. The best answer is not to log at all, however, if the logging is necessary there are options:
1. Log after the critical path - e.g. for the case of processing a market event, we could log when it was received, during processing or after it was processed. The later choice is always preferred for the latency purpose. The problem here is reordering of the log messages, for instance if the processing of an event has several stages (Receive MD -> Update OB -> Create order -> Send order) and we log after every stage is done, than the log messages will be in the opposite order (assuming that all stages are implemented as function that calls next stage);
2. Buffer log messages and dump buffered messages after the processing is done - it's a viable option that tolerates minor overhead of time and space;
3. Some logs are actually traces (e.g. reading CPU timestamp counter), so they can be treated as low-overhead special case;
4. For any type of logging its helpful to separate the stages of logging : data collection, formatting, dumping, and offload as much as possible from the critical path. One big question that helps to improve the latency is "do you really need to log now?" - in many cases it's tolerable to have a gap between the time when the event of interest happened and the time when information about this event reached a log file. Moreover this gap exists even with the naive logging using output to stdout, e.g. `std::cout` is a buffered output, so before `std::flush` we don't know if the data is sent from buffer to the file (stdout), and after that before we `fdatasync` the file descriptor we don't know if OS has sent the data to the actual physical storage. And even after that there is a minor chance that physical device would fail to persist data (e.g. due to power surge).

Apart from performance considerations there are questions that affect usability of a logger : output format, log file management, log rotation, single/multi-threaded logger, ... - they are parts of the idiosyncrasy of a particular logger implementation and is difficult to generalize.

Here is incomplete list of promising high performance loggers available to the public:
1. [spdlog](https://github.com/gabime/spdlog);
2. [binlog](https://github.com/morganstanley/binlog);
3. [NanoLog](https://github.com/PlatformLab/NanoLog) with [explanatory paper](https://www.usenix.org/system/files/conference/atc18/atc18-yang.pdf);

### Logging summary
For a starter we suggest using conventional logger like [boost::log](https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/index.html) and when performance becomes important, then it's time to switch to high perf loggers or some ad-hoc logging.

## Networking
We start by using public network protocol implementations (TCP, HTTP1/2, WebSocket...), e.g. [boost::asio](https://www.boost.org/doc/libs/1_84_0/doc/html/boost_asio.html), [libevent](https://libevent.org/)  etc. Later we add our own (possibly more high performance) implementations.

## Utilities
This is a collection of basic platform utilities, e.g. [ENUM](https://www.scs.stanford.edu/~dm/blog/va-opt.html) macro that simplifies definition of enumerations.

## Performance measurements
The platform has built-in performance trace collector with the tools to produce offline or realtime performance stats