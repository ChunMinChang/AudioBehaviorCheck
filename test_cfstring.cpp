#include <CoreFoundation/CoreFoundation.h>
#include <iostream> // for std::cout, std::endl
#include <string.h> // for strlen
#include <cstdlib>

const char* STRING = "hello world!";

// References:
// https://github.com/opensource-apple/CF/blob/3cc41a76b1491f50813e28a4ec09954ffa359e6f/CFString.c#L1605
// https://github.com/servo/core-foundation-rs/blob/2aac8fb85b5b114673280e273c04219c0c360e54/core-foundation/src/string.rs#L125
// https://github.com/servo/core-foundation-rs/blob/2aac8fb85b5b114673280e273c04219c0c360e54/io-surface/src/lib.rs#L48
int main() {
//   CFStringRef cfStringRef = CFStringCreateWithBytesNoCopy(
//     kCFAllocatorDefault, (const UInt8*)STRING, (CFIndex)strlen(STRING),
//     kCFStringEncodingUTF8, false, kCFAllocatorNull);
//   CFRelease(cfStringRef);

  char* words = (char*) calloc(1, strlen(STRING) * sizeof(char) + 1);
  strcpy(words, STRING);

  CFStringRef cfStringRef = CFStringCreateWithBytesNoCopy(
    kCFAllocatorDefault, (const UInt8*)words, (CFIndex)strlen(words),
    kCFStringEncodingUTF8, false, kCFAllocatorNull);
  CFRelease(cfStringRef);

  free(words);
  return 0;
}