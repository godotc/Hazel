//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:58:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-02 02:02:37
 * @ Description:
 */



#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <iostream>
#include <nethost.h>
#include <string>


#if _WIN32
    #include <windows.h>
    #define STR(s) L##s
#else
    #define STR(s) s
#endif


#include <stdlib.h>



#define getProcAddr(lib, name, type) ((type)GetProcAddress(HMODULE(lib), name))

// Function to load a library
void *load_library(const char_t *name)
{
    return (void *)LoadLibrary(name);
}

// Global function pointers for hostfxr
hostfxr_initialize_for_dotnet_command_line_fn initRuntimeCmdlFp;
hostfxr_initialize_for_runtime_config_fn      initRuntimeFp;
hostfxr_get_runtime_delegate_fn               getDelegateFp;
hostfxr_close_fn                              closeHostFxrFp;

void error_func(const char_t *msg)
{
    std::wcout << STR("ERROR: ") << msg << std::endl;
};

// Load hostfxr and get exports
bool load_hostfxr()
{
    char_t buffer[MAX_PATH];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    int    rc          = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0) {
        return false;
    }

    void *lib = load_library(buffer);
    if (!lib) {
        return false;
    }

    hostfxr_set_error_writer_fn setWriterErrorFunc = getProcAddr(lib, "hostfxr_set_error_writer", hostfxr_set_error_writer_fn);
    if (!setWriterErrorFunc) {
        std::cerr << "Failed to get hostfxr_set_error_writer" << std::endl;
    }
    setWriterErrorFunc(error_func);

    initRuntimeCmdlFp = getProcAddr(lib, "hostfxr_initialize_for_dotnet_command_line", hostfxr_initialize_for_dotnet_command_line_fn);
    initRuntimeFp     = getProcAddr(lib, "hostfxr_initialize_for_runtime_config", hostfxr_initialize_for_runtime_config_fn);
    getDelegateFp     = getProcAddr(lib, "hostfxr_get_runtime_delegate", hostfxr_get_runtime_delegate_fn);
    closeHostFxrFp    = getProcAddr(lib, "hostfxr_close", hostfxr_close_fn);

    return (initRuntimeCmdlFp && initRuntimeFp && getDelegateFp && closeHostFxrFp);
}

load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionFp = nullptr;
load_assembly_fn                          loadAssembyFp                = nullptr;

// Load .NET Core and get the function pointer to load assemblies
load_assembly_and_get_function_pointer_fn init_dotnet_delegates(const char_t *configPath)
{
    hostfxr_handle context = nullptr;

    int ret = initRuntimeFp(configPath, nullptr, &context);
    if (ret != 0 || context == nullptr) {
        std::cerr << "Init failed: " << std::hex << ret << std::endl;
        closeHostFxrFp(context);
        return nullptr;
    }

    ret = getDelegateFp(context,
                        hdt_load_assembly_and_get_function_pointer,
                        (void **)&loadAssemblyAndGetFunctionFp);
    if (ret != 0 || loadAssemblyAndGetFunctionFp == nullptr) {
        std::cerr << "Get delegate failed: load_assembly_and_get_fn_fptr " << std::hex << ret << std::endl;
    }
    printf("load_assembly_and_get_function_fptr: %p\n", loadAssemblyAndGetFunctionFp);


    ret = getDelegateFp(context,
                        hdt_load_assembly,
                        (void **)&loadAssembyFp);
    if (ret != 0 || loadAssembyFp == nullptr) {
        std::cerr << "Get delegate failed: load_assembly_fptr " << std::hex << ret << std::endl;
    }

    closeHostFxrFp(context);
    return nullptr;
}


