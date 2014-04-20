/*********************************************************
 * Copyright (C) 2006 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 *********************************************************/

/*
 * link.c --
 *
 * Symlink-specific inode operations for the filesystem portion of the
 * vmhgfs driver.
 */

/* Must come before any kernel header file. */
#include "driver-config.h"

#include "compat_fs.h"
#include "compat_namei.h"

#include "module.h"
#include "hgfsProto.h"
#include "fsutil.h"
#include "vm_assert.h"

/* HGFS symlink operations. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
static void *HgfsFollowlink(struct dentry *dentry,
                            struct nameidata *nd);
#else
static int HgfsFollowlink(struct dentry *dentry,
                          struct nameidata *nd);
#endif
static int HgfsReadlink(struct dentry *dentry,
                        char __user *buffer,
                        int buflen);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
static void HgfsPutlink(struct dentry *dentry,
                        struct nameidata *nd,
                        void *cookie);
#else
static void HgfsPutlink(struct dentry *dentry,
                        struct nameidata *nd);
#endif

/* HGFS inode operations structure for symlinks. */
struct inode_operations HgfsLinkInodeOperations = {
   .follow_link   = HgfsFollowlink,
   .readlink      = HgfsReadlink,
   .put_link      = HgfsPutlink,
};

/*
 * HGFS symlink operations.
 */

/*
 *----------------------------------------------------------------------
 *
 * HgfsFollowlink --
 *
 *    Modeled after nfs_follow_link from a 2.4 kernel so it'll work
 *    across all kernel revisions we care about.
 *
 * Results:
 *    Returns zero on success, negative error on failure.
 *
 *    On new kernels: The error is returned as void *.
 *    On older kernels: The error is returned as is.
 *
 * Side effects:
 *    None
 *
 *----------------------------------------------------------------------
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
static void *
HgfsFollowlink(struct dentry *dentry, // IN: Dentry containing link
               struct nameidata *nd)  // OUT: Contains target dentry
#else
static int
HgfsFollowlink(struct dentry *dentry, // IN: Dentry containing link
               struct nameidata *nd)  // OUT: Contains target dentry
#endif
{
   HgfsAttrInfo attr;
   char *fileName = NULL;
   int error;

   ASSERT(dentry);
   ASSERT(nd);

   if (!dentry) {
      LOG(4, (KERN_DEBUG "VMware hgfs: HgfsFollowlink: null input\n"));
      error = -EINVAL;
      goto out;
   }

   LOG(6, (KERN_DEBUG "VMware hgfs: HgfsFollowlink: calling "
           "HgfsPrivateGetattr\n"));
   error = HgfsPrivateGetattr(dentry, &attr, &fileName);
   if (!error) {

      /* Let's make sure we got called on a symlink. */
      if (attr.type != HGFS_FILE_TYPE_SYMLINK || fileName == NULL) {
         LOG(6, (KERN_DEBUG "VMware hgfs: HgfsFollowlink: got called "
                 "on something that wasn't a symlink\n"));
         error = -EINVAL;
         kfree(fileName);
      } else {
         LOG(6, (KERN_DEBUG "VMware hgfs: %s: calling nd_set_link %s\n",
                 __func__, fileName));
         nd_set_link(nd, fileName);
      }
   }
  out:

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
   return ERR_PTR(error);
#else
   return error;
#endif
}


/*
 *----------------------------------------------------------------------
 *
 * HgfsReadlink --
 *
 *    Modeled after nfs_read_link from a 2.4 kernel so it'll work
 *    across all kernel revisions we care about.
 *
 * Results:
 *    Returns zero on success, negative error on failure.
 *
 * Side effects:
 *    None
 *
 *----------------------------------------------------------------------
 */

static int
HgfsReadlink(struct dentry *dentry,  // IN:  Dentry containing link
             char __user *buffer,    // OUT: User buffer to copy link into
             int buflen)             // IN:  Length of user buffer

{
   HgfsAttrInfo attr;
   char *fileName = NULL;
   int error;

   ASSERT(dentry);
   ASSERT(buffer);

   if (!dentry) {
      LOG(4, (KERN_DEBUG "VMware hgfs: HgfsReadlink: null input\n"));
      return -EINVAL;
   }

   LOG(6, (KERN_DEBUG "VMware hgfs: HgfsReadlink: calling "
           "HgfsPrivateGetattr\n"));
   error = HgfsPrivateGetattr(dentry, &attr, &fileName);
   if (!error) {

      /* Let's make sure we got called on a symlink. */
      if (attr.type != HGFS_FILE_TYPE_SYMLINK || fileName == NULL) {
         LOG(6, (KERN_DEBUG "VMware hgfs: HgfsReadlink: got called "
                 "on something that wasn't a symlink\n"));
         error = -EINVAL;
      } else {
         LOG(6, (KERN_DEBUG "VMware hgfs: HgfsReadlink: calling "
                 "vfs_readlink\n"));
         error = vfs_readlink(dentry, buffer, buflen, fileName);
      }
      kfree(fileName);
   }
   return error;
}


/*
 *----------------------------------------------------------------------
 *
 * HgfsPutlink --
 *
 *    Modeled after page_put_link from a 2.6.9 kernel so it'll work
 *    across all kernel revisions we care about.
 *
 * Results:
 *    None
 *
 * Side effects:
 *    None
 *
 *----------------------------------------------------------------------
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
static void
HgfsPutlink(struct dentry *dentry, // dentry
            struct nameidata *nd,  // lookup name information
            void *cookie)          // cookie
#else
static void
HgfsPutlink(struct dentry *dentry, // dentry
            struct nameidata *nd)  // lookup name information
#endif
{
   char *fileName = NULL;

   LOG(6, (KERN_DEBUG "VMware hgfs: %s: put for %s\n",
           __func__, dentry->d_name.name));

   fileName = nd_get_link(nd);
   if (!IS_ERR(fileName)) {
      LOG(6, (KERN_DEBUG "VMware hgfs: %s: putting %s\n",
              __func__, fileName));
      kfree(fileName);
      nd_set_link(nd, NULL);
   }
}
