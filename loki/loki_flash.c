/*
 * loki_flash
 *
 * A sample utility to validate and flash .lok files
 *
 * by Dan Rosenberg (@djrbliss)
 * modified for use in recovery by Seth Shelnutt, adapted by PhilZ
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "loki.h"
#include "../recovery_ui.h"

int loki_flash(const char* partition_label, const char* loki_image)
{
	int ifd, aboot_fd, ofd, recovery, offs, match;
	void *orig, *aboot, *patch;
	struct stat st;
	struct boot_img_hdr *hdr;
	struct loki_hdr *loki_hdr;
	char outfile[1024];

	if (!strcmp(partition_label, "boot")) {
		recovery = 0;
	} else if (!strcmp(partition_label, "recovery")) {
		recovery = 1;
	} else {
if ( language== 1 ) {
		LOGE("[+] First argument must be \"boot\" or \"recovery\".\n");
		printf("[+] First argument must be \"boot\" or \"recovery\".\n");
}else{
		printf("[+] 第一个参数必须为 \"boot\" 或 \"recovery\"。\n");
}
		return 1;
	}

	/* Verify input file */
	aboot_fd = open(ABOOT_PARTITION, O_RDONLY);
	if (aboot_fd < 0) {
if ( language== 1 )
		printf("[-] Failed to open aboot for reading.\n");
else
		printf("[-] 无法打开 aboot 进行读取。\n");

		return 1;
	}

	ifd = open(loki_image, O_RDONLY);
	if (ifd < 0) {
if ( language== 1 )
		printf("[-] Failed to open %s for reading.\n", loki_image);
else
		LOGE("[-] 无法打开 %s 进行读取。\n", loki_image);

		return 1;
	}

	/* Map the image to be flashed */
	if (fstat(ifd, &st)) {
if ( language== 1 )
		printf("[-] fstat() failed.\n");
else
		printf("[-] fstat() 失败。\n");

		return 1;
	}

	orig = mmap(0, (st.st_size + 0x2000 + 0xfff) & ~0xfff, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (orig == MAP_FAILED) {
if ( language== 1 )
		printf("[-] Failed to mmap Loki image.\n");
else
		printf("[-] mmap Loki 镜像失败。\n");

		return 1;
	}

	hdr = orig;
	loki_hdr = orig + 0x400;

	/* Verify this is a Loki image */
	if (memcmp(loki_hdr->magic, "LOKI", 4)) {
if ( language== 1 )
		printf("[-] Input file is not a Loki image.\n");
else
		printf("[-] 输入文件并非一个 Loki 镜像。\n");

		return 1;
	}

	/* Verify this is the right type of image */
	if (loki_hdr->recovery != recovery) {
if ( language== 1 )
		printf("[-] Loki image is not a %s image.\n", recovery ? "recovery" : "boot");
else
		printf("[-] Loki 镜像并非一个 %s 镜像。\n", recovery ? "recovery" : "boot");

		return 1;
	}

	/* Verify the to-be-patched address matches the known code pattern */
	aboot = mmap(0, 0x40000, PROT_READ, MAP_PRIVATE, aboot_fd, 0);
	if (aboot == MAP_FAILED) {
if ( language== 1 )
		printf("[-] Failed to mmap aboot.\n");
else
		printf("[-] mmap aboot 失败。\n");

		return 1;
	}

	match = 0;

	for (offs = 0; offs < 0x10; offs += 0x4) {

		if (hdr->ramdisk_addr < ABOOT_BASE_SAMSUNG)
			patch = hdr->ramdisk_addr - ABOOT_BASE_G2 + aboot + offs;
		else if (hdr->ramdisk_addr < ABOOT_BASE_LG)
			patch = hdr->ramdisk_addr - ABOOT_BASE_SAMSUNG + aboot + offs;
		else
			patch = hdr->ramdisk_addr - ABOOT_BASE_LG + aboot + offs;

		if (patch < aboot || patch > aboot + 0x40000 - 8) {
if ( language== 1 )
			printf("[-] Invalid .lok file.\n");
else
			printf("[-] 无效的 .lok 文件。\n");

			return 1;
		}

		if (!memcmp(patch, PATTERN1, 8) ||
			!memcmp(patch, PATTERN2, 8) ||
			!memcmp(patch, PATTERN3, 8) ||
			!memcmp(patch, PATTERN4, 8) ||
			!memcmp(patch, PATTERN5, 8) ||
			!memcmp(patch, PATTERN6, 8)) {

			match = 1;
			break;
		}
	}

	if (!match) {
if ( language== 1 )
		printf("[-] Loki aboot version does not match device.\n");
else
		printf("[-] Loki aboot 版本与设备不匹配。\n");

		return 1;
	}

if ( language== 1 )
	printf("[+] Loki validation passed, flashing image.\n");
else
	printf("[+] Loki 验证通过，开始刷入镜像。\n");


	snprintf(outfile, sizeof(outfile),
			 "%s",
			 recovery ? RECOVERY_PARTITION : BOOT_PARTITION);

	ofd = open(outfile, O_WRONLY);
	if (ofd < 0) {
if ( language== 1 )
		printf("[-] Failed to open output block device.\n");
else
		printf("[-] 无法打开用于输出的块设备。\n");

		return 1;
	}

	if (write(ofd, orig, st.st_size) != st.st_size) {
if ( language== 1 )
		printf("[-] Failed to write to block device.\n");
else
		printf("[-] 无法打开用于写入的块设备。\n");

		return 1;
	}

if ( language== 1 )
	printf("[+] Loki flashing complete!\n");
else
	printf("[+] Loki 刷入完成！\n");


	close(ifd);
	close(aboot_fd);
	close(ofd);

	return 0;
}
