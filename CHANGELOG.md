# Chnagelog

## [v1.5.0](https://github.com/spider-all/spider-cplusplus/compare/v1.4.0...v1.5.0) (2021-10-06)

### Dependencies

* Add boost ([005d547](https://github.com/spider-all/spider-cplusplus/commit/005d547952815b31cc767ccc26deae35ab61ce1b))

### Features

* Convert request type to string ([8bb841f](https://github.com/spider-all/spider-cplusplus/commit/8bb841fd7cd83ca9453538d7814be957e63a80dd))
* Add crawler version ([915da04](https://github.com/spider-all/spider-cplusplus/commit/915da04a266d8ffc6f804ecab3de5195e7aa4296))
* Add users repo ([6eeadac](https://github.com/spider-all/spider-cplusplus/commit/6eeadac3d0373c03fd27435e160ff7922aaf5b90))

### Remove files

* Remove useless code ([f7671e8](https://github.com/spider-all/spider-cplusplus/commit/f7671e84b5d869a104c9fb5d3b49a9f534554d45))

## [v1.4.0](https://github.com/spider-all/spider-cplusplus/compare/v1.3.0...v1.4.0) (2021-09-20)

### Bug Fixes

* Fix mongodb batch insert ([44edf0b](https://github.com/spider-all/spider-cplusplus/commit/44edf0bcadff69cdc8a4391514aea1902ce213a3))
* Fix page and limit to int64 ([87b9668](https://github.com/spider-all/spider-cplusplus/commit/87b96683aa4408eb4c59e6f0e10ab35d1df67e81))

### Features

* Fetch data from MySQL ([9d38123](https://github.com/spider-all/spider-cplusplus/commit/9d381233614854d3bfb51708d41b29d20c3167cc))
* Complete mongodb ([38253bc](https://github.com/spider-all/spider-cplusplus/commit/38253bc482b26e1c01c3bef9d36b1dbfed59b9f0))
* Complete mongodb count and upsert ([67965d5](https://github.com/spider-all/spider-cplusplus/commit/67965d50f59a8d286f1fdba723faff2a0932209c))
* Add crawler type ([3832804](https://github.com/spider-all/spider-cplusplus/commit/38328040eae44cefea8b292e8cc5a6928af8ec97))
* Support multi github tokens ([3ebbe02](https://github.com/spider-all/spider-cplusplus/commit/3ebbe02fdfb22c636f02c6a5239ffa52b028c64e))
* Skip sleep on start ([7495da8](https://github.com/spider-all/spider-cplusplus/commit/7495da8db1264b70da8f251aa85b8184f5917b75))
* Add url for users ([9c9213a](https://github.com/spider-all/spider-cplusplus/commit/9c9213ac05118f172bd198af0bf5893509ec3888))

### Release version

* Remove SQLite, pg, leveldb, MySQL ([a3c5888](https://github.com/spider-all/spider-cplusplus/commit/a3c5888c0429f03dc7c578d47a25d330d3ec358d))

## [v1.3.0](https://github.com/spider-all/spider-cplusplus/compare/v1.2.5...v1.3.0) (2021-09-12)

### Bug Fixes

* Fix timezone and request with error ([0fc55bc](https://github.com/spider-all/spider-cplusplus/commit/0fc55bc023318e012baff3b9fd004c9f9eff55f4))

### Features

* Add sleep each request ([6156044](https://github.com/spider-all/spider-cplusplus/commit/6156044315fb3f20d0aa2e78c45b1914b2aa14e3))
* Add mysql, mongodb, postgresql deps ([0b6f4a6](https://github.com/spider-all/spider-cplusplus/commit/0b6f4a64b788669594e31e6a82345814af19c28b))
* Config database ([0798770](https://github.com/spider-all/spider-cplusplus/commit/0798770705c18f7621df4b5a3175eb60894e2580))
* Batch write data to leveldb ([b33b512](https://github.com/spider-all/spider-cplusplus/commit/b33b512e6aef85a67643e8dc8745e6fd83d790cd))
* Add count_x for sqlite ([58283d0](https://github.com/spider-all/spider-cplusplus/commit/58283d07af39b552c05b5d59b1c828a457c1d183))

### Release version

* Add prometheus ([9510c0e](https://github.com/spider-all/spider-cplusplus/commit/9510c0e0f6e9a5566188aab7af01ede0c4bfe4b1))
* Add helm chart ([eb95464](https://github.com/spider-all/spider-cplusplus/commit/eb9546430ceb9418ec2d2c6513f6dc57a77cbd01))
* Remove dynamo ([28cd993](https://github.com/spider-all/spider-cplusplus/commit/28cd993864555d3a8f363ce4cfd62eeb8fc6027b))
* Add abseil ([c0887ab](https://github.com/spider-all/spider-cplusplus/commit/c0887ab030f00308255a3ff294d3273ddb4ed949))

## [v1.2.5](https://github.com/spider-all/spider-cplusplus/compare/v1.2.4...v1.2.5) (2021-09-08)

### Release version

* Add mysql ([671e851](https://github.com/spider-all/spider-cplusplus/commit/671e8515d02af5fbc36b5517a55bdc6f7d93145c))

## [v1.2.4](https://github.com/spider-all/spider-cplusplus/compare/v1.2.3...v1.2.4) (2021-09-07)

### Release version

* Add soci and mysql library ([017ef7d](https://github.com/spider-all/spider-cplusplus/commit/017ef7d61fd20684143112a4476e6296d0e8ef4d))

## [v1.2.3](https://github.com/spider-all/spider-cplusplus/compare/v1.2.2...v1.2.3) (2021-09-06)

### Release version

* Release v1.2.3 ([d63310f](https://github.com/spider-all/spider-cplusplus/commit/d63310ffe6d3bdd25d60863e77f681d31bb505a4))

## v1.2.2 (2021-09-05)

### Code Refactoring

* Change the database class name ([1f39271](https://github.com/spider-all/spider-cplusplus/commit/1f392719697f2b420602896e8bc4c4c0fc4f2da1))
* Refactor the user count return value type ([b81baaf](https://github.com/spider-all/spider-cplusplus/commit/b81baaf438a5a3c0f05c24824f50c4ed1d4d3f46))
* Change the cmake file layout ([3a60ace](https://github.com/spider-all/spider-cplusplus/commit/3a60ace5db852d62cfb6199d4780732aac647503))
* Change cli return type. ([5b74276](https://github.com/spider-all/spider-cplusplus/commit/5b74276853434eda06cbf30f3368771536de1414))
* Rename the database files ([cce323e](https://github.com/spider-all/spider-cplusplus/commit/cce323eb97f8f779e9bf35d5e62e77dd3485a2c8))
* Compile with os lib. ([855eb8a](https://github.com/spider-all/spider-cplusplus/commit/855eb8aa64684c2e76a2aa2c5d67de30908a014f))

### Dependencies

* Remove all of 3rdparty deps. ([01185be](https://github.com/spider-all/spider-cplusplus/commit/01185be7425ddb193bfcfafa6e9064fcee6ad2e0))

### Dependencies

* Upgrade vcpkg to 2021.05.12 ([90fc446](https://github.com/spider-all/spider-cplusplus/commit/90fc446f5653ce37cdc2157dbc6d9b28d8204e6e))
* Upgrade vcpkg to 2021.04.30 ([f637262](https://github.com/spider-all/spider-cplusplus/commit/f637262a2a0bdaaa95c2c73a14bfb15ce19f1003))
* Upgrade deps. ([ce3fac4](https://github.com/spider-all/spider-cplusplus/commit/ce3fac42a229c6db6b43bf8b9532b2e19b3ecdc6))

### Dependencies

* Add cli11 as command line tool ([5afb6f4](https://github.com/spider-all/spider-cplusplus/commit/5afb6f47a65b925378ed4ae0ed73d2d05f46e77f))
* Add aws dynamo sdk ([abb2641](https://github.com/spider-all/spider-cplusplus/commit/abb26413cdcf58bd5c6f8e0428923abddda45390))
* Add spdlog deps. ([73b4f05](https://github.com/spider-all/spider-cplusplus/commit/73b4f055f156e85eb09a45947174b50ee5f07704))
* Add cpp httplib deps. ([c917734](https://github.com/spider-all/spider-cplusplus/commit/c917734922202bb1d1c11a2bb10d54bc351a0d71))
* Add json deps. ([cf6576c](https://github.com/spider-all/spider-cplusplus/commit/cf6576c7c257daf9bc8eed66627f01b296d7c696))
* Add cpr deps. ([0358c9e](https://github.com/spider-all/spider-cplusplus/commit/0358c9e5023529df373f86d9fe1097577b3b45bb))
* Add SQLiteCpp deps. ([ba7481a](https://github.com/spider-all/spider-cplusplus/commit/ba7481a99aa0c570dd9b1078260c311f5cfab416))

### Features

* Add crawler licenses ([141e76b](https://github.com/spider-all/spider-cplusplus/commit/141e76bccdc8c12b5d4e1f34335d1cd2bbd3eaee))
* Add crawler gitignore ([a766e98](https://github.com/spider-all/spider-cplusplus/commit/a766e98c503d9c3b257fe82404cfc7e3b04e90af))
* Add crawler emoji ([89e72ae](https://github.com/spider-all/spider-cplusplus/commit/89e72ae7706dd5082b4b52beca3db23870035a2a))
* Add libpqxx ([fe95d49](https://github.com/spider-all/spider-cplusplus/commit/fe95d49a145f8eedf6d51fc974b118601feca9be))
* Add list orgs ([7dfec21](https://github.com/spider-all/spider-cplusplus/commit/7dfec216bd6810d061604097182b2a370b94a1ba))
* Add mongodb ([55fada9](https://github.com/spider-all/spider-cplusplus/commit/55fada93174f93fad53a20b7f6ebd221d34add9e))
* Set version from package.json ([f6d3e7b](https://github.com/spider-all/spider-cplusplus/commit/f6d3e7bff0b428a9748652c6ce1012c5276be13b))
* Add cpp config file ([7050b05](https://github.com/spider-all/spider-cplusplus/commit/7050b05397185b421543414a7cb0c50b91d584f5))
* Complete dynamo database feature ([eddacf8](https://github.com/spider-all/spider-cplusplus/commit/eddacf88e45c728b363e4a61094ec335899ef058))
* Handle the reset rate limit ([1712a51](https://github.com/spider-all/spider-cplusplus/commit/1712a518029bca2a1382a5fd41573c7d43b1ffaa))
* Show spider version ([7847cb8](https://github.com/spider-all/spider-cplusplus/commit/7847cb83a5513346b75744b5de66eb4b03c6006a))
* Add new config field. ([fe003ff](https://github.com/spider-all/spider-cplusplus/commit/fe003ffde81bafb1bf20a66ad84b6c8045fcbde8))

### Performance Improvements

* Replace boost with httplib. ([cdafa78](https://github.com/spider-all/spider-cplusplus/commit/cdafa7831019e268b41ffc222844dca08a26c858))

### Remove files

* Remove the useless code ([e5b2a34](https://github.com/spider-all/spider-cplusplus/commit/e5b2a34bc497e8c333581c8835707380d904cf1c))
* Remove osx index file ([3502f78](https://github.com/spider-all/spider-cplusplus/commit/3502f784e9f6feb6c57a80507d24140dada80c7f))
* Remove useless defination. ([9eb8741](https://github.com/spider-all/spider-cplusplus/commit/9eb87418e7204d505546747ea2c8d4e537fc05ed))
* Remove useless code. ([f5071eb](https://github.com/spider-all/spider-cplusplus/commit/f5071ebd667a319db3500674f91687350534d49f))
* Remove useless files. ([1684869](https://github.com/spider-all/spider-cplusplus/commit/1684869914a53689cde1e649ca509fd85f69b371))

### Rename files

* Move include dir ([3f8f31b](https://github.com/spider-all/spider-cplusplus/commit/3f8f31b37da2b0440a7393569aef2516dad7d1bb))

