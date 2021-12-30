# OpenGL Core 4.6 with DSA and SPIR-V

This project outlines how to open a window with the Win32 API and how to create a modern OpenGL core context. It uses Spirv for the shaders and features a single triangle rendering using Direct State Access features of modern OpenGL.

It aims to just serve as a minimal refrence in how to get these aspects up and running quickly with minimal abstraction allowing you to just get into it.

## Build
This project uses the cmake build system. 
```
cd ModernOpenGLWin32 
mkdir build
cd build
cmake ..
make
```

### dependencies
Use package manager vcpkg (https://vcpkg.io/en/getting-started.html) and install dependencies.

```
.\vcpkg\bootstrap-vcpkg.bat
```
```
vcpkg install glew:x64-windows
```
```
vcpkg integrate install (optional for usage with visual studio 2019/22)
```



## Motivation

Doing a completely different project i needed to interface with the Win32 API as well as use spir-v. I thought there could be some benefit in this refrence existing.

## Contributing
Pull requests are welcome and issues as well.

## License
[MIT](https://choosealicense.com/licenses/mit/)