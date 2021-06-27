# vHashLib

vHashLib is a simple C implementation of hashes (associative arrays) using Hash Tables.

With vHashLib you can create associative arrays using case-sensitive null-terminated Unicode strings as keys and generic pointers (`void *`) as values.

## Usage

```c
struct UserInfo bar = {/* init struct data here*/}
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

Each Hash uses dynamically allocated memory so it needs to be freed using `Hash_free()` after usage to avoid memory problems.

When adding a new value to the hash using `Hash_set()`, new memory is allocated to copy both the key and the value. Internally, each key/value pair is stored within a `Tuple` structure that contains a `key` (`char *`), a `value` (`void *`) and a `length` for the stored value (`size_t`).

If a Tuple with the same key already exists, `Hash_set()` will safely update (override) the value.

Extracting items from the Hash using `Hash_get()`, `Hash_first()` or `Hash_last()`, will create dynamic memory for a `Tuple` structure that need to be freed after use with `Tuple_free()`. When freeing a Tuple, _only the Tuple data members will be freed_, the actual Hash data referenced by the tuple item will persist within the Hash until the item is deleted or the whole Hash is freed.

The size of the hash table can be customised at compile time by setting the `HASH_SIZE` constant, the default size is 128.

The current hash function simply computes the sum of all character values of the key and uses the modulus operator to get the hash value.
