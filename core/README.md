# Core framework structure

## Configuration
Configuration is an important part of a trading strategy and related applications (e.g. MD capture). The key component of a configuration framework is a mapping between the serialized form of configuration (e.g. JSON) and the internal structures that represent configuration for the C++ code. The answer for the question - "why can't we use directly DOM produced by preferred JSON parser?" is usability. JSON (or any other generic hierarchical data format) may need some data conversion and/or validation before it can be used by the internal components. For instance, a field `std::optional<std::string> value` can be represented in JSON in a few ways, e.g. `{"value": null}` or `{"value": ""}` or even `{}`.

There are a number of options for JSON/C++ mapping that are available as public libraries or frameworks. Our default choice is the [nlohmann](https://github.com/nlohmann/json) JSON parser that is both simple to use and feature rich. You can see the basic JSON/C++ mapping example in [sandbox/config_example](../apps/sandbox/config_example/main.cpp)

### How configuration is related to execution environment
An application lifecycle spans over several environments - Dev, CI/CD (build, various test envs), Pre-prod, Prod. Every of such environments potentially may have its own configuration with specific parameters, e.g. a trading strategy as part of integration test may connect to test end-point of an exchange with the set of IP addresses different from prod. Naturally, some parameters of configuration will be different. A typical approach for configuration management is "layered" or "stackable" configuration. Layered configuration consists of a set of config files that are applied on top of each other one by one. The parameters that are absent in the lower layers are added into the resulting config (merged) and parameters that already exist in one or more underlying layers are overridden by the higher layers. This process is similar to inheritance of OOP. Finally, an application has single configuration file specific to selected execution environment.
The same process of "config generation" can be done by the application itself, i.e. we could provide set of config files and rules to merge them into a single config. This intrusive approach doesn't change the result much - in the end, the application would have single view of config parameters tree. The external config generation seems to be less cohesive (for instance what if we want to change config generation - do we need to update the platform code or only the wrapper/startup scripts?).
One important consideration - we should not over-complicate config generation process, and the criterion of complexity in this case is whether by looking at the config files we can understand what parameter would be enabled in which environment.

## Internal integration
This is a collection of facilities for internal integration/binding. There are a number of ways to integrate C++ components, starting from basic function calls or object oriented interfaces with virtual functions and continue with generic std::function etc.

Normally these essential ways of code binding is enough on component level. However, on a system level the component integration with basic tools might become a burden. Any real-world application eventually reaches the point when internal component connectivity becomes too complex and the cost of adding a new feature is more expensive than the implementation of its function. There is no good answer for this problem because this situation happens when the initial assumptions become invalid.

We will use two design patterns that help solving this problem:
1. "Dynamic interface" or "multi std::function". It's also known by other names : Delegate, [multilambda](https://www.scs.stanford.edu/~dm/blog/param-pack.html#multilambda). In this platform we use name "DGate" (also "DGateRef" for its lightweight counterpart);
2. "Assembly/Relay". It's also known as "Pipeline" or "Workflow". This pattern is a deep review or an extension of a popular design pattern [Transaction Script](https://martinfowler.comb/eaaCatalog/transactionScript.html) described by Martin Fowler (unfortunately, every time I see a description of TS on the Web I find that ppl don't understand it and at best just repeat Martin Fowler book).

## Logging
There is no single best logger when you try to reach several goals (high performance, functionality and convenience). For this reason we use differentiated approach to logging.

On a general application level we can use any suitable logger which serves as a "structured std::cout". Typical use case of a general logger is initialization code, auxillary functions, and non performance critical code. A specialized logging solutions are used when its necessary.

<details>
<summary><b>Ways to alleviate the effect of of logging on performance</b></summary>

A trading platform should be more careful with "critical path" logging. The best answer is not to log at all (for many reasons, staring from the fact that usually a general logger contains "global lock" and implicitly synchronizes the app threads), however, if the logging is necessary there are options:
1. To log after the critical path - e.g. for the case of processing a market event, we could log when it was received, during processing or after it was processed. The later choice is always preferred for the latency purpose. The problem here is reordering of the log messages, for instance if the processing of an event has several stages (Receive Market Data -> Update Order Book -> Update Indicators -> Create order -> Send order) and we log after every stage is done, than the log messages will be in the opposite order (assuming that all stages are implemented as function that calls next stage);
2. To buffer log messages and dump buffered messages after the processing is done - it's a viable option that tolerates minor overhead of time and space;
3. Some logs are actually traces (e.g. reading CPU timestamp counter), so they can be treated as low-overhead special case;
4. For any type of logging its helpful to separate the stages of logging : data collection, formatting, dumping, and offload as much as possible from the critical path. One big question that helps to improve the latency is "do you really need to log now?" - in many cases it's tolerable to have a gap between the time when the event of interest happened and the time when information about this event reached a log file. Moreover this gap exists even with the naive logging using output to stdout, e.g. `std::cout` is a buffered output, so before `std::flush` we don't know if the data is sent from buffer to the file (stdout), and after that before we `fdatasync` the file descriptor we don't know if OS has sent the data to the actual physical storage. And even after that there is a minor chance that physical device would fail to persist data (e.g. due to power surge);
5. Another useful idea is that some mutable data is actually constant while application is logging it, e.g. there is no point in getting new timestamp every time you are logging something during critical path.

</details>

Apart from performance considerations there are questions that affect usability of a logger : output format, log file management, log rotation, single/multi-threaded logger, ... - they are parts of the idiosyncrasy of a particular logger implementation and is difficult to generalize.

<details>
<summary><b>More details on logging selection and use cases</b></summary>

