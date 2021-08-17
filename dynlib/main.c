#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    void* handle = dlopen("./libreturn42.so", RTLD_LAZY);
    if (!handle) {
        fputs(dlerror(), stderr);
        return 1;
    }

    int (*f)() = dlsym(handle, "return42");
    char* error;
    if ((error = dlerror()) != NULL) {
        fputs(error, stderr);
        return 1;
    }

    printf("%d\n", f());
    dlclose(handle);

    return 0;
}
