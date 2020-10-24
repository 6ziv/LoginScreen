#include<miniini.h>
#include<iostream>
#include<fstream>

int main()
{
    char buf[64];
    std::ifstream fin( "example.ini" );
    fin.read(buf, 63);
    if(fin.bad())
        std::cout << "ERROR: Could not read from example.ini";
    buf[63] = '\0';

    INIFile ini;
    if(!ini.LoadBuffer(buf, 64))
        std::cout << "ERROR: error in parsing ini file data";

    INISection * section = ini.GetSection("section");
    if(!section)
        std::cout << "ERROR: Missing section [section]" << std::endl;
    int answer;
    if(!section->ReadInt("answer", answer))
        std::cout << "ERROR: Missing tag answer=" << std::endl;
    std::cout << "The answer is " << answer << std::endl;
    return 0;
}