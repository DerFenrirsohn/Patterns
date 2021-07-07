
#include <stdlib.h>

#include "pattern.hpp"
#include <unistd.h>

PatternHolder * ph = new PatternHolder();
int main(int argc, char *argv[])
{
    
    std::string str;
    // while(true)
    // {
    //     std::cin>>str;
    //     ph->input(str);
    //     ph->printmainpatterns(10);
    // }
    ph->readText("../data/(1) The Hunger Games.txt");
    ph->printmainpatterns(10);

    
    return 0;
}