int unmanager_function_call()
{
    // Path to the .NET assembly
    const char_t *dotnetlib_path = STR("C:\\Users\\norm\\1\\Hazel\\example\\project0\\csscript\\bin\\Debug\\net8.0\\csscript.dll");

    // Fully qualified name of the type and method
    const char_t *dotnet_type        = STR("abc.Class1, csscript");
    const char_t *dotnet_type_method = STR("Add1");
    // const char_t *dotnet_type_method = STR("add_numbers");

    // Function pointer to the managed delegate
    typedef int (*component_entry_point_fn)(int, int);
    component_entry_point_fn add_method = nullptr;

    // Load the assembly and get the function pointer
    int ret = loadAssemblyAndGetFunctionFp(
        dotnetlib_path,
        dotnet_type,
        dotnet_type_method,
        // nullptr,
        UNMANAGEDCALLERSONLY_METHOD, // delegate_type_name
        nullptr,
        (void **)&add_method);

    if (ret != 0 || add_method == nullptr) {
        std::cerr << "Failed to get add_method " << std::hex << ret << std::endl;
        return -1;
    }

    // Call the .NET method
    int result = add_method(3, 7);
    std::cout << "Result from .NET: " << result << std::endl;

    return 0;
}

int managerd_function_call()
{

    // Path to the .NET assembly
    const char_t *dotnetlib_path     = STR("C:\\Users\\norm\\1\\Hazel\\example\\project0\\csscript\\bin\\Debug\\net8.0\\csscript.dll");
    using createInstanceDelegateType = void *(STDMETHODCALLTYPE *)(void);
    createInstanceDelegateType fp;

    // 5. 创建 Calculator 实例
    int ret = loadAssemblyAndGetFunctionFp(dotnetlib_path,
                                           STR("abc.Class1, csscript"),
                                           STR("CreateInstance"),
                                           nullptr,
                                           nullptr,
                                           (void **)&fp);
    if (ret != 0 || fp == nullptr) {
        fprintf(stderr, "Failed to get CreateInstance function pointer\n");
        return -1;
    }

    void *calculator_instance = nullptr;
    calculator_instance       = fp();

    // 6. 绑定实例方法 Add
    using AddType = int(STDMETHODCALLTYPE *)(void *, int, int);
    AddType add_method;
    ret = loadAssemblyAndGetFunctionFp(
        dotnetlib_path,
        STR("abc.Class1, csscript"),
        STR("Add"),
        nullptr,
        nullptr,
        // "System.Int32, System.Private.CoreLib", // 方法签名
        (void **)&add_method);

    // 7. 调用实例方法
    int result = add_method(calculator_instance, 3, 5); // 返回 8

    // // 8. 调用静态方法
    // GetMessageDelegate get_msg;
    // rc = load_assembly_fptr(
    //     L"ManagedLib.dll",
    //     L"ManagedLib.Calculator, ManagedLib",
    //     L"GetMessage",
    //     nullptr,
    //     nullptr,
    //     (void **)&get_msg);
    // const char *message = get_msg(); // "Hello from Managed .NET!"

    return 0;
}


int main()
{
#if _WIN32
    _putenv("COREHOST_TRACE=1");                     // Enable tracing
    _putenv("COREHOST_TRACEFILE=hostfxr_trace.log"); // Log to a file
#else
    setenv("COREHOST_TRACE", "1", 1);                     // Enable tracing
    setenv("COREHOST_TRACEFILE", "hostfxr_trace.log", 1); // Log to a file
#endif

    if (!load_hostfxr()) {
        std::cerr << "Failed to load hostfxr" << std::endl;
        return -1;
    }


    // auto config_path = STR("C:/Users/norm/1/Hazel/example/project0/csscript/bin/Debug/net8.0/csscript.runtimeconfig.json");
    auto config_path = STR("C:\\Users\\norm\\1\\Hazel\\example\\project0\\csscript\\bin\\Debug\\net8.0\\csscript.runtimeconfig.json");

    init_dotnet_delegates(config_path);
    if (!loadAssemblyAndGetFunctionFp) {
        std::cerr << "Failed to get load_assembly_and_get_function_pointer" << std::endl;
        return -1;
    }

    // unmanager_function_call();
    managerd_function_call();
}