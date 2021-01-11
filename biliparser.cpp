#include <algorithm>
#include <limits>
#include <chrono>
#include <thread>
#include <vector>
#include "biliparser.h"
#include "get_resp.h"
#include "yuki.h"
#include "util.h"

#define TIME_PAUSE_MAJOR 1500
#define TIME_PAUSE_MINOR 200

BiliAlbumParser::~BiliAlbumParser() {}
BiliAlbumParser::BiliAlbumParser() : page_num(0), from_time("2009-06-01")
{
    unsigned year, month, day;
    now_time(year, month, day);
    if (time_str(year, month, day, to_time))
    {
        to_time = "2099-12-31";
    }
}
BiliAlbumParser::BiliAlbumParser(const BiliAlbumParser &b) : user_id(b.user_id), user_name(b.user_name), page_num(b.page_num), from_time(b.from_time), to_time(b.to_time) {}
BiliAlbumParser::BiliAlbumParser(const std::string &s) : page_num(0), from_time("2009-06-01")
{
    if (!check_user_id(s))
    {
        user_id = s;
    }
    unsigned year, month, day;
    now_time(year, month, day);
    if (time_str(year, month, day, to_time))
    {
        to_time = "2099-12-31";
    }
}

std::string BiliAlbumParser::get_user_id() const
{
    return user_id;
}

std::string BiliAlbumParser::get_user_name() const
{
    return user_name;
}
// 获取页面数量
int BiliAlbumParser::parse_page_num()
{
    CURL *curl = nullptr;
    curl = curl_easy_init();
    int status = 1;
    if (curl)
    {
        struct curl_slist *chunk = base_chunk("https://space.bilibili.com", "https://space.bilibili.com/");
        const std::string url = "https://api.vc.bilibili.com/link_draw/v1/doc/upload_count?uid=" + user_id;
        struct MemoryStruct mem;
        CURLcode res = perform_get(curl, chunk, mem, url);
        if (res == CURLE_OK)
        {
            int code_or_page = get_all_count(mem.memory); //解析页面数量
            if (code_or_page < 0)
            {
                std::cerr << "\n"
                          << curl_easy_strerror(res)
                          << "Error: parse_page_num-code_or_page=" << code_or_page << "\n"
                          << "\n(⊙︿⊙) 好像被小电视发现了,待会儿再试吧" << std::endl;
            }
            else
            {
                page_num = code_or_page;
                status = 0;
            }
        }
        else
        {
            std::cerr << "\n"
                      << curl_easy_strerror(res) << "\nError: parse_page_num-perform_get"
                      << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        }
        free(mem.memory);
        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "\nError: parse_page_num-curl_easy_init"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    }
    return status;
}

// 获取用户名
int BiliAlbumParser::parse_user_name()
{
    CURL *curl = nullptr;
    curl = curl_easy_init();
    int status = 1;
    if (curl)
    {
        struct curl_slist *chunk = base_chunk("https://space.bilibili.com", "https://space.bilibili.com/");
        const std::string url = "https://api.bilibili.com/x/space/acc/info?jsonp=jsonp&mid=" + user_id;
        struct MemoryStruct mem;
        CURLcode res = perform_get(curl, chunk, mem, url);
        if (res == CURLE_OK)
        {
            std::string code_or_name = get_name(mem.memory); //解析用户名
            if (code_or_name.empty())
            {
                std::cerr << "\n"
                          << curl_easy_strerror(res)
                          << "Error: parse_user_name-code_or_name=" << code_or_name << "\n"
                          << "\n(⊙︿⊙) 好像被小电视发现了,待会儿再试吧" << std::endl;
            }
            else
            {
                user_name = code_or_name;
                status = 0;
            }
        }
        else
        {
            std::cerr << "\n"
                      << curl_easy_strerror(res) << "\nError: parse_user_name-perform_get"
                      << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        }
        free(mem.memory);
        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "\nError: parse_user_name-curl_easy_init"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    }
    return status;
}

