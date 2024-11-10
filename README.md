# spider

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/spider-all/spider-cplusplus)

Crawl GitHub APIs and store the discovered orgs, repos, commits...

## Prerequisites

Make sure you have installed all of the following prerequisites on your development machine:

- g++ or clang++ - Compile the project, on MacOS you should install clang with `xcode-select --install`.
- cmake - open-source, cross-platform family of tools designed to build, test and package software.
- make - build automation tool that automatically builds executable programs and libraries from source code by reading files called Makefiles which specify how to derive the target program.
- ccache - speeds up recompilation by caching previous compilations and detecting when the same compilation is being done.
- jq - jq is like sed for JSON data.

### MacOS

``` sh
xcode-select --install
brew install jq ccache cmake make
```

### Ubuntu

``` bash
apt-get install build-essential make cmake ccache jq
```

## Compile

``` bash
make # generate the debug mode executable binary
make debug # same as before
make release # generate the release mode executable binary
```

## Configuration

You can just copy the `config.yaml.sample` to `config.yaml`, `spider` will search the config file in current directory `./etc/config.yaml` by default.

## Parameters

- `--version` or `-v` - Output the spider version, and exit immediately.
- `--version` or `-c` - Set the config file path.

## Database

Now `spider` support `SQLite3`. Database settings are in the `etc/config.yaml.sample`.

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Contact

Tosone - [@itosone](https://twitter.com/itosone) - i@tosone.cn

Project Link: [https://github.com/tosone/spider-cplusplus](https://github.com/tosone/spider-cplusplus)
