# xx-crypto-cli

Windows/Linux Crypto CLI Toolbox. 跨平台密码工具箱。

:warning: WIP: ONLY a few commands work. 开发中！仅部分功能可用。

## Build

Run `git submodule update --init` to fetch the submodules.

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

## Links

[cmdparser](https://github.com/XUJINKAI/cmdparser)

[GmSSL](https://github.com/guanzhi/GmSSL)

[nist_sts](https://github.com/ZZMarquis/nist_sts)

## License

Copyright 2022 XUJINKAI/xx-crypto-cli. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
