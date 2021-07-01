#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash.h"

/** OLD version, using binary trees.
 * Working fine, but support for delete is missing, and
 * the binary tree ends up being unbalanced soon.
 */

/**
 * A HashNode is a generic struct with a left and right pointers
 * to other HashNodes in the tree
 * and pointers for a key and value Tuple
 * so that they can hold different type of data
 */
typedef struct _Node HashNode;
typedef struct _Node {
  HashNode *left; ///< Pointer to the first item in the Hash
  HashNode *right; ///< Pointer to the last item in the Hash
  Tuple data; ///< Structure that contains the data for the Hash
} HashNode;

/**
 * A Hash is a sorted set of items,
 * like a dictionary or associative array
 */
typedef struct _Hash {
  HashNode *root; ///< Pointer to the root node of the Hash tree
  int length; ///< Length of the Hash
} Hash;

void HashNode_free(HashNode **this);

/**
 * Creates a new empty Hash and returns its pointer
 */
Hash *Hash_new() {
  Hash *this = (Hash *)calloc(sizeof(Hash), 1);
  if (this == NULL) return NULL;
  return this;
}

/**
 * Safely deletes all nodes from the given Hash
 * starting from the leftmost one
 */
void Hash_purge(Hash *this) {
  if (this->root == NULL) return;
  // Walking cursor for the current node
  HashNode *current = this->root;
  // Keeps track of the parent node to update its pointers
  HashNode *parent = NULL;
  while(1) {
    if (current->left != NULL) {
      parent = current;
      current = current->left;
      continue;
    }
    if (current->right != NULL) {
      parent = current;
      current = current->right;
      continue;
    }
    // At this point, the current node is either a leaf or a single root node
    if (current == this->root) {
      // Exit the loop and take care of the root separately
      parent = NULL;
      break;
    }
    // Update the parent node (detach the current)
    if (parent->left == current) parent->left = NULL;
    if (parent->right == current) parent->right = NULL;
    // Delete the current node
    this->length -= 1;
    HashNode_free(&current);
    // Restart from the root, this is not very performant,
    // but avoid using queues
    current = this->root;
  }
  // At this point, the root node should be the only one
  this->length = 0;
  HashNode_free(&current);
  this->root = NULL;
}

/**
 * Destroys a hash and all its nodes
 */
void Hash_free(Hash **this) {
  if (this != NULL) {
    // First walk the hash and free all its nodes
    // We implicitely trust the developer that didn't mess up
    // with the length attribute of the hash
    if (!Hash_empty(*this)) Hash_purge(*this);

    // Then free the Hash itself
    // This will erase all data in memory
    memset(*this, 0, sizeof(Hash));
    // Free the pointer to which this is pointing
    // which is the actual pointer to the hash
    free(*this);
    *this = NULL;
  }
}

/**
 * Tells if a hash is empty
 */
bool Hash_empty(const Hash *this) {
  return (this->length == 0 || this->root == NULL);
}

/**
 * Returns the length of the given hash
 */
int  Hash_length(const Hash *this) {
  return this->length;
}

/**
 * Creates a new Hash node with the provided key/value/length
 */
HashNode *HashNode_new(const char *key, const void *value, size_t length) {
  HashNode *this = (HashNode *)calloc(sizeof(HashNode), 1);
  if (this == NULL) return NULL;
  // Copy the key as string
  this->data.key = strdup(key);
  if (this->data.key == NULL) {
    HashNode_free(&this);
    return NULL;
  }
  // Allocate memory for the value
  this->data.value = calloc(length, 1);
  if (this->data.value == NULL) {
    memset(this->data.key, 0, strlen(this->data.key) + 1);
    free(this->data.key);
    this->data.key = NULL;
    HashNode_free(&this);
    return NULL;
  }
  this->data.length = length;
  // Copy the actual data for the value
  memcpy(this->data.value, value, this->data.length);
  return this;
}

/**
 * Destroys the given Hash node
 */
