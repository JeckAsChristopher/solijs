{
  "targets": [
    {
      "target_name": "sljs",
      "sources": [ "libs/core.cpp" ],
      "include_dirs": [
        "<!(node -p \"require('node:path').join(process.execPath, '..', '..', 'include', 'node')\")",
        "node_modules/node-addon-api"
      ],
      "cflags_cc": [
        "-O2",
        "-std=c++17",
        "-fPIC"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "libraries": [ "-ldl" ]
    }
  ]
}
