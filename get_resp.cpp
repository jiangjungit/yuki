#include "get_resp.h"
#include "json.hpp"
#include "img_group.h"
#include "yuki.h"
#ifdef WIN_OK_H
#include <errno.h>
#include <windows.h>

// 转码
std::string utf8_gbk(const char *src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, nullptr, 0);
    wchar_t *wszGBK = new wchar_t[len + 1];
    std::memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, nullptr, 0, nullptr, nullptr);
    char *szGBK = new char[len + 1];
    std::memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, nullptr, nullptr);
    std::string strTemp(szGBK);
    if (wszGBK)
    {
        delete[] wszGBK;
    }
    if (szGBK)
    {
        delete[] szGBK;
    }
    return strTemp;
}
#endif

// 下载图片
int download_img(const std::string &url, const std::string &save_path, const curl_slist *chunk)
{
    CURL *curl = nullptr;
    curl = curl_easy_init();
    int status = 1;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());      //图片链接
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);     //请求头
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);        //关闭进度表
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite); //保存函数
        FILE *pagefile = nullptr;
#ifdef WIN_OK_H
        errno_t err = fopen_s(&pagefile, save_path.c_str(), "wb"); //Win下的保存
        if (err == 0)
#else
        pagefile = fopen(save_path.c_str(), "wb");
        if (pagefile)
#endif
        {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile); //设置文件指针
            curl_easy_perform(curl);                             //启动保存
            fclose(pagefile);
            status = 0;
        }
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "\nError: download_img-curl_easy_init"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    }
    return status;
}

// 构造基本请求头，注意释放工作是调用者的责任
curl_slist *base_chunk(const std::string &origin, const std::string &referer)
{
    curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "sec-ch-ua: Google Chrome 87");
    chunk = curl_slist_append(chunk, "sec-ch-ua-mobile: ?0");
    chunk = curl_slist_append(chunk, "sec-fetch-dest: empty");
    chunk = curl_slist_append(chunk, "sec-fetch-mode: cors");
    chunk = curl_slist_append(chunk, "sec-fetch-site: same-site");
    chunk = curl_slist_append(chunk, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36");
    if (origin.length() > 0)
    {
        chunk = curl_slist_append(chunk, ("origin: " + origin).c_str());
    }
    if (referer.length() > 0)
    {
        chunk = curl_slist_append(chunk, ("referer: " + referer).c_str());
    }
    return chunk;
}

//保存到内存，即把返回数据存入变量，一次响应很可能被多次调用
static size_t write_memory_callback(void *contents, const size_t size, const size_t nmemb, MemoryStruct *mem)
{
    size_t realsize = size * nmemb; //当前处理数据量
    //申请已有数据+当前数据+尾0内存
    char *ptr = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
    if (ptr == nullptr)
    {
        std::cerr << "\nError: write_memory_callback: No enough memory"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        return 0; //返回处理量与待处理量不统一可引发报错
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize); //从上次数据末尾处起追加新数据
    mem->size += realsize;                                 //数据量更新
    mem->memory[mem->size] = 0;                            //尾零结束
    return realsize;
}

// 按设置执行网络请求并写入内存
CURLcode perform_get(CURL *curl, const curl_slist *chunk, MemoryStruct &mem, const std::string &url)
{
    mem.memory = static_cast<char *>(malloc(1)); //随着调用realloc增长
    if (mem.memory == nullptr)
    {
        std::cerr << "\nError: perform_get: No enough memory"
                  << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
        return CURLE_OUT_OF_MEMORY;
    }
    mem.size = 0; //此时无数据
#ifdef DEBUG
    std::cout << "\ncall perform_get: url=\n"
              << url << std::endl;
#endif
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);                    //请求头
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());                     //请求地址
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback); //写入内存的钩子函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);                      //设置内存地址
    return curl_easy_perform(curl);
}

// 解析json格式字符串并获取页面总数
int get_all_count(const char *s)
{
#ifdef DEBUG
    std::cout << "\ncall get_all_count: s=\n"
#ifdef WIN_OK_H
              << utf8_gbk(s) << std::endl;
#else
              << s << std::endl;
#endif
#endif
    const nlohmann::json j = nlohmann::json::parse(s);
    int code = j["code"].get<int>();
    if (!code)
    {
        return j["data"]["all_count"].get<int>();
    }
    return -1;
}

// 解析json格式字符串并获取用户名
std::string get_name(const char *s)
{
#ifdef DEBUG
    std::cout << "\ncall get_name: s=\n"
#ifdef WIN_OK_H
              << utf8_gbk(s) << std::endl;
#else
              << s << std::endl;
#endif
#endif
    const nlohmann::json j = nlohmann::json::parse(s);
    int code = j["code"].get<int>();
    std::string name;
    if (!code)
    {
        name = j["data"]["name"].get<std::string>();
#ifdef WIN_OK_H
        name = utf8_gbk(name.c_str());
#endif
    }
    return name;
}

// 解析json格式字符串并获取动态说明和图片地址
img_group get_img_group(const char *s, const std::string &one_doc_id)
{
#ifdef DEBUG
    std::cout << "\ncall get_img_group: one_doc_id=" << one_doc_id << "\ns=\n"
#ifdef WIN_OK_H
              << utf8_gbk(s) << std::endl;
#else
              << s << std::endl;
#endif
#endif
    img_group g;
    g.doc_id = one_doc_id; //动态ID
    nlohmann::json j = nlohmann::json::parse(s);
    int code = j["code"].get<int>();
    if (!code)
    {
        j = j["data"]["item"];
        g.description = j["description"].get<std::string>(); //说明
        g.upload_time = j["upload_time"].get<std::string>(); //上传时间YYYY-MM-DD HH:mm:ss
        for (const nlohmann::json &item : j["pictures"])
        { //图片链接地址
            g.imgs.push_back(item["img_src"].get<std::string>());
        }
    }
    return g;
}

// 解析json格式字符串并获取当前页的动态编号列表
std::vector<std::string> doc_list(const char *s)
{
#ifdef DEBUG
    std::cout << "\ncall doc_list: s=\n"
#ifdef WIN_OK_H
              << utf8_gbk(s) << std::endl;
#else
              << s << std::endl;
#endif
#endif
    const nlohmann::json j = nlohmann::json::parse(s);
    std::vector<std::string> doc_list;
    int code = j["code"].get<int>();
    if (!code)
    {
        for (const nlohmann::json &item : j["data"]["items"])
        { //动态编号数字很大，使用长数据位防止溢出
            doc_list.push_back(std::to_string(item["doc_id"].get<unsigned long long>()));
        }
    }
    return doc_list;
}

// 写入上传时间和动态说明到文件
int write_comment(const std::string &txt_name, const std::string &upload_time, const std::string &description)
{
    std::ofstream file(txt_name.c_str()); //打开文件
    if (file)
    {
        file << upload_time << "\n"
             << description << std::endl;
        file.close();
        return 0;
    }
    std::cerr << "\nError: write_comment: Failed in opening File: " << txt_name
              << "\n(⊙︿⊙) 对不起,我似乎遇到了点麻烦" << std::endl;
    return 1;
}