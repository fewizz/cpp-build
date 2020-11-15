extern "C" void test();

#define test __attribute__((constructor)) void

int main(int argc, char* argv[]) {}