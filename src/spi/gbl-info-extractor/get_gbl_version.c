/**
 * \file get_gbl_version.c
 * \brief Extract the firmware version from the supplied GBL (Ember/Silabs) firmware image and dumps it to the console
 *
 * This file is licensed under the terms of BSD-3-clause license (see the LICENSE file)
**/

#include <stdio.h>
#include <unistd.h>	// For sync() etc...
#include <inttypes.h>	// For PRIu32
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef UNIT_TEST
#define DEBUG 4	/*!< In unit test mode, set debug level to maximum verbosity */
#endif
#ifndef DEBUG
#define DEBUG 1	/*!< Debug level (from 1 to 4, 4 being the most verbose) */
#endif

#ifdef DEBUG
#include <assert.h>	// For assert()
#else
#define assert(x) do { if (!(x)) return NULL; } while(0);      /*!< Empty replacement for the standard assert() macro */
#endif

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>	/* Only available in C99 */
#else
/**
 * Booleans
**/
typedef enum {
	false,
	true
} bool;
#endif

/* Return values */
#define EXIT_FAILURE	5	/*!< This means there was a failure */
#define EXIT_SUCCESS	0	/*!< This means the program succeeded */

#ifdef UNIT_TEST
FILE *unit_test_hooked_stdout;	/* Stream where stdout is redirected in unit test mode */
#define pstdout unit_test_hooked_stdout
/* Redirect OS calls to unit-test hook functions customized per-caller function */
#define hooked_open(a,b) unit_test_hook_open(__func__,a,b)
#define hooked_close(a) unit_test_hook_close(__func__,a)
#define hooked_read(a,b,c) unit_test_hook_read(__func__,a,b,c)
#define hooked_write(a,b,c) unit_test_hook_write(__func__,a,b,c)
#define hooked_ioctl(a,b,c) unit_test_hook_ioctl(__func__,a,b,c)
#define hooked_lseek(a,b,c) unit_test_hook_lseek(__func__,a,b,c)
#define hooked_fstat(a,b) unit_test_hook_fstat(__func__,a,b)
#else
#define pstdout stdout	/*!< Redirection of the output FILE* (will point to stdout in non UNIT_TEST mode) */
/* Use real OS calls for hooked functions in non-unit-test mode */
#define hooked_open open	/*!< Wrapper around the open() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_close close	/*!< Wrapper around the close() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_read read	/*!< Wrapper around the read() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_write write	/*!< Wrapper around the write() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_ioctl ioctl	/*!< Wrapper around the ioctl() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_lseek lseek	/*!< Wrapper around the lseek() system call, redirected to a hook if UNIT_TEST directive is defined */
#define hooked_fstat fstat	/*!< Wrapper around the fstat() system call, redirected to a hook if UNIT_TEST directive is defined */
#endif

static char *progname; /*!< The command name under which we were called (for logging) */

typedef uint32_t gbl_tag_id_t;

struct gbl_tl_tag {
	gbl_tag_id_t gbl_tag_id;	/*!< Tag ID for this section */
	uint32_t gbl_tag_length;	/*!< Length of this section */
	/* Following this entry, we will find the tag payload itself, starting with the first byte, for a length of gbl_tag_length */
};

typedef struct ApplicationData {
	uint32_t type;	/*!< Bitfield representing type of application */
	uint32_t version;	/*!< Version value for this application */
	uint32_t capabilities;	/*!< Capabilities of this application */
	uint8_t productId[16];	/*!< Unique ID (UUID or GUID) of the product that this application was built for */
} ApplicationData_t;

#define EBL_TAG_ID_APPLICATION 0xF40A0AF4U

/* debugX() is used for compatibility with the equivalent macros in U-Boot code */
#ifndef debugX
#ifdef DEBUG
#	include <stdio.h>
#	define debugX(level,fmt,args...) if (DEBUG>=level) fprintf(stderr, fmt, ## args);	/*!< Debugging macro (based on the current debug level in #DEBUG */
#else
#	define debugX(level,fmt,args...)	/*!< Debugging macro (based on the current debug level in #DEBUG */
#endif /* DEBUG */
#endif

/**
 * \brief Dump a buffer as hex to a stream
 * \param fd The file descriptor on which to write
 * \param buffer The buffer to dump
 * \param len The length of \p buffer
**/
void hexdump_buffer(FILE *fd, const unsigned char *buffer, size_t len) {
	size_t i;
	unsigned char nibble;
	
	for (i=0; i<len; i++) {
		if (i!=0) fputc(' ', fd);	/* Add separator but on first byte */
		
		nibble = buffer[i] >> 4;
		if (nibble<=9)
			nibble += '0';
		else
			nibble += 'a' - 10;
		fputc(nibble, fd);	/* Dump high order nibble as ascii char */
		
		nibble = buffer[i] & 0xf;
		if (nibble<=9)
			nibble += '0';
		else
			nibble += 'a' - 10;
		fputc(nibble, fd);	/* Dump high order nibble as ascii char */
	}
	fputc('\n', fd);
}

