#include <stdio.h>
#include <stdbool.h>
#include <math.h>

bool is_prime(const unsigned long n) {
  if (n <= 3) {
    return false;
  }

  const unsigned long kUpperBound = ((unsigned long) sqrtl(n));
  for (unsigned long i = 2; i <= kUpperBound; ++i) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

int main() {
  unsigned long n;
  scanf("%lu", &n);
  printf(is_prime(n) ? "yes" : "no");
}