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

Napi::FunctionReference jsStdoutLogger;

Napi::Value SetStdoutLogger(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!info[0].IsFunction()) {
        Napi::TypeError::New(env, "Expected a function").ThrowAsJavaScriptException();
        return env.Null();
    }
    jsStdoutLogger = Napi::Persistent(info[0].As<Napi::Function>());
    return env.Undefined();
}

void logToJs(const std::string& message) {
    if (!jsStdoutLogger.IsEmpty()) {
        jsStdoutLogger.Call({ Napi::String::New(jsStdoutLogger.Env(), message) });
    }
}

std::string captureStdout(const std::function<void()>& func) {
    if (!jsStdoutLogger.IsEmpty()) {
        func();
        return "[output sent via jsStdoutLogger]";
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) return "[ERROR] pipe creation failed";

    int stdout_fd = dup(STDOUT_FILENO);
    if (stdout_fd == -1) return "[ERROR] dup failed";

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
        close(stdout_fd);
        return "[ERROR] dup2 failed";
    }
    close(pipefd[1]);

    func();

    fflush(stdout);
    dup2(stdout_fd, STDOUT_FILENO);
    close(stdout_fd);

    char buffer[BUFFER_SIZE];
    ssize_t count = read(pipefd[0], buffer, BUFFER_SIZE - 1);
    close(pipefd[0]);
    buffer[(count > 0) ? count : 0] = '\0';

    return std::string(buffer);
}

std::string dlErrorWrapper(void* handle, const std::string& context) {
    std::string err(dlerror());
    if (handle) dlclose(handle);
    return "[ERROR] " + context + ": " + err;
}

void* safeDlopen(const std::string& path) {
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) logToJs("[ERROR] dlopen failed: " + std::string(dlerror()));
    return handle;
}

template<typename T>
T safeDlsym(void* handle, const std::string& name, std::string& error) {
    dlerror(); // Clear old errors
    T sym = reinterpret_cast<T>(dlsym(handle, name.c_str()));
    const char* err = dlerror();
    if (err) {
        error = std::string(err);
        return nullptr;
    }
    return sym;
}

std::string executeTextSymbol(const std::string& soPath, const std::string& symbolName) {
    void* handle = safeDlopen(soPath);
    if (!handle) return "[ERROR] Cannot open .so file";

    std::string err;
    auto func = safeDlsym<void(*)()>(handle, symbolName, err);
    if (!func) return dlErrorWrapper(handle, "symbol lookup");

    std::string output = captureStdout(func);
    dlclose(handle);
    return output;
}

int executeValueSymbol(const std::string& soPath, const std::string& symbolName, bool& ok) {
    void* handle = safeDlopen(soPath);
    if (!handle) {
        ok = false;
        return -1;
    }

    std::string err;
    auto func = safeDlsym<int(*)()>(handle, symbolName, err);
    if (!func) {
        dlErrorWrapper(handle, "symbol lookup");
        ok = false;
        return -1;
    }

    int result = func();
    dlclose(handle);
    ok = true;
    return result;
}

std::vector<std::string> inspectSymbols(const std::string& soPath) {
    std::vector<std::string> symbols;
    std::string command = "nm -D --defined-only " + soPath + " 2>/dev/null";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return { "[ERROR] Failed to execute nm" };

    char line[256];
    while (fgets(line, sizeof(line), pipe.get()) != nullptr)
        symbols.emplace_back(line);

    if (symbols.empty()) symbols.push_back("No dynamic symbols found.");
    return symbols;
}

std::string executeTextArgs(const std::string& soPath, const std::string& symbolName, const std::vector<std::string>& args) {
    void* handle = safeDlopen(soPath);
    if (!handle) return "[ERROR] Cannot open .so file";

    std::string err;
    auto func = safeDlsym<void(*)(int, const char**)>(handle, symbolName, err);
    if (!func) return dlErrorWrapper(handle, "symbol lookup");

    std::vector<const char*> cargs;
    for (const auto& s : args) cargs.push_back(s.c_str());

    std::string output = captureStdout([&]() {
        func(static_cast<int>(cargs.size()), cargs.data());
    });

    dlclose(handle);
    return output;
}

int executeValueArgs(const std::string& soPath, const std::string& symbolName, const std::vector<std::string>& args, bool& ok) {
    void* handle = safeDlopen(soPath);
    if (!handle) {
        ok = false;
        return -1;
    }

    std::string err;
    auto func = safeDlsym<int(*)(int, const char**)>(handle, symbolName, err);
    if (!func) {
        dlErrorWrapper(handle, "symbol lookup");
        ok = false;
        return -1;
    }

    std::vector<const char*> cargs;
    for (const auto& s : args) cargs.push_back(s.c_str());

    int result = func(static_cast<int>(cargs.size()), cargs.data());
    dlclose(handle);
    ok = true;
    return result;
}