int BiliAlbumParser::get_page_num() const
{
    return page_num;
}

// 设置起止时间
int BiliAlbumParser::set_time(const std::string &beg_time, const std::string &end_time, const bool checked)
{
    if (checked)
    {
        from_time = beg_time;
        to_time = end_time;
        return 0;
    }
    std::string tmp_time;
    if (string_to_time(beg_time, tmp_time))
    {
        return 1;
    }
    else
    {
        from_time = tmp_time;
    }
    if (string_to_time(end_time, tmp_time))
    {
        return 1;
    }
    else
    {
        to_time = tmp_time;
    }
    return 0;
}

// 获取一个页面上的多个动态id
std::vector<std::string> BiliAlbumParser::parse_page_doc_id(const struct curl_slist *chunk, const std::string &url)
{
    CURL *curl = nullptr;
    curl = curl_easy_init();
    std::vector<std::string> page_doc;
    if (curl)
    {
        struct MemoryStruct mem;
        CURLcode res = perform_get(curl, chunk, mem, url);
        if (res == CURLE_OK)
        {
            page_doc = doc_list(mem.memory);
            if (page_doc.empty())
            {
                std::cerr << "\n"
                          << curl_easy_strerror(res) << "\nError: parse_page_doc_id-perform_get"
                          << "\n(⊙︿⊙) 好像被小电视发现了,待会儿再试吧" << std::endl;
            }
        }
        else
        {
            std::cerr << "\n"
                      << curl_easy_strerror(res) << "\nError: parse_page_doc_id-perform_get"
                      << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        }
        free(mem.memory);
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "\nError: parse_page_doc_id-curl_easy_init"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    }
    return page_doc;
}

// 保存图片和说明
int BiliAlbumParser::parse_doc_id(const std::string &save_path)
{
    struct curl_slist *chunk = base_chunk("https://space.bilibili.com", "https://space.bilibili.com/");
    struct curl_slist *img_chunk = base_chunk("https://h.bilibili.com", "https://h.bilibili.com/");
    const std::string url = "https://api.vc.bilibili.com/link_draw/v1/doc/doc_list?uid=" + user_id + "&page_size=30&biz=all&page_num=";
    const std::chrono::milliseconds ps_mj(TIME_PAUSE_MAJOR), ps_mn(TIME_PAUSE_MINOR); //暂停时间
    std::vector<std::string> fail_doc;
    int n = 0, remain_doc = 0, err = 0; //成功图片数量和状态码
    for (int i = 0; i < page_num; i++)
    {
        std::vector<std::string> page_doc = parse_page_doc_id(chunk, url + std::to_string(i));
        if (page_doc.empty())
        {
            remain_doc = 1;
            break; //解析出错结束大循环
        }
        for (const auto &d : page_doc) //返回的动态按时间从晚到早排序
        {
            img_group g = parse_img_group(img_chunk, d); //一个动态下的若干图片
            if (g.imgs.empty())
            {
                remain_doc = 1;
                i = page_num; //解析出错结束大循环
                break;
            }
            std::this_thread::sleep_for(ps_mj); //暂停
            if (g.upload_time.substr(0, 10) > to_time)
            { //时间比较是字符串比较，前十位格式为YYYY-MM-DD
                std::cout << "\033[K跳过: " << g.upload_time << " 动态ID: " << d << "\r" << std::flush;
                continue; //还未进入时间段
            }
            if (g.upload_time.substr(0, 10) < from_time)
            {
                i = page_num; //结束大循环
                std::cout << "\033[K跳过: " << g.upload_time << " 动态ID: " << d << "\r" << std::flush;
                break; //不在时间段内，小循环结束
            }
            const std::string doc_path = join_path(save_path, d); //动态的保存路径
            if ((check_dir(doc_path) && make_direct(doc_path)) ||
                (write_comment(join_path(doc_path, "description.txt"), g.upload_time, g.description)))
            { //动态路径不存在又新建失败，或是不能写入说明
                fail_doc.push_back(d);
                continue; //记录失败
            }
            for (const auto &u : g.imgs)
            { //保存图片
                const std::string m = join_path(doc_path, basename(u));
                if (check_dir(m) && download_img(u, m, img_chunk))
                {
                    fail_doc.push_back(d);
                    break;
                }
                n++;
                std::this_thread::sleep_for(ps_mn); //暂停
            }
            std::cout << "\033[K在下啦: " << g.upload_time << " 动态ID: " << d << " 图: " << g.imgs.size() << "/" << n << "\r" << std::flush;
        }
    }
    curl_slist_free_all(chunk);
    curl_slist_free_all(img_chunk);
    std::cout << "\033[K" << std::endl;
    if (fail_doc.size())
    { //输出失败列表
        err = 1;
        std::cerr << "\n(T^T) 这几个动态ID失败了:" << std::endl;
        for (auto &d : fail_doc)
        {
            std::cout << d << std::endl;
        }
    }
    if (remain_doc)
    {
        err = 1;
        std::cerr << "\n(T^T) 未完成全部页面解析" << std::endl;
    }
    if (n > 0)
    {
        std::cout << "(*^▽^*) 一共搞到" << n << "张好康的" << std::endl;
    }
    return err;
}

