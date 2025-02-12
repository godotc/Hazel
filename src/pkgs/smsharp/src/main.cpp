//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:58:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-02-13 02:17:08
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

// Function to get an export from a library
void *get_export(void *lib, const char *name)
{
    return (void *)GetProcAddress((HMODULE)lib, name);
}

// Function to load a library
void *load_library(const char_t *name)
{
    return (void *)LoadLibrary(name);
}

// Global function pointers for hostfxr
hostfxr_initialize_for_dotnet_command_line_fn init_runtime_cmdl_fptr;
hostfxr_initialize_for_runtime_config_fn      init_runtime_fptr;
hostfxr_get_runtime_delegate_fn               get_delegate_fptr;
hostfxr_close_fn                              close_fptr;

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

    // hostfxr_set_error_writer_fn fptr = (hostfxr_set_error_writer_fn)get_export(lib, "hostfxr_set_error_writer");
    // if (!fptr) {
    //     std::cerr << "Failed to get hostfxr_set_error_writer" << std::endl;
    // }
    // fptr(error_func);

    init_runtime_cmdl_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)get_export(
        lib, "hostfxr_initialize_for_dotnet_command_line");
    init_runtime_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fptr        = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

    return (init_runtime_cmdl_fptr && init_runtime_fptr && get_delegate_fptr && close_fptr);
}

// Load .NET Core and get the function pointer to load assemblies
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path)
{
    void          *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle context                                = nullptr;

    hostfxr_initialize_parameters parms{
        .size        = 1,
        .host_path   = nullptr,
        .dotnet_root = nullptr,
    };


    int ret = init_runtime_fptr(config_path, nullptr, &context);
    if (ret != 0 || context == nullptr) {
        std::cerr << "Init failed: " << std::hex << ret << std::endl;
        close_fptr(context);
        return nullptr;
    }

    ret = get_delegate_fptr(context,
                            hdt_load_assembly_and_get_function_pointer,
                            &load_assembly_and_get_function_pointer);
    if (ret != 0 || load_assembly_and_get_function_pointer == nullptr) {
        std::cerr << "Get delegate failed: " << std::hex << ret << std::endl;
    }

    close_fptr(context);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}

// Load .NET Core and get the function pointer to load assemblies
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly2()
{
    void          *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle context                                = nullptr;

    int ret = init_runtime_cmdl_fptr(0, 0, nullptr, &context);
    if (ret != 0 || context == nullptr) {
        std::cerr << "Init failed: " << std::hex << ret << std::endl;
        close_fptr(context);
        return nullptr;
    }

    ret = get_delegate_fptr(context,
                            hdt_load_assembly_and_get_function_pointer,
                            &load_assembly_and_get_function_pointer);
    if (ret != 0 || load_assembly_and_get_function_pointer == nullptr) {
        std::cerr << "Get delegate failed: " << std::hex << ret << std::endl;
    }

    close_fptr(context);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}

int main()
{
    if (!load_hostfxr()) {
        std::cerr << "Failed to load hostfxr" << std::endl;
        return -1;
    }

    // Path to the .runtimeconfig.json file

    auto config_path = STR("C:/Users/norm/1/Hazel/example/project0/csscript/bin/Debug/net8.0/csscript.runtimeconfig.json");

    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = get_dotnet_load_assembly(config_path);
    // auto load_assembly_and_get_function_pointer = get_dotnet_load_assembly2();
    if (!load_assembly_and_get_function_pointer) {
        std::cerr << "Failed to get load_assembly_and_get_function_pointer" << std::endl;
        return -1;
    }

    // Path to the .NET assembly
    const char_t *dotnetlib_path = STR("C:\\Users\\norm\\1\\Hazel\\example\\project0\\csscript\\bin\\Debug\\net8.0\\csscript.dll");

    // Fully qualified name of the type and method
    const char_t *dotnet_type        = STR("csscript.Class1, csscript");
    const char_t *dotnet_type_method = STR("Add");

    // Function pointer to the managed delegate
    typedef int (*component_entry_point_fn)(int, int);
    component_entry_point_fn add_method = nullptr;

    // Load the assembly and get the function pointer
    int rc = load_assembly_and_get_function_pointer(
        dotnetlib_path,
        dotnet_type,
        dotnet_type_method,
        nullptr, // delegate_type_name
        nullptr,
        (void **)&add_method);

    if (rc != 0 || add_method == nullptr) {
        std::cerr << "Failed to get add_method" << std::endl;
        return -1;
    }

    // Call the .NET method
    int result = add_method(3, 7);
    std::cout << "Result from .NET: " << result << std::endl;

    return 0;
}