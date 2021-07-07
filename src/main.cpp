
#include <stdlib.h>

#include <pattern.hpp>
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
    // Pattern p = Pattern(ph->str2pat("Bonjour, comment allez vous ?"));
    // Pattern p2 = Pattern(ph->simplify(ph->str2pat("Bonjour, comment allez vous ?")));
    // std::cout<<p2.getName()<<std::endl;

    // ph->input("Bonjour, coumment allez vous ?");
    // ph->input("Bonjour, coumment allez vous ?");
    // ph->input("Bonjour, coumment allez vous ?");

    
    return 0;
}