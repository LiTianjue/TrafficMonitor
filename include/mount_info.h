#ifndef MOUNT_INFO_H
#define MOUNT_INFO_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * 挂载点信息结构
 */
struct MountInfo {
    std::string device;      // 设备名称（如 /dev/sda1）
    std::string mount_point; // 挂载点路径（如 /home）
    std::string filesystem;  // 文件系统类型（如 ext4, xfs, tmpfs）
    std::string options;     // 挂载选项（如 rw,noatime）
    int dump_freq;           // dump频率（通常为0或1）
    int pass_no;             // fsck检查顺序（通常为0, 1, 2）
    bool valid;              // 是否有效（是否找到）

    MountInfo() : dump_freq(0), pass_no(0), valid(false) {}
};

/**
 * 获取所有挂载点信息
 * @return 挂载点信息列表
 */
std::vector<MountInfo> get_mount_info();

/**
 * 获取指定挂载点的信息
 * @param mount_point 挂载点路径
 * @return MountInfo 挂载信息，如果未找到则valid字段为false
 */
MountInfo get_mount_info(const std::string& mount_point);

/**
 * 检查指定路径是否在某个挂载点下
 * @param path 要检查的路径
 * @return 对应的挂载点信息，如果未找到则valid字段为false
 */
MountInfo find_mount_for_path(const std::string& path);

#endif // MOUNT_INFO_H

