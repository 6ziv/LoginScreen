#include<miniini.h>
#include<iostream>

int main()
{
    INIFile ini;
    if(!ini.OpenFile("example.ini"))
        std::cout << "ERROR: Could not open example.ini" << std::endl;

    INISection * general = ini.GetSection("general");
    if(!general)
        std::cout << "ERROR: Missing section [general] in example.ini" 
                  << std::endl;

    const char * appname;
    int appbuild;
    float brightness;
    bool fullscreen;
    if(!general->ReadString("AppName", appname))
        std::cout << "ERROR: Missing tag AppName= in section [general] in example.ini" 
                  << std::endl;
    if(!general->ReadInt("AppBuild", appbuild))
        std::cout << "ERROR: Missing or invalid tag AppBuild= in section [general] in example.ini" 
                  << std::endl;
    if(!general->ReadFloat("Brightness", brightness))
        std::cout << "ERROR: Missing or invalid tag Brightness= in section [general] in example.ini" 
                  << std::endl;
    if(!general->ReadBool("FullScreen", fullscreen))
        std::cout << "ERROR: Missing or invalid tag FullScreen= in section [general] in example.ini" 
                  << std::endl;
    std::cout << appname << " build " << appbuild << " fullscreen is " 
              << fullscreen << " brightness is " << brightness << std::endl;

    return 0;
}
