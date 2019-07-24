extern "C" int test(int count)
{
    return [&count]() { return count + 5; }();
}