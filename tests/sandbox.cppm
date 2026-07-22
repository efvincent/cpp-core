import core;

using namespace base;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  Arena arena = Arena();
  if (!arena.init(MB(3))) {
    console::print_err(Str8("failed to allocate arena memory."));
    return 1;
  }

  return 0;
}