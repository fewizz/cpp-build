extern "C" void test();

#define on_test __attribute__((constructor)) void

int main(int argc, char* argv[]) {}