std::string executeStringArgs(const std::string& soPath, const std::string& symbolName, const std::vector<std::string>& args) {
    void* handle = safeDlopen(soPath);
    if (!handle) return "[ERROR] Cannot open .so file";

    std::string err;
    auto func = safeDlsym<const char*(*)(int, const char**)>(handle, symbolName, err);
    if (!func) {
        std::string errorMessage = dlErrorWrapper(handle, "symbol lookup");
        logToJs(errorMessage);
        return errorMessage;
    }

    std::vector<const char*> cargs;
    for (const auto& s : args) cargs.push_back(s.c_str());

    const char* result = nullptr;
    std::string output;

    try {
        result = func(static_cast<int>(cargs.size()), cargs.data());
        output = result ? std::string(result) : "[ERROR] Null result";
    } catch (...) {
        output = "[ERROR] Exception during function execution";
    }

    if (!jsStdoutLogger.IsEmpty()) {
        logToJs("[StringArgs Output] " + output);
        output = "[output sent via jsStdoutLogger]";
    }

    dlclose(handle);
    return output;
}

std::string executeStringReturnSymbol(const std::string& soPath, const std::string& symbolName) {
    void* handle = safeDlopen(soPath);
    if (!handle) return "[ERROR] Cannot open .so file";

    std::string err;
    auto func = safeDlsym<const char*(*)()>(handle, symbolName, err);
    if (!func) {
        std::string errorMessage = dlErrorWrapper(handle, "symbol lookup");
        logToJs(errorMessage);
        return errorMessage;
    }

    const char* result = nullptr;
    std::string output;

    try {
        result = func();
        output = result ? std::string(result) : "[ERROR] Null result";
    } catch (...) {
        output = "[ERROR] Exception during function execution";
    }

    if (!jsStdoutLogger.IsEmpty()) {
        logToJs("[StringReturn Output] " + output);
        output = "[output sent via jsStdoutLogger]";
    }

    dlclose(handle);
    return output;
}

// N-API BINDINGS

Napi::Value RunText(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), executeTextSymbol(info[0].As<Napi::String>(), info[1].As<Napi::String>()));
}

Napi::Value RunValue(const Napi::CallbackInfo& info) {
    bool ok;
    int result = executeValueSymbol(info[0].As<Napi::String>(), info[1].As<Napi::String>(), ok);
    if (!ok) {
        Napi::Error::New(info.Env(), "Function call failed").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    return Napi::Number::New(info.Env(), result);
}

Napi::Value Inspect(const Napi::CallbackInfo& info) {
    auto list = inspectSymbols(info[0].As<Napi::String>());
    Napi::Array arr = Napi::Array::New(info.Env(), list.size());
    for (size_t i = 0; i < list.size(); ++i)
        arr.Set(i, Napi::String::New(info.Env(), list[i]));
    return arr;
}

Napi::Value RunArgsText(const Napi::CallbackInfo& info) {
    std::vector<std::string> args;
    Napi::Array inputArgs = info[2].As<Napi::Array>();
    for (uint32_t i = 0; i < inputArgs.Length(); ++i)
        args.push_back(inputArgs.Get(i).As<Napi::String>());
    return Napi::String::New(info.Env(), executeTextArgs(info[0].As<Napi::String>(), info[1].As<Napi::String>(), args));
}

Napi::Value RunArgsValue(const Napi::CallbackInfo& info) {
    std::vector<std::string> args;
    Napi::Array inputArgs = info[2].As<Napi::Array>();
    for (uint32_t i = 0; i < inputArgs.Length(); ++i)
        args.push_back(inputArgs.Get(i).As<Napi::String>());
    bool ok;
    int result = executeValueArgs(info[0].As<Napi::String>(), info[1].As<Napi::String>(), args, ok);
    if (!ok) {
        Napi::Error::New(info.Env(), "Function call failed").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    return Napi::Number::New(info.Env(), result);
}

Napi::Value RunArgsString(const Napi::CallbackInfo& info) {
    std::vector<std::string> args;
    Napi::Array inputArgs = info[2].As<Napi::Array>();
    for (uint32_t i = 0; i < inputArgs.Length(); ++i)
        args.push_back(inputArgs.Get(i).As<Napi::String>());
    return Napi::String::New(info.Env(), executeStringArgs(info[0].As<Napi::String>(), info[1].As<Napi::String>(), args));
}

Napi::Value RunStringReturn(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), executeStringReturnSymbol(info[0].As<Napi::String>(), info[1].As<Napi::String>()));
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("setStdoutLogger", Napi::Function::New(env, SetStdoutLogger));
    exports.Set("runText", Napi::Function::New(env, RunText));
    exports.Set("runValue", Napi::Function::New(env, RunValue));
    exports.Set("inspect", Napi::Function::New(env, Inspect));
    exports.Set("runArgsText", Napi::Function::New(env, RunArgsText));
    exports.Set("runArgsValue", Napi::Function::New(env, RunArgsValue));
    exports.Set("runArgsString", Napi::Function::New(env, RunArgsString));
    exports.Set("runStringReturn", Napi::Function::New(env, RunStringReturn));
    return exports;
}

NODE_API_MODULE(sljs, Init)
