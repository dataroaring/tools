#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <errno.h>
#include <endian.h>

#include <stdint.h>

#include <iostream>
using namespace std;


#define BLOCK_SIZE 4096

using __le64 = uint64_t;
using __le32 = uint32_t;
using __u32 = uint32_t;
using __le16 = uint16_t;
using __u8 = uint8_t;


// Define ext4_super_block structure
struct ext4_super_block {
    __le32 s_inodes_count;
    __le32 s_blocks_count_lo;
    __le32 s_r_blocks_count_lo;
    __le32 s_free_blocks_count_lo;
    __le32 s_free_inodes_count;
    __le32 s_first_data_block;
    __le32 s_log_block_size;
    __le32 s_log_cluster_size;
    __le32 s_blocks_per_group;
    __le32 s_clusters_per_group;
    __le32 s_inodes_per_group;
    __le32 s_mtime;
    __le32 s_wtime;
    __le16 s_mnt_count;
    __le16 s_max_mnt_count;
    __le16 s_magic;
    __le16 s_state;
    __le16 s_errors;
    __le16 s_minor_rev_level;
    __le32 s_lastcheck;
    __le32 s_checkinterval;
    __le32 s_creator_os;
    __le32 s_rev_level;
    __le16 s_def_resuid;
    __le16 s_def_resgid;
    __le32 s_first_ino;
    __le16 s_inode_size;
    __le16 s_block_group_nr;
    __le32 s_feature_compat;
    __le32 s_feature_incompat;
    __le32 s_feature_ro_compat;
    __u8 s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    __le32 s_algorithm_usage_bitmap;
    __u8 s_prealloc_blocks;
    __u8 s_prealloc_dir_blocks;
    __le16 s_reserved_gdt_blocks;
    __u8 s_journal_uuid[16];
    __le32 s_journal_inum;
    __le32 s_journal_dev;
    __le32 s_last_orphan;
    __le32 s_hash_seed[4];
    __u8 s_def_hash_version;
    __u8 s_jnl_backup_type;
    __le16 s_desc_size;
    __le32 s_default_mount_opts;
    __le32 s_first_meta_bg;
    __le32 s_mkfs_time;
    __le32 s_jnl_blocks[17];
    __le32 s_blocks_count_hi;
    __le32 s_r_blocks_count_hi;
    __le32 s_free_blocks_count_hi;
    __le16 s_min_extra_isize;
    __le16 s_want_extra_isize;
    __le32 s_flags;
    __le16 s_raid_stride;
    __le16 s_mmp_update_interval;
    __le64 s_mmp_block;
    __le32 s_raid_stripe_width;
    __u8 s_log_groups_per_flex;
    __u8 s_checksum_type;
    __u8 s_encryption_level;
    __u8 s_reserved_pad;
    __le64 s_kbytes_written;
    __le32 s_snapshot_inum;
    __le32 s_snapshot_id;
    __le64 s_snapshot_r_blocks_count;
    __le32 s_snapshot_list;
    __le32 s_error_count;
    __le32 s_first_error_time;
    __le32 s_first_error_ino;
    __le64 s_first_error_block;
    __u8 s_first_error_func[32];
    __le32 s_first_error_line;
    __le32 s_last_error_time;
    __le32 s_last_error_ino;
    __le32 s_last_error_line;
    __le64 s_last_error_block;
    __u8 s_last_error_func[32];
    __u8 s_mount_opts[64];
    __le32 s_usr_quota_inum;
    __le32 s_grp_quota_inum;
    __le32 s_overhead_clusters;
    __le32 s_backup_bgs[2];
    __u8 s_encrypt_algos[4];
    __u8 s_encrypt_pw_salt[16];
    __le32 s_lpf_ino;
    __le32 s_prj_quota_inum;
    __le32 s_checksum_seed;
    __u8 s_wtime_hi;
    __u8 s_mtime_hi;
    __u8 s_mkfs_time_hi;
    __u8 s_lastcheck_hi;
    __u8 s_first_error_time_hi;
    __u8 s_last_error_time_hi;
    __u8 s_first_error_errcode;
    __u8 s_last_error_errcode;
    __le16 s_encoding;
    __le16 s_encoding_flags;
    __le32 s_orphan_file_inum;
    __le32 s_reserved[94];
    __le32 s_checksum;
};

