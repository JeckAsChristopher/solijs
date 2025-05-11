#include <napi.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <memory>
#include <cstdio>

#define BUFFER_SIZE 4096

// Global JS logger
Napi::FunctionReference jsStdoutLogger;

// Register a JS logger callback
Napi::Value SetStdoutLogger(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!info[0].IsFunction()) {
        Napi::TypeError::New(env, "Expected a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    jsStdoutLogger = Napi::Persistent(info[0].As<Napi::Function>());
    return env.Undefined();
}

// Call JS logger from C++
void logToJs(const std::string& message) {
    if (!jsStdoutLogger.IsEmpty()) {
        jsStdoutLogger.Call({ Napi::String::New(jsStdoutLogger.Env(), message) });
    }
}

// Utility: capture stdout from any void function
std::string captureStdout(const std::function<void()>& func) {
    if (!jsStdoutLogger.IsEmpty()) {
        func(); // Output will go to JS via logToJs
        return "[output sent via jsStdoutLogger]";
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) return "pipe creation failed";

    int stdout_fd = dup(STDOUT_FILENO);
    if (stdout_fd == -1) return "dup failed";

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) return "dup2 failed";
    close(pipefd[1]);

    func();

    fflush(stdout);
    dup2(stdout_fd, STDOUT_FILENO);
    close(stdout_fd);

    char buffer[BUFFER_SIZE];
    ssize_t count = read(pipefd[0], buffer, BUFFER_SIZE - 1);
    close(pipefd[0]);
    buffer[count > 0 ? count : 0] = '\0';

    return std::string(buffer);
}

// Mode 1: Run void function and capture text output
std::string executeTextSymbol(const std::string& soPath, const std::string& symbolName) {
    void* handle = dlopen(soPath.c_str(), RTLD_LAZY);
    if (!handle) return "dlopen error: " + std::string(dlerror());

    void (*func)();
    *(void**)(&func) = dlsym(handle, symbolName.c_str());
    if (!func) {
        std::string err = dlerror();
        dlclose(handle);
        return "symbol error: " + err;
    }

    std::string output = captureStdout(func);
    dlclose(handle);
    return output;
}

// Mode 2: Run int-returning function
int executeValueSymbol(const std::string& soPath, const std::string& symbolName, bool& ok) {
    void* handle = dlopen(soPath.c_str(), RTLD_LAZY);
    if (!handle) {
        ok = false;
        return -1;
    }

    int (*func)();
    *(void**)(&func) = dlsym(handle, symbolName.c_str());
    if (!func) {
        ok = false;
        dlclose(handle);
        return -1;
    }

    int result = func();
    dlclose(handle);
    ok = true;
    return result;
}

// Mode 3: Inspect available dynamic symbols
std::vector<std::string> inspectSymbols(const std::string& soPath) {
    std::vector<std::string> symbols;
    std::string command = "nm -D --defined-only " + soPath + " 2>/dev/null";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return { "nm failed to execute" };

    char line[256];
    while (fgets(line, sizeof(line), pipe.get()) != nullptr) {
        std::string l(line);
        if (!l.empty()) symbols.push_back(l);
    }

    if (symbols.empty()) {
        symbols.push_back("No dynamic symbols found.");
    }

    return symbols;
}

// Mode 4: call void function with (int, const char**)
std::string executeTextArgs(const std::string& soPath, const std::string& symbolName, const std::vector<std::string>& args) {
    void* handle = dlopen(soPath.c_str(), RTLD_LAZY);
    if (!handle) return "dlopen error: " + std::string(dlerror());

    void (*func)(int, const char**);
    *(void**)(&func) = dlsym(handle, symbolName.c_str());
    if (!func) {
        std::string err = dlerror();
        dlclose(handle);
        return "symbol error: " + err;
    }

    std::vector<const char*> cargs;
    for (const auto& s : args) cargs.push_back(s.c_str());

    std::string output = captureStdout([&]() {
        func(cargs.size(), cargs.data());
    });

    dlclose(handle);
    return output;
}

// Mode 5: call int-returning function with (int, const char**)
int executeValueArgs(const std::string& soPath, const std::string& symbolName, const std::vector<std::string>& args, bool& ok) {
    void* handle = dlopen(soPath.c_str(), RTLD_LAZY);
    if (!handle) {
        ok = false;
        return -1;
    }

    int (*func)(int, const char**);
    *(void**)(&func) = dlsym(handle, symbolName.c_str());
    if (!func) {
        dlclose(handle);
        ok = false;
        return -1;
    }

    std::vector<const char*> cargs;
    for (const auto& s : args) cargs.push_back(s.c_str());

    int result = func(cargs.size(), cargs.data());
    dlclose(handle);
    ok = true;
    return result;
}

// JS bindings
Napi::Value RunText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (string path, string symbol)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string soPath = info[0].As<Napi::String>();
    std::string symbol = info[1].As<Napi::String>();
    return Napi::String::New(env, executeTextSymbol(soPath, symbol));
}

Napi::Value RunValue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (string path, string symbol)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string soPath = info[0].As<Napi::String>();
    std::string symbol = info[1].As<Napi::String>();
    bool ok;
    int result = executeValueSymbol(soPath, symbol, ok);
    if (!ok) {
        Napi::Error::New(env, "Function call failed").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Number::New(env, result);
}

Napi::Value Inspect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected (string path)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string soPath = info[0].As<Napi::String>();
    std::vector<std::string> list = inspectSymbols(soPath);
    Napi::Array arr = Napi::Array::New(env, list.size());
    for (size_t i = 0; i < list.size(); ++i) {
        arr.Set(i, Napi::String::New(env, list[i]));
    }
    return arr;
}

Napi::Value RunArgsText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Expected (string path, string symbol, array args)").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string soPath = info[0].As<Napi::String>();
    std::string symbol = info[1].As<Napi::String>();
    Napi::Array inputArgs = info[2].As<Napi::Array>();

    std::vector<std::string> args;
    for (uint32_t i = 0; i < inputArgs.Length(); ++i) {
        args.push_back(inputArgs.Get(i).As<Napi::String>().Utf8Value());
    }

    return Napi::String::New(env, executeTextArgs(soPath, symbol, args));
}

Napi::Value RunArgsValue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Expected (string path, string symbol, array args)").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string soPath = info[0].As<Napi::String>();
    std::string symbol = info[1].As<Napi::String>();
    Napi::Array inputArgs = info[2].As<Napi::Array>();

    std::vector<std::string> args;
    for (uint32_t i = 0; i < inputArgs.Length(); ++i) {
        args.push_back(inputArgs.Get(i).As<Napi::String>().Utf8Value());
    }

    bool ok;
    int result = executeValueArgs(soPath, symbol, args, ok);
    if (!ok) {
        Napi::Error::New(env, "Failed to call function with args").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, result);
}

// Exports
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("setStdoutLogger", Napi::Function::New(env, SetStdoutLogger));
    exports.Set("runText", Napi::Function::New(env, RunText));
    exports.Set("runValue", Napi::Function::New(env, RunValue));
    exports.Set("inspect", Napi::Function::New(env, Inspect));
    exports.Set("runArgsText", Napi::Function::New(env, RunArgsText));
    exports.Set("runArgsValue", Napi::Function::New(env, RunArgsValue));
    return exports;
}

NODE_API_MODULE(sljs, Init)
