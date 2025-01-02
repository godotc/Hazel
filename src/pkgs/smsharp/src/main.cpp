//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:58:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-03 00:37:51
 * @ Description:
 */



#include <coreclr_delegates.h>
#include <cstddef>
#include <cstdlib>
#include <hostfxr.h>
#include <nethost.h>

#include <libloaderapi.h>
#include <ostream>
#include <windows.h>
#include <winerror.h>



#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <cstdio>
#include <iostream>



auto get_export(void *lib, const char *name)
{
    return (void *)GetProcAddress((HMODULE)lib, name);
}

auto load_libraryy(const char *name)
{
    return (void *)LoadLibraryEx(name);
}


hostfxr_initialize_for_runtime_config_fn init_fptr;
hostfxr_get_runtime_delegate_fn          get_delegate_fptr;
hostfxr_close_fn                         close_fptr;

// Using the nethost library, discover the location of hostfxr and get exports
bool load_hostfxr()
{
    // Pre-allocate a large buffer for the path to hostfxr
    char_t buffer[MAX_PATH];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    int    rc          = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0) {
        return false;
    }

    printf("hostfxr path: %ls\n", buffer);


    // Load hostfxr and get desired exports
    void                                    *lib               = load_libraryy(buffer);
    hostfxr_initialize_for_runtime_config_fn init_fptr         = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    hostfxr_get_runtime_delegate_fn          get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    hostfxr_close_fn                         close_fptr        = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

    return (init_fptr && get_delegate_fptr && close_fptr);
}


// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path)
{
    // Load .NET Core
    void          *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle cxt                                    = nullptr;
    int            rc                                     = init_fptr(config_path, nullptr, &cxt);
    if (rc != 0 || cxt == nullptr)
    {
        std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
        close_fptr(cxt);
        return nullptr;
    }

    // Get the load assembly function pointer
    rc = get_delegate_fptr(
        cxt,
        hdt_load_assembly_and_get_function_pointer,
        &load_assembly_and_get_function_pointer);
    if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

    close_fptr(cxt);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}

int main()
{

    auto load_assembly_and_get_function_pointer = get_dotnet_load_assembly(nullptr);
    // Function pointer to managed delegate
    component_entry_point_fn hello = nullptr;

    // int rc = load_assembly_and_get_function_pointer(
    //     "lib",
    //     dotnet_type,
    //     dotnet_type_method,
    //     nullptr /*delegate_type_name*/,
    //     nullptr,
    //     (void **)&hello);
}