// Define ext4_group_desc structure
struct ext4_group_desc {
    __le32 bg_block_bitmap_lo;
    __le32 bg_inode_bitmap_lo;
    __le32 bg_inode_table_lo;
    __le16 bg_free_blocks_count_lo;
    __le16 bg_free_inodes_count_lo;
    __le16 bg_used_dirs_count_lo;
    __le16 bg_flags;
    __le32 bg_exclude_bitmap_lo;
    __le16 bg_block_bitmap_csum_lo;
    __le16 bg_inode_bitmap_csum_lo;
    __le16 bg_itable_unused_lo;
    __le16 bg_checksum;
    __le32 bg_block_bitmap_hi;
    __le32 bg_inode_bitmap_hi;
    __le32 bg_inode_table_hi;
    __le16 bg_free_blocks_count_hi;
    __le16 bg_free_inodes_count_hi;
    __le16 bg_used_dirs_count_hi;
    __le16 bg_itable_unused_hi;
    __le32 bg_exclude_bitmap_hi;
    __le16 bg_block_bitmap_csum_hi;
    __le16 bg_inode_bitmap_csum_hi;
    __u32 bg_reserved;
};

#define EXT4_SUPERBLOCK_OFFSET 1024
#define BLOCK_SIZE 4096

// Function to lock memory
void lock_memory(void *addr, size_t length) {
    if (mlock(addr, length) == -1) {
        perror("mlock failed");
        exit(EXIT_FAILURE);
    }
}

// Function to map a file to memory and lock the EXT4 metadata
void lock_ext4_metadata(const char *fs_path) {
    int fd = open(fs_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open filesystem");
        exit(EXIT_FAILURE);
    }

    // Memory map the superblock
    off_t superblock_offset = 0;
    struct ext4_super_block *superblock = (struct ext4_super_block *)mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, superblock_offset);
    if (superblock == MAP_FAILED) {
        perror("Failed to map superblock");
        close(fd);
        exit(EXIT_FAILURE);
    }

    superblock = (struct ext4_super_block *)((char*)superblock + 1024);

    // Lock superblock memory
    lock_memory(superblock, BLOCK_SIZE);

    printf("s_log_groups_per_flex %d\n", superblock->s_log_groups_per_flex);
    printf("s_s_blocks_per_group  %d\n", superblock->s_blocks_per_group);

    // Calculate the number of block groups
    uint32_t blocks_count = le32toh(superblock->s_blocks_count_lo) + ((uint64_t)le32toh(superblock->s_blocks_count_hi) << 32);
    uint32_t block_group_count = (blocks_count + le32toh(superblock->s_blocks_per_group) - 1) / le32toh(superblock->s_blocks_per_group);

    off_t group_offset = BLOCK_SIZE;

    struct ext4_group_desc *group_descs = (struct ext4_group_desc *)mmap(NULL, BLOCK_SIZE * block_group_count, PROT_READ, MAP_SHARED, fd, BLOCK_SIZE);
    if (group_descs == MAP_FAILED) {
        perror("Failed to map group descriptor");
        return;
    }


    // Lock block and inode bitmaps for all block groups
    for (uint32_t group = 0; group < block_group_count; group++) {
        struct ext4_group_desc *group_desc = group_descs + group;
        // Lock memory for block bitmap
        uint32_t block_bitmap_block = le32toh(group_desc->bg_block_bitmap_lo);
        off_t block_bitmap_offset = (off_t)block_bitmap_block * BLOCK_SIZE;
        void *block_bitmap = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, block_bitmap_offset);
        if (block_bitmap == MAP_FAILED) {
            perror("Failed to map block bitmap");
            continue;
        }
        lock_memory(block_bitmap, BLOCK_SIZE);

        // Lock memory for inode bitmap
        uint32_t inode_bitmap_block = le32toh(group_desc->bg_inode_bitmap_lo);
        off_t inode_bitmap_offset = (off_t)inode_bitmap_block * BLOCK_SIZE;
        void *inode_bitmap = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, inode_bitmap_offset);
        if (inode_bitmap == MAP_FAILED) {
            perror("Failed to map inode bitmap");
            continue;
        }
        lock_memory(inode_bitmap, BLOCK_SIZE);

        printf("handle group %d, block_bitmap %d, inode_bitmap %d\n", group, block_bitmap_block, inode_bitmap_block);
    }
    munmap(group_descs, block_group_count * BLOCK_SIZE);

    // Clean up: unmap the superblock
    munmap(superblock, BLOCK_SIZE);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <EXT4 device or mount point>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Lock the EXT4 filesystem metadata
    lock_ext4_metadata(argv[1]);

    printf("EXT4 metadata locked into memory successfully.\n");

    char input_char;
    cin >> input_char;
    return 0;
}

