<div align="center">

# librconv

a library to parse and convert various rhythm game formats

</div>

## Building

Before attempting to build, be sure you have all ([test-](#test-dependencies))[dependencies](#dependenceies) set up.

Building is done via `cmake`:

```sh
mkdir build
cd build
cmake ..
```

### Dependenceies

Required libraries are added as git-submodules:

```sh
git submodules update --recursive --remote
```

### Test Dependencies

Tests are run via [libcheck](https://github.com/libcheck/check).
Please see it's installation guide for further information.
