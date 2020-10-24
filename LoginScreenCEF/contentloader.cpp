#include "contentloader.h"
#include "helpers.h"
ContentLoader::ContentLoader(const std::wstring filename)
{
    FILE* zfile;
    errno_t e=_wfopen_s(&zfile,filename.c_str(),L"rb");
    my_assert((e==NULL)&&(zfile!=nullptr));

    zip_source_t *src=zip_source_filep_create(zfile,0,-1,nullptr);
    my_assert(src!=nullptr);
    zip=zip_open_from_source(src,ZIP_RDONLY,nullptr);
    my_assert(zip!=nullptr);


    zip_file_t *fabout=zip_fopen(zip,"about.txt",ZIP_FL_ENC_GUESS);
    if(fabout==nullptr)return;
    zip_stat_t astat;
    zip_stat_init(&astat);
    zip_stat(zip,"about.txt",NULL,&astat);

    if((astat.valid&ZIP_STAT_SIZE)!=0){
        size_t asize=static_cast<int64_t>(astat.size);
        archive_info.resize((asize-1)/sizeof(wchar_t)+2);
        memset(archive_info.data(),0,asize+2);
        zip_fread(fabout,archive_info.data(),asize);
    }else{
        size_t asize=0;
        while(1){
            archive_info.resize(asize+64);
            memset(archive_info.data()+asize,0,64*sizeof(wchar_t));
            zip_int64_t ret=zip_fread(fabout,archive_info.data()+asize,64*sizeof(wchar_t));
            if(ret<63*sizeof(wchar_t))
                break;
            asize+=64;
        }
    }
    zip_fclose(fabout);
}
bool ContentLoader::open(const char *fname){
    size=-1;
    file=zip_fopen(zip,fname,ZIP_FL_ENC_GUESS);
    if(file==nullptr){
        return false;
    }
    //MessageBoxA(0,fname,"file",MB_OK);
    zip_stat_init(&stat);
    zip_stat(zip,fname,NULL,&stat);
    if((stat.valid&ZIP_STAT_SIZE)!=0)
        size=static_cast<int64_t>(stat.size);

    zip_uint32_t cbcomment;
    comment="";
    const char* cmt;
    if((stat.valid&ZIP_STAT_INDEX)!=0)
        cmt=zip_file_get_comment(zip,stat.index,&cbcomment,ZIP_FL_ENC_GUESS);
    if(cbcomment>0)
        comment=std::string(cmt,cbcomment);

    return true;
}
size_t ContentLoader::read(void* data,size_t size){
    if(file==nullptr)return 0;
    auto ret=zip_fread(file,data,size);
    if(ret<0)return 0;
    else return ret;
}
bool ContentLoader::seek(int64_t offset,int pos){
    return (0==zip_fseek(file,offset,pos));
}
int64_t ContentLoader::tell(){
    return zip_ftell(file);
}
void ContentLoader::close(){
    if(file!=nullptr)
        zip_fclose(file);
}
const std::string& ContentLoader::getComment(){
    return comment;
}
int64_t ContentLoader::getSize(){
    return size;
}
