#ifndef BILIALBUM
#define BILIALBUM
#include <iostream>
#include <string>
#include <initializer_list>
#include "img_group.h"

class BiliAlbumParser
{
    friend std::ostream &operator<<(std::ostream &os, const BiliAlbumParser &b);

private:
    std::string user_id;
    std::string user_name;
    int page_num;
    std::string from_time;
    std::string to_time;
    int parse_page_num();
    int parse_user_name();
    std::vector<std::string> parse_page_doc_id(const struct curl_slist *, const std::string &);
    int parse_doc_id(const std::string &);
    img_group parse_img_group(const struct curl_slist *, const std::string &);

public:
    ~BiliAlbumParser();
    BiliAlbumParser();
    BiliAlbumParser(const std::string &);
    BiliAlbumParser(const BiliAlbumParser &);
    int set_time(const std::string &, const std::string &, const bool);
    int parse(const std::string &, const int);
    std::string get_user_id() const;
    std::string get_user_name() const;
    int get_page_num() const;
};
std::ostream &operator<<(std::ostream &os, const BiliAlbumParser &b);
#endif