//解析一个动态下的图片
img_group BiliAlbumParser::parse_img_group(const struct curl_slist *chunk, const std::string &one_doc_id)
{
    CURL *curl = nullptr;
    curl = curl_easy_init();
    img_group g;
    if (curl)
    {
        const std::string url = "https://api.vc.bilibili.com/link_draw/v1/doc/detail?doc_id=" + one_doc_id;
        struct MemoryStruct mem;
        CURLcode res = perform_get(curl, chunk, mem, url);
        if (res == CURLE_OK)
        {
            g = get_img_group(mem.memory, one_doc_id);
            if (g.imgs.empty())
            {
                std::cerr << "\n"
                          << curl_easy_strerror(res) << "\nError: parse_img_group-get_img_group"
                          << "\n(⊙︿⊙) 好像被小电视发现了,待会儿再试吧" << std::endl;
            }
        }
        else
        {
            std::cerr << "\n"
                      << curl_easy_strerror(res) << "\nError: parse_img_group-perform_get"
                      << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        }
        free(mem.memory);
    }
    else
    {
        std::cerr << "\nError: parse_img_group-curl_easy_init"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    }
    curl_easy_cleanup(curl);
    return g;
}

// 设置、解析和下载
int BiliAlbumParser::parse(const std::string &save_path, const int force)
{
    const std::string user_path = join_path(save_path, user_id);
    if (check_save_path(user_path))
    {
        std::cerr << "\n(￣へ￣) 哼你骗我, 这里连文件夹都没建: " << save_path << std::endl;
        return 1;
    }
    if (!parse_page_num() && !parse_user_name())
    {
        std::cout << "(<ゝω·)☆ 接受你的挑战\n"
                  << "保存路径: " << save_path << std::endl
                  << *this << std::endl;
        if (!force)
        {
            std::cout << "(>ω·* )/ 准备起飞? [y]/n" << std::endl;
            char c = std::cin.get();
            clean_cin();
            if (c == 'n' || c == 'N')
            {
                std::cout << "收工啦ε=ε=ε=ε=ε=(*·ω-q)" << std::endl;
                return 0;
            }
        }
        return parse_doc_id(user_path);
    }
    return 1;
}

// 打印设置信息
std::ostream &operator<<(std::ostream &os, const BiliAlbumParser &b)
{
    os << "用户编号: " << b.get_user_id() << "\n"
       << "用户昵称: " << b.get_user_name() << "\n"
       << "开始时间: " << b.from_time << "\n"
       << "结束时间: " << b.to_time << "\n"
       << "检测页数: " << b.get_page_num();
    return os;
}
