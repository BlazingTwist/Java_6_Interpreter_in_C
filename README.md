## Do not actually use this!

It's interpreted, so the performance is bad from the get-go.  
The garbage collector is naive and memory inefficient.

But worst of all:  
Not all opCodes are supported, and even the ones that are don't fully respect the Java SE 6 specification.

Did I mention it's **just** Java 6?  
I repeat **do not** use this.

---

You want to use it anyway? Ugh, fine.

1. Convert your .class files to hex strings and split them so that they don't exceed C's max string length.  
    Prepend with a 32 bit hex encoding of the .class file length in bytes.  
    E.g. `{"000000ff", "cafebabe...", "...", ...}`
2. include `jvm/include/interpretedjvm.h` and instantiate it with `InterpretedJVM jvm = prepareJVM({classFile1, classFile2, ...}, numFiles)`
3. Push the parameters (if any) with `push_<type>(&jvm, <value>)`
4. Push your EntryPoint (e.g. main) with `push_Method(&jvm, "<qualified_class_name>", "<method_name>", "method_descriptor")`
5. Invoke with `executeAllMethods_<numReturnValues>Return(&jvm)`
6. Make sure you release your return values when you're done. `decrementReferenceCount(<HeapPointer>)`
7. Once finished, release the interpreter. `releaseJVM(&jvm)`

For some examples, check [the pgp directory](pgp)