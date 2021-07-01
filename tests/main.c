#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "hash.h"
#include "hash_tests.h"

#ifndef LOCALE
#define LOCALE "en_GB.UTF-8"
#endif

int main() {
  if (strstr(LOCALE, "UTF-8") == NULL) {
    printf("The given locale (%s) does not support UTF-8\n", LOCALE);
    return EXIT_FAILURE;
  }

  if (!setlocale(LC_ALL, LOCALE)) {
    printf("Unable to set locale to '%s'\n", LOCALE);
    return EXIT_FAILURE;
  }

  printf("\nUsing locale: %s\n", LOCALE);

  printf("\n");

  printf("Basic tests\n");
  TestHash_new();
  TestHash_set();
  TestHash_first();
  TestHash_last();
  TestHash_delete();

  printf("\n");
  printf("\n");

  printf("Unicode tests\n");
  TestHash_unicode();

  printf("\n");

  printf("Unicode bulk tests\n");
  TestHash_bulk();

  printf("\n");
  printf("Done!\n\n");
  return EXIT_SUCCESS;
}
