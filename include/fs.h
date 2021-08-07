/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_H_
#define _FS_H_

#include <types.h>
#include <bits.h>
#include <state.h>
#include <list.h>
#include <time.h>

typedef enum fmode {
    FMODE_RDONLY    = BIT(0),
    FMODE_WRONLY    = BIT(1),
    FMODE_RW        = FMODE_RDONLY | FMODE_WRONLY,

    FMODE_LSEEK     = BIT(3),
    FMODE_PREAD     = BIT(4),
    FMODE_EXEC      = BIT(5),
} fmode_t;

struct file;
struct inode;
struct super_block;

// struct file_ops {
//     state (*read)(struct file *fp, void *dest, size_t len, loff_t *);
//     state (*write)(struct file *fp, const void *src, size_t len, loff_t *);
//     state (*mmap)(struct file *fp);

//     state (*open) (struct inode *, struct file *);
//     state (*release) (struct inode *, struct file *);
//     state (*flush) (struct file *, fl_owner_t id);
// };

// struct inode_ops {
//     struct dentry *(*lookup)(struct inode *,struct dentry *, unsigned int);
//     state (*create)(struct user_namespace *, struct inode *,struct dentry *, umode_t, bool);

//     state (*readlink) (struct dentry *, char *, int);
//     state (*link)(struct dentry *,struct inode *,struct dentry *);
//     state (*unlink)(struct inode *,struct dentry *);
//     state (*symlink)(struct user_namespace *, struct inode *,struct dentry *, const char *);

//     state (*mkdir) (struct user_namespace *, struct inode *,struct dentry *, umode_t);
//     state (*rmdir) (struct inode *,struct dentry *);
// };

struct sb_ops {
    void (*sync)(struct super_block *);
};

struct file {
    enum fmode fmode;
    struct file_ops *ops;
};

struct inode {
    umode_t mode;
    uid_t   uid;
    gid_t   gid;

    struct timespec atime;  /* access time */
    struct timespec mtime;  /* modify time */
    struct timespec ctime;  /* change time */
};

struct super_block {
    struct sb_ops *ops;
};

struct fs_type {
    char *name;
    list_t  list;
};

state kernel_execve(const char *file, const char *const *argv, const char *const *envp);
state filesystem_register(struct fs_type *fs);

#endif  /* _FS_H_ */