void HashNode_free(HashNode **this) {
  if (this != NULL) {
    // Cleanup the data memory and free the data pointers
    if ((*this)->data.key != NULL) {
      memset((*this)->data.key, 0, strlen((*this)->data.key) + 1);
      free((*this)->data.key);
    }
    if ((*this)->data.value != NULL) {
      memset((*this)->data.value, 0, (*this)->data.length);
      free((*this)->data.value);
    }
    // Clean memory for the node
    memset(*this, 0, sizeof(HashNode));
    // Free and NULLify the node pointer
    free(*this);
    *this = NULL;
  }
}

/**
 * Sets a key/value pair in given Hash
 */
bool Hash_set(Hash *this, const char *key, const void *value, size_t length) {
  if (this->root == NULL) {
    // First element
    HashNode *item = HashNode_new(key, value, length);
    if (item == NULL) return false;
    this->root = item;
    this->length += 1;
    return true;
  }
  // Start pointing at the first node
  HashNode *current = this->root;
  while(1) {
    // If the given key matches with the current item, override the value
    // without changing the length
    int comp = strcmp(key, current->data.key);
    if (comp == 0) {
      memcpy(current->data.value, value, length);
      return true;
    }

    // If the given key is < than the current node key, walk the left branch
    if (comp < 0) {
      if (current->left == NULL) {
        // Attach the new node to the left of the current node
        HashNode *item = HashNode_new(key, value, length);
        if (item == NULL) return false;
        current->left = item;
        this->length += 1;
        return true;
      }
      // Walk down the left path
      current = current->left;
      continue;
    }

    // If the given key is > than the current node key, walk the right branch
    if (comp > 0) {
      if (current->right == NULL) {
        // Attach it to the right of the current node
        HashNode *item = HashNode_new(key, value, length);
        if (item == NULL) return false;
        current->right = item;
        this->length += 1;
        return true;
      }
      // Walk down the right path
      current = current->right;
      continue;
    }
  }
  return false;
}

/**
 * Gets the value for the given key, or NULL if the key does not exist
 */
void *Hash_get(Hash *this, void *key) {
  if (this->root != NULL) {
    HashNode *current = this->root;
    while (current != NULL) {
      int comp = strcmp(key, current->data.key);
      if (comp == 0) {
        // Key was found
        return current->data.value;
      }
      if (comp < 0) {
        // Look into the left branch
        current = current->left;
      }
      if (comp > 0) {
        // Look into the right branch
        current = current->right;
      }
    }
  }
  // No key was found
  return NULL;
}

/**
 * Gets the key/value pair for the first Hash item
 */
Tuple *Hash_first(Hash *this) {
  if (this->root != NULL) {
    HashNode *current = this->root;
    while (current->left != NULL) {
      // Walk down to the leftmost child
      current = current->left;
    }
    // Warning: 1) this just create space for the Tuple itself,
    // not for the actual content
    // 2) you will need to free the Tuple after use, but the content
    // will not be freed until the node is freed
    Tuple *data = malloc(sizeof(Tuple));
    memcpy(data, &(current->data), sizeof(Tuple));
    return data;
  }
  // The Hash is empty
  return NULL;
}

/**
 * Gets the key/value pair for the last Hash item
 */
Tuple *Hash_last(Hash *this) {
  if (this->root != NULL) {
    HashNode *current = this->root;
    while (current->right != NULL) {
      // Walk down the rightmost child
      current = current->right;
    }
    // Same memory principle of Hash_first will apply
    Tuple *data = malloc(sizeof(Tuple));
    memcpy(data, &(current->data), sizeof(Tuple));
    return data;
  }
  // The Hash was empty
  return NULL;
}

/**
 * Destroys the given Tuple
 * Only the container, without destroying the associated data
 * Use HashNode_free() to destroy the data
 */
void Tuple_free(Tuple **this) {
  if (this != NULL) {
    // Clean memory for the Tuple
    memset(*this, 0, sizeof(Tuple));
    // Free and NULLify the Tuple pointer
    free(*this);
    *this = NULL;
  }
}

