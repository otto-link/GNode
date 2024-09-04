![gnode_banner](https://github.com/otto-link/GNode/assets/121820229/32885341-e59d-4914-ad37-5dc326aec006)
_This is just a nice picture._

# GNode

A generic node-based data structure for node graph programming in C++.

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

``` bash
mkdir build && cd build
cmake ..
make
```

To get the documentation:
```
cd build
make doc_gnode
[YOUR_WEB_BROWSER] doc/doc_doxygen/html/index.html
```

### Usage examples

Examples are available in the examples folder, featuring a calculator that includes both float and 2D vector of floats data types:
```mermaid
---
title: graph
---
flowchart LR
    95832391008960([ValueVec])
    95832391009376([SumVec])
    95832391010000([Value])
    95832391010416([Add])
    95832391011552([Print])
    95832391011792([Value])
    95832391012208([Add])
    95832391013488([Print])
95832391008960 --> 95832391009376;
95832391009376 --> 95832391010416;
95832391010000 --> 95832391010416;
95832391010416 --> 95832391011552;
95832391010416 --> 95832391012208;
95832391011792 --> 95832391012208;
95832391012208 --> 95832391013488;
```

