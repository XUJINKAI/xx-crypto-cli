# crypto-tool (xx)

WIP: 开发中!

Windows/Linux 密码工具，包装openssl/gmssl常用命令。

## Build

Linux:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

Windows:
```sh
mkdir build-win
cd build-win
cmake ..
cmake --build . --config release
```

## License

MIT © XUJINKAI
