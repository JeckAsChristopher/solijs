# solijs

[![npm version](https://img.shields.io/npm/v/sljs?color=blue&label=npm)](https://www.npmjs.com/package/sljs)
[![GitHub release](https://img.shields.io/github/v/release/JeckAsChristopher/solijs?label=release)](https://github.com/JeckAsChristopher/solijs/releases)
[![License](https://img.shields.io/github/license/JeckAsChristopher/solijs)](LICENSE)
[![Issues](https://img.shields.io/github/issues/JeckAsChristopher/solijs)](https://github.com/JeckAsChristopher/solijs/issues)

**solijs** is a prebuilt native Node.js module that links and interacts with Linux `.so` (shared object) files directly from JavaScript using the N-API interface.

This tool allows you to:
- Run functions inside `.so` files (both `void` and `int` returning)
- Capture their output (text or return values)
- Inspect dynamic symbols defined inside the `.so`

> **NOTE:** This is a **pre-release** version. Functionality is stable, but APIs may improve in upcoming versions.

## Update Logs
**Deprecated packges**
1. *1.8.0-prerelease* — due to large size thr deps remove and add something better

**Release packages**
1. *1.9.0-alpha* — removes the deps folder(a big size folder)

2. *2.0.0* — added new features such as outputting arrays, buffers and more! 

3. *2.3.1* — **Now Suppport stdout prompting, and like chatbot!**

> updates coming soon!

---

## Installation

### 1. Install the module

```bash
npm install sljs
```

> **No need for node-gyp!** The native `.node` file is precompiled.

---

## Usage

### Load the module

```js
const sljs = require('sljs');
```

---

### `runText(path, symbol)`

Runs a `void` function from `.so` and captures `stdout`.

```cpp
// C source example (text.c)
#include <stdio.h>
void say_hello() {
    printf("Hello from C shared object!\n");
}
```

**Compile:**

```bash
gcc -fPIC -shared -o libtext.so text.c
```

**Call from JS:**

```js
const output = sljs.runText('./libtext.so', 'say_hello');
console.log(output); // "Hello from C shared object!"
```

---

### `runValue(path, symbol)`

Runs an `int`-returning function and returns the result.

```cpp
// C source example (value.c)
int give_number() {
    return 69420;
}
```

**Compile:**

```bash
gcc -fPIC -shared -o libvalue.so value.c
```

**Call from JS:**

```js
const num = sljs.runValue('./libvalue.so', 'give_number');
console.log(num); // 69420
```

---

### `inspect(path)`

Inspects available dynamic symbols from `.so` using `nm`.

```js
const symbols = sljs.inspect('./libvalue.so');
console.log(symbols); // List of symbol lines (like from `nm -D`)
```

---

## Use Cases

- Custom algorithms written in C/C++ (like hashing or compression)
- Wrapping performant native logic for JS access
- Interfacing low-level Linux `.so` modules
- Fast native extensions without recompiling every install

---

## License

MIT  
Author: Jeck Christopher Anog

---

## Disclaimer

This module does **not** decompile `.so` files. It **exposes and interacts** with dynamic symbols using safe, flexible bindings.

---

## Warning

**DO NOT INSTALL ANY DEPRECATED PACKAGES AS IF DECLARED VULNERABLE TO ANY THREATS!**
