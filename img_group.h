#ifndef IMG_GROUP
#define IMG_GROUP
#include <string>
#include <vector>

// 一个动态的描述
struct img_group
{
    std::string doc_id;            //动态编号
    std::string description;       //说明
    std::string upload_time;       //上传时间
    std::vector<std::string> imgs; //图片地址
};
#endif