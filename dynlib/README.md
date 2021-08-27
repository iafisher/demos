A minimal example of loading a shared library at runtime on Linux. The code to load the library is in `main.c`, and the library code itself is in `return_42.c`.

You can also easily load the library in Python:

```python
import ctypes

lib = ctypes.cdll.LoadLibrary("./libreturn42.so")
print(lib.return42())
```

With a bit more effort, the library can be used in Go, too:

```go
package main

/*
#cgo LFLAGS: -ldl
#include <dlfcn.h>

typedef int (*f_type)();

int return42_wrapper(void *f) {
	return ((f_type)f)();
}
*/
import "C"

import "fmt"

func main() {
	handle := C.dlopen(C.CString("libreturn42.so"), C.RTLD_LAZY)
	return42_ptr := C.dlsym(handle, C.CString("return42"))
	result := C.return42_wrapper(return42_ptr)
	fmt.Println(result)
}
```

This technique may be useful if:

- Your application needs to load compiled plugins at runtime.
- You are writing a programming language interpreter that needs to dynamically load compiled code as part of a foreign-function interface.

Credit to [David A. Wheeler's chapter on dynamically loaded libraries](https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html) for explaining this technique to me.
