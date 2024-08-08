1. Iterate all header file under src directory
    1. Use the libclang to  generate the AST
    2. Parse AST to read the compile time property: `[[I_Want_To_Reflect]] class A{....}`
    3. Generate the reflect: types register codes
 2. Include by header files (*.generated.h)
 3. Done