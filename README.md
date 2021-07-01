# vHashLib

vHashLib is a simple C implementation of hashes (associative arrays) using Hash Tables.

With vHashLib you can create associative arrays using case-sensitive null-terminated Unicode strings as keys and generic pointers (`void *`) as values.

## Install

Run `make && make install` to compile and install the components using the default directory prefix (`/usr/local`).

Alternatively, run `make && make install -e PREFIX=/custom/path` to install the components with a custom directory prefix.

The static library will be installed into `$PREFIX/lib/libvhash.a` and the header file into `$PREFIX/include/vhash.h`.

When compiling your own programs, use the `-I $PREFIX/include` and `-L $PREFIX/lib` GCC compiler options and `-lvhash` options within your `Makefile`s or directly into the command line.

## Usage

```c
#include <vhash.h>

struct UserInfo bar = {/* init struct data here*/};
char *someString = "Loren ipsum...";

// Create a new hash and allocate memories
Hash *myhash = Hash_new();

// Add values to the hash
Hash_set(myhash, "myUserID", &bar, sizeof(UserInfo));
Hash_set(myhash, "SomeString", someString, strlen(someString) + 1);

// Fetch values
Tuple *userData = Hash_get(myhash, "myUserID");
struct UserInfo *baz = (struct UserInfo *)userData->value;

// Do something with your user data...

// Delete values
Hash_delete(myhash, "SomeString");

// Do something more with your hash...

// Free the hash before exit
Tuple_free(&userData);
Hash_free(&myhash);
```

### Creation and disposal

New hashes are created with `Hash_new()`. Each Hash uses dynamically allocated memory so it needs to be freed using `Hash_free()` after usage to properly release the memory.

### Filling hashes

Using `Hash_set()` you can add a new key/value pair to an existing Hash, or safely update (override) the value of an existing item. In both cases new memory is allocated to copy both the key and the value.
Internally, each key/value pair is stored within a `Tuple` structure that contains a `key` (`char *`), a `value` (`void *`) and a `length` for the stored value (`size_t`).

### Extracting data from hashes

Extracting items from the Hash using `Hash_get()`, `Hash_first()` or `Hash_last()`, will create dynamic memory for a `Tuple` structure that need to be freed after use with `Tuple_free()`. When freeing a Tuple, _only the Tuple data members will be freed_, the actual Hash data referenced by the tuple item will persist within the Hash until the item is deleted with `Hash_delete()` or the whole Hash is freed.

You can also use `Hash_getValue()` to get a generic pointer to the value stored under the given key. In this case, no length information is returned, and you need to be sure about what data type is stored in order to do a proper casting.

### Hash table defaults

The current hash function simply computes the sum of all byte values of the key and uses the modulus operator to get the hash value.

The size of the hash table can be customised at compile-time by setting the `HASH_SIZE` constant, for example: `make -e HASH_SIZE=256 && make install`. The default hash size is 128.

## Run the tests

Run `make test` to compile a version of the package with debug symbols enabled and start the unit tests.

**Important notes**

 - The unit tests require [Valgrind](https://www.valgrind.org/) if you are on a Linux system.
 - If you run the tests before installing the package, run a `make clean` to ensure your installation does not contain debug symbols.

## License

vHashLib is licensed under LGPL. Please refer to the LICENSE file for detailed information.

