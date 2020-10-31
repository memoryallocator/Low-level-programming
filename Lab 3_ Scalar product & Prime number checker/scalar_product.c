#include <stdio.h>
#include <stddef.h>

const int kA[] = { 1, 2, 3, 4, 5 };
const int kB[] = { 9, 8, 7, 6, 5 };

const size_t kArrSize = sizeof(kA) / sizeof(kA[0]);

long long int scalar_product(const int x[], const int y[]) {
  long long int res = 0;
  for (size_t i = 0; i < kArrSize; ++i) {
    res += x[i] * y[i];
  }
  return res;
}

int main() {
  printf("%lld\n", scalar_product(kA, kB));
}