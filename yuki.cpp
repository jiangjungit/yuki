#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <limits>
#include "yuki.h"
#include "get_resp.h"
#include "biliparser.h"
#include "util.h"

// 主要功能
int yuki(int argc, char const *argv[])
{
    int err = 0;
    std::string user_id, save_path, from_time, to_time;
    if (argc > 1)
    {
        err = option_parse(argc, argv, user_id, save_path, from_time, to_time);
    }
    else
    {
        err = option_input(user_id, save_path, from_time, to_time);
    }
    if (!err)
    {
        BiliAlbumParser bl(user_id);
        if (!from_time.empty() && !to_time.empty() && bl.set_time(from_time, to_time, argc <= 1)) //设置时间
        {
            err = 1;
            std::cerr << "-t参数错误" << std::endl;
        }
        if (!err)
        {
            if (curl_global_init(CURL_GLOBAL_DEFAULT)) //全局初始化
            {
                std::cerr << "\nError: main-curl_global_init" << std::endl;
            }
            else
            {
                err = bl.parse(save_path, argc - 1);
                curl_global_cleanup();
            }
        }
    }
    return err;
}

// 抬头
void title_print()
{
    //\x1b[38;2;<r>;<g>;<b>m设置颜色,\x1b[m恢复默认颜色
    std::cout << "\n\
\x1b[38;2;144;50;73m██╗   ██╗ \x1b[38;2;100;164;192m██╗   ██╗ \x1b[38;2;165;142;152m██╗  ██╗ \x1b[38;2;100;164;192m██╗\n\
\x1b[38;2;144;50;73m╚██╗ ██╔╝ \x1b[38;2;100;164;192m██║   ██║ \x1b[38;2;165;142;152m██║ ██╔╝ \x1b[38;2;100;164;192m██║\n\
\x1b[38;2;144;50;73m ╚████╔╝  \x1b[38;2;100;164;192m██║   ██║ \x1b[38;2;165;142;152m█████╔╝  \x1b[38;2;100;164;192m██║\n\
\x1b[38;2;144;50;73m  ╚██╔╝   \x1b[38;2;100;164;192m██║   ██║ \x1b[38;2;165;142;152m██╔═██╗  \x1b[38;2;100;164;192m██║\n\
\x1b[38;2;144;50;73m   ██║    \x1b[38;2;100;164;192m╚██████╔╝ \x1b[38;2;165;142;152m██║  ██╗ \x1b[38;2;100;164;192m██║\n\
\x1b[38;2;144;50;73m   ╚═╝    \x1b[38;2;100;164;192m ╚═════╝  \x1b[38;2;165;142;152m╚═╝  ╚═╝ \x1b[38;2;100;164;192m╚═╝\x1b[m"
              << std::endl;
#ifdef YUKI_VERSION_MAJOR
    std::cout << "版本: " << YUKI_VERSION_MAJOR << "." << YUKI_VERSION_MINOR << "." << YUKI_VERSION_PATCH << "\nhttps://github.com/jiangjungit/yuki" << std::endl;
#endif
}

int main(int argc, char const *argv[])
{
#ifdef WIN_OK_H
    DWORD dwOriginalOutMode = 0;
    int color_err = set_color_cmd(dwOriginalOutMode, false);
    if (color_err)
    {
        std::cerr << "Warning: failed in setting virtual terminal" << std::endl;
    }
#endif
    title_print();
    int err = yuki(argc, argv);
    if (argc <= 1)
    {
        std::cout << "按Enter键退出" << std::endl;
        std::cin.get();
    }
#ifdef WIN_OK_H
    if (!color_err)
    { //恢复显示设置
        set_color_cmd(dwOriginalOutMode, true);
    }
#endif
    if (err == 2)
    {
        err = 0; //帮助返回err=2是正常功能, 主程序返回0
    }
    return err;
}