The platform will use [boost::log](https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/index.html) library as a general purpose logger (please see [mini-example](../apps/sandbox/logger_example/main.cpp)). For the critical path we will use ad-hoc hybrid solution that is more similar to tracing rather than logging (TBD).
The general app logging system will support 2 levels of logging:
1. The global logger is initialized at the application start:
    ```
    BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(appLogger, src::logger_mt)
    ```
    The global appLogger will have the following format (fields: timestamp, severity, filename:line and log message):
    ```
    2024-11-24 07:37:06.712746 DEBUG FileName.h:43 This is debug message with some data : 123
    2024-11-24 07:37:07.712746 INFO  FileName.h:43 This is info message
    2024-11-24 07:37:08.123456 WARN  FileName.h:43 This is warning message
    2024-11-24 07:37:09.123456 ERROR FileName.h:43 This is error message
    ```
    The global logger will be available via set of convenient macros, e.g.:
    ```
    LOG_DEBUG("This is debug message with some data : " << 1 << 2 << 3);
    LOG_INFO(...)
    LOG_WARN(...)
    LOG_ERROR(...)
    ```
    Another version if the logger macros accepts the logger as the first parameter:
    ```
    auto& logger = appLogger::get(); //..."cache" the reference to global logger so you call singleton every time.
    ...
    LOG_DEBUG(logger, "This is debug message with some data : " << 1 << 2 << 3);
    LOG_INFO(logger, ...)
    LOG_WARN(logger, ...)
    LOG_ERROR(logger, ...)
    ```

The global logger is configured on application level with the options:

* filename/console (default : console)
* rotation policy (default: every 10 megabytes, and on midnight UTC)
* format (default format is described above)

2. A local logger is initialized by any component if it needs to define specific logging format or mode, for example:

* Add a tag for simpler filtering, e.g.:
```
2024-11-24 07:37:07.712746 INFO  [FEEDER] FileName.h:43 This is feeder log message
2024-11-24 07:37:07.712746 INFO  [TRADING] FileName.h:43 This is trading strategy log message
```
* Use single threaded logger for less overhead compare to multi-threaded version;
* Use different severity for the message of specific component. For instance, if we are more interested in detailed logging from a trading strategy (severity : info) and less interested in the feeder logs (severity : warning).

</details>

<details>
<summary><b>(incomplete) list of promising high performance loggers available to the public</b></summary>
These loggers offer higher performance however they are more difficult to use. We can consider them when the platform becomes more mature.

1. [spdlog](https://github.com/gabime/spdlog);
2. [binlog](https://github.com/morganstanley/binlog);
3. [NanoLog](https://github.com/PlatformLab/NanoLog) with [explanatory paper](https://www.usenix.org/system/files/conference/atc18/atc18-yang.pdf);

</details>

From a technical perspective an algo-trading system can be viewed as special case of a logging system both from architectural and functional points of view (typically 99.9...% of a HFT system are logs). However we should not "throw the baby out with the bathwater" - a logging is important but auxiliary function of a trading system.

### Logging summary
For a starter we suggest using conventional logger like [boost::log](https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/index.html) and when performance becomes important, then it's time to switch to high perf loggers or some ad-hoc logging.

## Application
In order to simplify creation of new trading and utility apps and integrate them into common execution environment there are set of helper classes & functions that goes beyond `main(argc, argv)`. They help to find associated configuration file, start all necessary process facilities (e.g. logger) and finally start up a target component (e.g. strategy under strategy runner). You can see an example [app_example](../apps/sandbox/app_example/main.cpp)

### Application config file path resolution sequence
When application is [looking for an associated config](app/configLocator.h) file it follows these steps:

#### Explicit config definition:
1. Anything that is passed as `--config/-c` cmdline parameter is treated as a config file;
    Example: `./app --config /path/to/config.json`
2. If config file is not set via command line, then the application searches for `$APP_CONFIG` environment variable;
    Example: `APP_CONFIG=/path/to/config.json ./app`

#### "Associated" config file lookup:
If the filename is not set via `--config` cmdline argument or ENV then config file by convention has a name
`appName.json`, e.g. if the app name is `binance_md_capture`, then config filename is `binance_md_capture.json`.

There is a way to override this convention by providing appName as a 3rd parameter of locate [locateConfig()](app/configLocator.h)
function. This is done to allow different versions of the same binary, e.g. if the app name is `mm_binance`, however
there is a special build with perf tracing `mm_binance_perf`. They both use the same config file `mm_binance.json`.

The location of a config file `appName.json` is searched in the following order:
1. The folder where the application binary is located;
    Example: application mm_binance is located in the folder `/home/alex/local-run/` and it's executed using full
    path: `$ /home/alex/local-run/mm_binance`
    The config file is searched in the same folder: `/home/alex/local-run/mm_binance.json`
    Please note that this may not be the same as the current working directory (CWD).
2. The folder defined by `$APP_CONFIG_DIR` environment variable;
    Example: `APP_CONFIG_DIR=/path/to/config ./mm_binance`
    The config file is searched in the folder `/path/to/config/mm_binance.json`

## Networking and I/O
We start by using public I/O and network protocol implementations (TCP, HTTP1/2, WebSocket...), e.g. [boost::asio](https://www.boost.org/doc/libs/1_84_0/doc/html/boost_asio.html), [POCO::Net](https://docs.pocoproject.org/current/Poco.Net.html), [libevent](https://libevent.org/)  etc. Later we add our own (possibly higher performance) implementations.

## Utilities
This is a collection of basic platform utilities, e.g. [ENUM](https://www.scs.stanford.edu/~dm/blog/va-opt.html) macro that simplifies definition of enumerations.

## Performance measurement
The platform has built-in performance trace collector with the tools to produce offline or realtime performance stats