/**
 * \brief Convert a 32-bit integer from little-endian to host order
 *
 * \param le_u32 A 32-bit integer read from a 4-byte buffer containing a little endian value
 * \return The resulting 32-bit value
 */
uint32_t letohl(uint32_t le_u32) {
	uint8_t* bp;
	uint32_t result;
	
	bp = (uint8_t*)(&le_u32) + 3;	/* Point to last byte */
	
	result = (uint32_t)(*bp--);
	result <<= 8;
	result |= *bp--;
	result <<= 8;
	result |= *bp--;
	result <<= 8;
	result |= *bp;
	
	return result;
}

/**
 * \brief Convert a 16-bit integer from little-endian to host order
 *
 * \param le_u16 A 16-bit integer read from a 2-byte buffer containing a little endian value
 * \return The resulting 16-bit value
 */
uint16_t letohs(uint16_t le_u16) {
	uint8_t* bp;
	uint16_t result;
	
	bp = (uint8_t*)(&le_u16) + 1;	/* Point to last byte */
	
	result = (uint16_t)(*bp--);
	result <<= 8;
	result |= *bp;
	
	return result;
}

char* gbl_tag_id_to_str(gbl_tag_id_t tag_id) {
	switch (tag_id) {
		case 0xE35050E3UL:
			return "EBL_IMAGE_MAGIC_WORD";
		case 0x03000000UL:
			return "EBL_COMPATIBILITY_MAJOR_VERSION";
		case 0x00000200UL:
			return "EBLV2_COMPATIBILITY_MAJOR_VERSION";
		case 0x03A617EBUL:
			return "EBL_TAG_ID_HEADER_V3";
		case 0xF50909F5UL:
			return "EBL_TAG_ID_BOOTLOADER";
		case EBL_TAG_ID_APPLICATION:
			return "EBL_TAG_ID_APPLICATION";
		case 0xF60808F6UL:
			return "EBL_TAG_ID_METADATA";
		case 0xFE0101FEUL:
			return "EBL_TAG_ID_PROG";
		case 0xFD0303FDUL:
			return "EBL_TAG_ID_ERASEPROG";
		case 0xFC0404FCUL:
			return "EBL_TAG_ID_END";
		case 0x5EA617EBUL:
			return "EBL_TAG_ID_SE_UPGRADE";
		case 0xFB0505FBUL:
			return "EBL_TAG_ID_ENC_HEADER";
		case 0xFA0606FAUL:
			return "EBL_TAG_ID_ENC_INIT";
		case 0xF90707F9UL:
			return "EBL_TAG_ID_ENC_EBL_DATA";
		case 0xF70909F7UL:
			return "EBL_TAG_ID_ENC_MAC";
		case 0xF70A0AF7UL:
			return "EBL_TAG_ID_SIGNATURE_ECDSA_P256";
		case 0xF30B0BF3UL:
			return "EBL_TAG_ID_CERTIFICATE_ECDSA_P256";
		case 0x00000000UL:
			return "EBL_TYPE_NONE";
		case 0x00000001UL:
			return "EBL_TYPE_ENCRYPTION_AESCCM";
		case 0x00000100UL:
			return "EBL_TYPE_SIGNATURE_ECDSA";
		default:
			return "(unknown)";
	}
}

/**
 * \brief Main routine
**/
int 
#ifndef UNIT_TEST
    main
#else
    run_main
