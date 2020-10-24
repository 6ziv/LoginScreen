#include "conf.h"
#include <Windows.h>

void Conf::InitSettings(CefSettings &settings){

    settings.size=sizeof(cef_settings_t);

    cef_string_utf16_set(L"zh_cn",5,&settings.locale,1);
    cef_string_utf16_clear(&settings.log_file);
    cef_string_utf16_clear(&settings.cache_path);

    settings.no_sandbox=0;
    cef_string_utf16_set(L"No-Agent.Should-Not-Access-Internet",35,&settings.user_agent,1);
    //settings.log_severity=LOGSEVERITY_DISABLE;
    settings.log_severity=LOGSEVERITY_VERBOSE;
    cef_string_utf16_clear(&settings.user_data_path);//TODO:Move
    cef_string_utf16_clear(&settings.product_version);
    cef_string_utf16_clear(&settings.root_cache_path);
    //settings.background_color=CefColorSetARGB(255,255,255,255);

    cef_string_utf16_set(L"--harmony",9,&settings.javascript_flags,1);

    cef_string_utf16_clear(&settings.locales_dir_path);
    cef_string_utf16_clear(&settings.main_bundle_path);
    cef_string_utf16_clear(&settings.framework_dir_path);
    cef_string_utf16_clear(&settings.resources_dir_path);

    cef_string_utf16_set(L"*",1,&settings.accept_language_list,1);
    settings.external_message_pump=0;
    settings.pack_loading_disabled=0;
    settings.remote_debugging_port=0;
    cef_string_utf16_clear(&settings.browser_subprocess_path);

    settings.persist_session_cookies=0;
    settings.persist_user_preferences=0;
    settings.ignore_certificate_errors=1;
    settings.command_line_args_disabled=1;
    settings.multi_threaded_message_loop=0;
    settings.windowless_rendering_enabled=0;
    settings.uncaught_exception_stack_size=64;

    cef_string_utf16_clear(&settings.application_client_id_for_file_scanning);
}
void Conf::InitBrowserSettings(CefBrowserSettings &settings){
    settings.size=sizeof(cef_browser_settings_t);
    settings.webgl=STATE_DISABLED;
    settings.plugins=STATE_DISABLED;
    settings.databases=STATE_DISABLED;
    settings.javascript=STATE_ENABLED;
    settings.remote_fonts=STATE_ENABLED;
    settings.tab_to_links=STATE_DISABLED;
    settings.web_security=STATE_ENABLED;
    settings.image_loading=STATE_ENABLED;
    settings.local_storage=STATE_DISABLED;
    settings.background_color=CefColorSetARGB(255,255,255,255);
    cef_string_utf16_clear(&settings.default_encoding);
    settings.text_area_resize=STATE_ENABLED;
    settings.application_cache=STATE_DISABLED;
    settings.default_font_size=16;
    cef_string_utf16_clear(&settings.fixed_font_family);
    settings.minimum_font_size=4;
    cef_string_utf16_set(L"*",1,&settings.accept_language_list,1);
    settings.javascript_dom_paste=STATE_DISABLED;
    settings.windowless_frame_rate=60;
    cef_string_utf16_set(L"fantasy",7,&settings.fantasy_font_family,1);
    cef_string_utf16_set(L"monospace",9,&settings.fixed_font_family,1);
    cef_string_utf16_set(L"serif",5,&settings.serif_font_family,1);
    cef_string_utf16_set(L"sans-serif",10,&settings.sans_serif_font_family,1);
    cef_string_utf16_set(L"cursive",10,&settings.cursive_font_family,1);
    cef_string_utf16_set(L"serif",5,&settings.standard_font_family,1);

    settings.default_fixed_font_size=16;
    settings.javascript_close_windows=STATE_DISABLED;
    settings.minimum_logical_font_size=4;
    settings.file_access_from_file_urls=STATE_DISABLED;
    settings.javascript_access_clipboard=STATE_DISABLED;
    settings.image_shrink_standalone_to_fit=STATE_DEFAULT;
    settings.universal_access_from_file_urls=STATE_DISABLED;

    cef_string_utf16_set(L"utf-8",5,&settings.default_encoding,1);


}
std::wstring Conf::getResourceFile(){
    DWORD cbSize=0;
    if(ERROR_SUCCESS!=RegGetValueW(HKEY_LOCAL_MACHINE,L"Software\\LoginScreen",L"ResourceFile",RRF_RT_REG_EXPAND_SZ|RRF_RT_REG_SZ,NULL,NULL,&cbSize))return std::wstring();
    wchar_t* buffer=reinterpret_cast<wchar_t*>(malloc(cbSize));
    if(!buffer)return std::wstring();
    if(ERROR_SUCCESS!=RegGetValueW(HKEY_LOCAL_MACHINE,L"Software\\LoginScreen",L"ResourceFile",RRF_RT_REG_EXPAND_SZ|RRF_RT_REG_SZ,NULL,buffer,&cbSize))return std::wstring();
    std::wstring ret=std::wstring(buffer);
    free(buffer);
    return ret;
}
