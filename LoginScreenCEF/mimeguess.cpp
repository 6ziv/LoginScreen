#include "mimeguess.h"
#include <cstring>

const std::string_view& MIMEGuess::guess(const std::string_view& key){
    std::pair<std::string_view,std::string_view> searchkey={key,""};
    auto result=std::lower_bound(
            mimes.begin(),mimes.end(),searchkey,
            [](const std::pair<std::string_view,std::string_view> &a,
                const std::pair<std::string_view,std::string_view> &b
                )
            {
                return a.first<b.first;
            }
    );
    if(result->first==key){
        return result->second;
    }else
        return default_result;

}
