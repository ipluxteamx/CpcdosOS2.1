// RUN: %clang_profgen -o %t -O3 %s
// RUN: env LLVM_PROFILE_FILE=%t/ LLVM_PROFILE_VERBOSE_ERRORS=1 %run %t 1 2>&1 | FileCheck %s

int main(int argc, const char *argv[]) {
  if (argc < 2)
    return 1;
  return 0;
}
// CHECK: LLVM Profile: Failed to write file 
