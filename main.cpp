#include <map>
#include <iostream>

std::map <int, int> f;

int main()
{
    f.insert(1);
    std::cout << f.at(0) << std::endl;
}