#endif
            (int argc, char *argv[]) {

	int fw_fd;	/* The firmware image file descriptor (to read) */
	int opt;	/* For getopt() */
	off_t offset;	/* Current offset as returned by lseek() */
	
	static const char  *fw_image_filename;	/* Pointer to a firmware image filename (provided as arg) */
	unsigned long long fw_image_file_sz;
	struct gbl_tl_tag  gbl_tag;	/* A memory area where we read the content of GBL tags */
	ApplicationData_t  gbl_app_info;	/* A memory area where we store the GBL application data */
	struct stat        filestat;	/* To read a file size on disk */

	bool debug = false;
	bool done = false;	/* Will be set to true when processing is done and we can exit */
	
	uint8_t gbl_info_tag_buf[64];	/* A buffer to hold the payload of a GBL info tag */
	
	progname = argv[0];
	
#ifdef UNIT_TEST
	fprintf(stderr, "%s: Running in unit test mode\n", progname);
#endif
	while ((opt = getopt(argc, argv, "hd"
	                    )) > 0) {
		switch (opt) {
		case 'h':
			printf("%s: Extract information from a GBL firmware image file\n", progname);
			printf("Usage %s [-h] [-d] gbl_file\n", progname);
			return EXIT_SUCCESS;
			break;
		case 'd':
			debug = true;
			break;
		}
	}

	/* Make sure there is at least one non-optional argument: this is the firmware image filename */
	if (optind == argc) {
		fprintf(stderr, "%s: no GBL firmware image filename specified\n", progname);
		return EXIT_FAILURE;
	}
	else {
		fw_image_filename = argv[optind];
		if (++optind != argc)
			fprintf(stderr, "%s: Warning extra argument(s) provided... ignoring\n", progname);
	}
	
	/* Open the image file and check that it is valid */
	if ((fw_fd = hooked_open(fw_image_filename, O_SYNC | O_RDONLY)) < 0) {
		fprintf(stderr, "%s: %s: %s\n", progname, fw_image_filename, strerror(errno));
		return EXIT_FAILURE;
	}

	if (hooked_fstat(fw_fd, &filestat) < 0) {
		fprintf(stderr,"%s: %s: %s\n", progname, fw_image_filename, strerror(errno));
		return EXIT_FAILURE;
	}
	else {
		fw_image_file_sz = (unsigned long long)(filestat.st_size);
		debugX(2,"Firmware image size on disk is %llu\n", fw_image_file_sz);
	}

	memset(&gbl_tag, 0, sizeof(gbl_tag));

	//~ /* Move to the end of the file... */
	//~ if (lseek(fd, -sizeof(image_footer_t), SEEK_END) < 0) {
		//~ fprintf(stderr, "Error while seeking to start of image footer\n");
		//~ return -1;
	//~ }

	offset = 0;
	while (!done) {
		/* ... and read the tag content */
		debugX(2,"Reading at offset %lu/%lu\n", (unsigned long)offset, (unsigned long)fw_image_file_sz);
		if (hooked_read(fw_fd, &gbl_tag, sizeof(gbl_tag)) != sizeof(gbl_tag)) {
			fprintf(stderr, "Failed reading next GBL tag from firmware image\n");
			return EXIT_FAILURE;
		}

		if (DEBUG>=4) {
			fprintf(stderr, "Dumping first GBL tag:\n");
			hexdump_buffer(stderr, (unsigned char *)(&gbl_tag), sizeof(gbl_tag));
		}
		
		/* Make sure we read gbl_tag_id as little endian (convention for ember GBL files) */
		uint32_t gbl_tag_id = letohl(gbl_tag.gbl_tag_id);
		uint32_t gbl_tag_length = letohl(gbl_tag.gbl_tag_length);
		
		debugX(2,"Read tag 0x%"PRIx32" (%s)\n", gbl_tag_id, gbl_tag_id_to_str(gbl_tag_id));
		switch(gbl_tag_id) {
			case EBL_TAG_ID_APPLICATION:
				debugX(2,"Found GBL info tag of length %u\n", gbl_tag_length);
				if (gbl_tag_length>64) {
					fprintf(stderr, "Error GBL info tag is too large: %u\n", gbl_tag_length);
					return EXIT_FAILURE;
				}
				else {
					memset(&gbl_info_tag_buf, 0, sizeof(gbl_info_tag_buf));
					if (hooked_read(fw_fd, &gbl_info_tag_buf, gbl_tag_length) != gbl_tag_length) {
						fprintf(stderr, "Failed reading GBL info tag payload from firmware image\n");
						return EXIT_FAILURE;
					}
					if (DEBUG>=4) {
						fprintf(stderr, "Dumping GBL info tag payload:\n");
						hexdump_buffer(stderr, (unsigned char *)(&gbl_info_tag_buf), gbl_tag_length);
					}
					if (gbl_tag_length<sizeof(gbl_app_info)) {
						fprintf(stderr, "GBL application data (%u bytes) in firmware image is too short (min %lu bytes expected)\n", gbl_tag_length, sizeof(gbl_app_info));
						return EXIT_FAILURE;
					}
					if (gbl_tag_length>sizeof(gbl_app_info)) {
						fprintf(stderr, "GBL application data (%u bytes) in firmware image is larger than expected (%lu bytes). Truncating\n", gbl_tag_length, sizeof(gbl_app_info));
					}
					memcpy(&gbl_app_info, &gbl_info_tag_buf, sizeof(gbl_app_info));
					printf(/*"GBL version: */"0x%08"PRIx32"\n", letohl(gbl_app_info.version));
					
					done = true;
				}
				break;
			default:
				offset = hooked_lseek(fw_fd, (off_t)(gbl_tag_length), SEEK_CUR);
				if (offset<0) {
					fprintf(stderr, "Error %d while seeking to end of TAG %u: %s\n", errno, gbl_tag_id, strerror(errno));
					return EXIT_FAILURE;
				}
		}
	}
	return EXIT_SUCCESS;
}
