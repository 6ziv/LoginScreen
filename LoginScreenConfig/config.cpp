#include "config.h"
#include <Windows.h>
#include <WinBase.h>
#include <Shlwapi.h>
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QDir>
#include "priv.h"
#include "../LoginScreenLibrary/guid.h"
void Config::restartAsAdmin(QString newcommand){
    QString qs;
    if(parsing){
        for(size_t i=1;i<QApplication::arguments().size();i++)
        qs.append(QApplication::arguments().at(i)+" ");
    }else{
        qs=newcommand;
    }
    Priv::RunAsAdmin(qs);
}
void Config::install(){
    if(!Priv::IsAdmin())restartAsAdmin("--install");
    wchar_t t;
    auto l=GetSystemDirectoryW(&t,0);
    wchar_t* system=new wchar_t[l+1];
    GetSystemDirectoryW(system,l);
    system[l]=L'\0';
    QFile::copy(QDir(QApplication::applicationDirPath()).filePath("LoginScreenLibrary.dll"),QDir(QString::fromWCharArray(system)).filePath("LoginScreenLibrary.dll"));
    delete[] system;
    QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\LoginScreen",QSettings::NativeFormat);
    settings1.setValue("Application",QDir(QApplication::applicationDirPath()).filePath("LoginScreenCEF.exe"));
    settings1.sync();
}
void Config::uninstall(){
    if(!Priv::IsAdmin())restartAsAdmin("--uninstall");
    wchar_t t;
    auto l=GetSystemDirectoryW(&t,0);
    wchar_t* system=new wchar_t[l+1];
    GetSystemDirectoryW(system,l);
    system[l]=L'\0';
    QFile(QDir(QString::fromWCharArray(system)).filePath("LoginScreenLibrary.dll")).remove();
    delete[] system;

    QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE",QSettings::NativeFormat);
    settings1.remove("LoginScreen");
    settings1.sync();
}
void Config::activate(){
    if(!Priv::IsAdmin())restartAsAdmin("--activate");
    LPOLESTR str;
    if(S_OK==StringFromCLSID(CLSID_CSample,&str)){
        QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\"+QString::fromWCharArray(str),
                           QSettings::NativeFormat);
        settings1.setValue(".","LoginScreenLibrary");
        settings1.sync();
        QSettings settings2("HKEY_CLASSES_ROOT\\CLSID\\"+QString::fromWCharArray(str),QSettings::NativeFormat);
        settings2.setValue(".","LoginScreenLibrary");
        settings2.sync();
        QSettings settings3("HKEY_CLASSES_ROOT\\CLSID\\"+QString::fromWCharArray(str)+"\\InprocServer32",QSettings::NativeFormat);
        settings3.setValue(".","LoginScreenLibrary.dll");
        settings3.setValue("ThreadingModel","Apartment");
        settings3.sync();
        CoTaskMemFree(str);
    }
}
void Config::deactivate(){
    if(!Priv::IsAdmin())restartAsAdmin("--deactivate");
    LPOLESTR str;
    if(S_OK==StringFromCLSID(CLSID_CSample,&str)){
        QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers",QSettings::NativeFormat);
        settings1.remove(QString::fromWCharArray(str));
        settings1.sync();
        QSettings settings2("HKEY_CLASSES_ROOT\\CLSID\\",QSettings::NativeFormat);
        settings2.remove(QString::fromWCharArray(str));
        settings2.sync();
        CoTaskMemFree(str);
    }
}
void Config::resource(QString s){
    if(!Priv::IsAdmin())restartAsAdmin("--resource="+s);
    QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\LoginScreen",QSettings::NativeFormat);
    settings1.setValue("ResourceFile",s);
}
bool Config::isinstalled(){
    wchar_t t;
    auto l=GetSystemDirectoryW(&t,0);
    wchar_t* system=new wchar_t[l+1];
    GetSystemDirectoryW(system,l);
    system[l]=L'\0';
    if(!QFile::exists(QDir(QString::fromWCharArray(system)).filePath("LoginScreenLibrary.dll"))){
        delete[] system;
        return false;
    }
    delete[] system;
    QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\LoginScreen",QSettings::NativeFormat);
    if(!settings1.contains("Application"))
        return false;
    return true;
}
bool Config::isactivated(){
    LPOLESTR str;
    if(S_OK==StringFromCLSID(CLSID_CSample,&str)){
        QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers",
                           QSettings::NativeFormat);
        bool ret=settings1.contains(QString::fromWCharArray(str)+"/.");
        CoTaskMemFree(str);
        return ret;
    }
    return false;
}
QString Config::resource(){
    QSettings settings1("HKEY_LOCAL_MACHINE\\SOFTWARE\\LoginScreen",QSettings::NativeFormat);
    return settings1.value("ResourceFile").toString();
}
