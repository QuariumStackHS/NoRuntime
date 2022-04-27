#ifndef SHA1_HPP
#define SHA1_HPP


#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};

#endif /* SHA1_HPP */
inline int MatchSHA1File(std::string StorageName,std::string Filename){
    SHA1 Creaper=SHA1();
    std::string CurrentKey=Creaper.from_file(Filename);
    std::ifstream StorageFile(StorageName);
    std::stringstream ss;
    ss<<StorageFile.rdbuf();
    StorageFile.close();
    if(strcmp(ss.str().c_str(),CurrentKey.c_str())==0){
        return 1;
        
    }
    else{
        std::ofstream StorageFileOverwrite(StorageName);
        StorageFileOverwrite<<CurrentKey;
        StorageFileOverwrite.close();
        return 0;
    }

}