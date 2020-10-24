#include<miniini.h>
#include<iostream>

int main()
{
    INIFile ini;
    std::string fname = "example.ini";
    if(!ini.OpenFile(fname))
        std::cout << "ERROR: Could not open example.ini" << std::endl;

    std::string sname = "general";
    INISection * general = ini.GetSection(sname);
    if(!general)
        std::cout << "ERROR: Missing section [general] in example.ini"
                  << std::endl;

    std::string appname;
    int appbuild;
    float brightness;
    bool fullscreen;
    std::string appname_tag = "AppName";
    std::string appbuild_tag = "AppBuild";
    std::string brightness_tag = "Brightness";
    std::string fullscreen_tag = "FullScreen";
    if(!general->ReadString(appname_tag, appname))
        std::cout << "ERROR: Missing tag AppName= in section [" << sname 
                  << "] in example.ini" << std::endl;
    if(!general->ReadInt(appbuild_tag, appbuild))
        std::cout << "ERROR: Missing or invalid tag AppBuild= in section [" 
                  << sname << "] in example.ini" << std::endl;
    if(!general->ReadFloat(brightness_tag, brightness))
        std::cout << "ERROR: Missing or invalid tag Brightness= in section [" 
                  << sname << "] in example.ini" << std::endl;
    if(!general->ReadBool(fullscreen_tag, fullscreen))
        std::cout << "ERROR: Missing or invalid tag FullScreen= in section [" 
                  << sname << "] in example.ini" << std::endl;
    std::cout << appname << " build " << appbuild << " fullscreen is " 
              << fullscreen << " brightness is " << brightness << std::endl;

    return 0;
}
