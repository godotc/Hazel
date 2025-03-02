using System.Runtime.InteropServices;

namespace abc{


public class Class1
{

    static void Main(string[] args)
    {
        Console.WriteLine("Hello World!");
    }

    // [UnmanagedCallersOnly]
    // [DllExport("Add1")]
    // [UnmanagedCallersOnly(EntryPoint = "add_numbers")]
    public static int Add1(int a, int b){
        System.Console.WriteLine("Hello World!");
        System.Console.WriteLine(a);
        System.Console.WriteLine(b);
        return a + b;
    }

}

}