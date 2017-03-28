#include <string>
#include <climits>
#include <memory>

#include <util/util.h>

Unit unit() {
    return (Unit){};
}

Result<int, IntParseError> parse_int(char* str) {
    std::unique_ptr<char> endptr = std::make_unique<char>();
    char * c = endptr.get();
    long int large = strtol(str, &c, 10);

    if (large > INT_MAX || large < INT_MIN) {
        return Result<int, IntParseError>(TOO_LARGE);
    }

    if (*endptr != '\0') {
        return Result<int, IntParseError>(INVALID_STRING);
    }

    return Result<int, IntParseError>((int)large);
}

int better_rand(int limit) {
unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) limit + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}