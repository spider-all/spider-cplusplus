# Chnagelog

## [2.0.0](https://github.com/spider-all/spider-cplusplus/compare/v1.4.0...2.0.0) (2021-09-27)

### Features

* [915da04](https://github.com/spider-all/spider-cplusplus/commit/915da04a266d8ffc6f804ecab3de5195e7aa4296) Add crawler version
* [6eeadac](https://github.com/spider-all/spider-cplusplus/commit/6eeadac3d0373c03fd27435e160ff7922aaf5b90) Add users repo

### Remove files

* [f7671e8](https://github.com/spider-all/spider-cplusplus/commit/f7671e84b5d869a104c9fb5d3b49a9f534554d45) Remove useless code

## [v1.4.0](https://github.com/spider-all/spider-cplusplus/compare/v1.3.0...v1.4.0) (2021-09-20)

### Bug Fixes

* [44edf0b](https://github.com/spider-all/spider-cplusplus/commit/44edf0bcadff69cdc8a4391514aea1902ce213a3) Fix mongodb batch insert
* [87b9668](https://github.com/spider-all/spider-cplusplus/commit/87b96683aa4408eb4c59e6f0e10ab35d1df67e81) Fix page and limit to int64

### Features

* [9d38123](https://github.com/spider-all/spider-cplusplus/commit/9d381233614854d3bfb51708d41b29d20c3167cc) Fetch data from MySQL
* [38253bc](https://github.com/spider-all/spider-cplusplus/commit/38253bc482b26e1c01c3bef9d36b1dbfed59b9f0) Complete mongodb
* [67965d5](https://github.com/spider-all/spider-cplusplus/commit/67965d50f59a8d286f1fdba723faff2a0932209c) Complete mongodb count and upsert
* [3832804](https://github.com/spider-all/spider-cplusplus/commit/38328040eae44cefea8b292e8cc5a6928af8ec97) Add crawler type
* [3ebbe02](https://github.com/spider-all/spider-cplusplus/commit/3ebbe02fdfb22c636f02c6a5239ffa52b028c64e) Support multi github tokens
* [7495da8](https://github.com/spider-all/spider-cplusplus/commit/7495da8db1264b70da8f251aa85b8184f5917b75) Skip sleep on start
* [9c9213a](https://github.com/spider-all/spider-cplusplus/commit/9c9213ac05118f172bd198af0bf5893509ec3888) Add url for users

### Release version

* [a3c5888](https://github.com/spider-all/spider-cplusplus/commit/a3c5888c0429f03dc7c578d47a25d330d3ec358d) Remove SQLite, pg, leveldb, MySQL

## [v1.3.0](https://github.com/spider-all/spider-cplusplus/compare/v1.2.5...v1.3.0) (2021-09-12)

### Bug Fixes

* [0fc55bc](https://github.com/spider-all/spider-cplusplus/commit/0fc55bc023318e012baff3b9fd004c9f9eff55f4) Fix timezone and request with error

### Features

* [6156044](https://github.com/spider-all/spider-cplusplus/commit/6156044315fb3f20d0aa2e78c45b1914b2aa14e3) Add sleep each request
* [0b6f4a6](https://github.com/spider-all/spider-cplusplus/commit/0b6f4a64b788669594e31e6a82345814af19c28b) Add mysql, mongodb, postgresql deps
* [0798770](https://github.com/spider-all/spider-cplusplus/commit/0798770705c18f7621df4b5a3175eb60894e2580) Config database
* [b33b512](https://github.com/spider-all/spider-cplusplus/commit/b33b512e6aef85a67643e8dc8745e6fd83d790cd) Batch write data to leveldb
* [58283d0](https://github.com/spider-all/spider-cplusplus/commit/58283d07af39b552c05b5d59b1c828a457c1d183) Add count_x for sqlite

### Release version

* [9510c0e](https://github.com/spider-all/spider-cplusplus/commit/9510c0e0f6e9a5566188aab7af01ede0c4bfe4b1) Add prometheus
* [eb95464](https://github.com/spider-all/spider-cplusplus/commit/eb9546430ceb9418ec2d2c6513f6dc57a77cbd01) Add helm chart
* [28cd993](https://github.com/spider-all/spider-cplusplus/commit/28cd993864555d3a8f363ce4cfd62eeb8fc6027b) Remove dynamo
* [c0887ab](https://github.com/spider-all/spider-cplusplus/commit/c0887ab030f00308255a3ff294d3273ddb4ed949) Add abseil

## [v1.2.5](https://github.com/spider-all/spider-cplusplus/compare/v1.2.4...v1.2.5) (2021-09-08)

### Release version

* [671e851](https://github.com/spider-all/spider-cplusplus/commit/671e8515d02af5fbc36b5517a55bdc6f7d93145c) Add mysql

## [v1.2.4](https://github.com/spider-all/spider-cplusplus/compare/v1.2.3...v1.2.4) (2021-09-07)

### Release version

* [017ef7d](https://github.com/spider-all/spider-cplusplus/commit/017ef7d61fd20684143112a4476e6296d0e8ef4d) Add soci and mysql library

## [v1.2.3](https://github.com/spider-all/spider-cplusplus/compare/v1.2.2...v1.2.3) (2021-09-06)

### Release version

* [d63310f](https://github.com/spider-all/spider-cplusplus/commit/d63310ffe6d3bdd25d60863e77f681d31bb505a4) Release v1.2.3

## v1.2.2 (2021-09-05)

### Code Refactoring

* [1f39271](https://github.com/spider-all/spider-cplusplus/commit/1f392719697f2b420602896e8bc4c4c0fc4f2da1) Change the database class name
* [b81baaf](https://github.com/spider-all/spider-cplusplus/commit/b81baaf438a5a3c0f05c24824f50c4ed1d4d3f46) Refactor the user count return value type
* [3a60ace](https://github.com/spider-all/spider-cplusplus/commit/3a60ace5db852d62cfb6199d4780732aac647503) Change the cmake file layout
* [5b74276](https://github.com/spider-all/spider-cplusplus/commit/5b74276853434eda06cbf30f3368771536de1414) Change cli return type.
* [cce323e](https://github.com/spider-all/spider-cplusplus/commit/cce323eb97f8f779e9bf35d5e62e77dd3485a2c8) Rename the database files
* [855eb8a](https://github.com/spider-all/spider-cplusplus/commit/855eb8aa64684c2e76a2aa2c5d67de30908a014f) Compile with os lib.

### Features

* [141e76b](https://github.com/spider-all/spider-cplusplus/commit/141e76bccdc8c12b5d4e1f34335d1cd2bbd3eaee) Add crawler licenses
* [a766e98](https://github.com/spider-all/spider-cplusplus/commit/a766e98c503d9c3b257fe82404cfc7e3b04e90af) Add crawler gitignore
* [89e72ae](https://github.com/spider-all/spider-cplusplus/commit/89e72ae7706dd5082b4b52beca3db23870035a2a) Add crawler emoji
* [fe95d49](https://github.com/spider-all/spider-cplusplus/commit/fe95d49a145f8eedf6d51fc974b118601feca9be) Add libpqxx
* [7dfec21](https://github.com/spider-all/spider-cplusplus/commit/7dfec216bd6810d061604097182b2a370b94a1ba) Add list orgs
* [55fada9](https://github.com/spider-all/spider-cplusplus/commit/55fada93174f93fad53a20b7f6ebd221d34add9e) Add mongodb
* [f6d3e7b](https://github.com/spider-all/spider-cplusplus/commit/f6d3e7bff0b428a9748652c6ce1012c5276be13b) Set version from package.json
* [7050b05](https://github.com/spider-all/spider-cplusplus/commit/7050b05397185b421543414a7cb0c50b91d584f5) Add cpp config file
* [eddacf8](https://github.com/spider-all/spider-cplusplus/commit/eddacf88e45c728b363e4a61094ec335899ef058) Complete dynamo database feature
* [1712a51](https://github.com/spider-all/spider-cplusplus/commit/1712a518029bca2a1382a5fd41573c7d43b1ffaa) Handle the reset rate limit
* [7847cb8](https://github.com/spider-all/spider-cplusplus/commit/7847cb83a5513346b75744b5de66eb4b03c6006a) Show spider version
* [fe003ff](https://github.com/spider-all/spider-cplusplus/commit/fe003ffde81bafb1bf20a66ad84b6c8045fcbde8) Add new config field.

### Performance Improvements

* [cdafa78](https://github.com/spider-all/spider-cplusplus/commit/cdafa7831019e268b41ffc222844dca08a26c858) Replace boost with httplib.

### Remove files

* [e5b2a34](https://github.com/spider-all/spider-cplusplus/commit/e5b2a34bc497e8c333581c8835707380d904cf1c) Remove the useless code
* [3502f78](https://github.com/spider-all/spider-cplusplus/commit/3502f784e9f6feb6c57a80507d24140dada80c7f) Remove osx index file
* [9eb8741](https://github.com/spider-all/spider-cplusplus/commit/9eb87418e7204d505546747ea2c8d4e537fc05ed) Remove useless defination.
* [f5071eb](https://github.com/spider-all/spider-cplusplus/commit/f5071ebd667a319db3500674f91687350534d49f) Remove useless code.
* [1684869](https://github.com/spider-all/spider-cplusplus/commit/1684869914a53689cde1e649ca509fd85f69b371) Remove useless files.

### Rename files

* [3f8f31b](https://github.com/spider-all/spider-cplusplus/commit/3f8f31b37da2b0440a7393569aef2516dad7d1bb) Move include dir

