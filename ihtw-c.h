// IHTW means "I hope this works" (as in it runs properly).
// Previous attempts would fail testing, possibly due to random errors and memory corruption.
#include <stdint.h>

static uint8_t IHTW_LIBRARY_VERSION = 1;
static uint8_t IHTW_FORMAT_VERSION = 1;

static uint32_t IHTW_HEADER = 'IHTW';

#if UINTPTR_MAX == 0xFFFFu
typedef uint16_t IHTW_UINTPTR;
#elif UINTPTR_MAX == 0xFFFFFFFFu
typedef uint32_t IHTW_UINTPTR;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
typedef uint64_t IHTW_UINTPTR;
#endif


union ihtw_bit_order
{
    uint8_t byte;
    struct
    {
        uint8_t first_half : 4;
        uint8_t second_half : 4;
    } bits;
};

static uint8_t ihtw_bit_swap_lookup_table[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };


/*
    The one-byte format is like this:
    tag + (optional) name + (optional with extenions) size tag + data

    The format with the class byte is like this:
    tag + (mandatory) class + (optional) name + (optional depending on tag type and with extensions) size tag + data

    The class is intended to denote a phone number, mailing address, credit card number, etc.
    By the way, tag names are interpreted as byte buffers, so include the null terminator byte in the name buffer size.
*/
enum IHTW_FORMAT_TYPE
{
    IHTW_BIT_FIELD_ONE_BYTE = 0,
    IHTW_NUMBER_RANGE_ONE_BYTE = 1,
    IHTW_WITH_CLASS_BYTE = 2,
};

/*
    Most of these entries are zero-indexed indices into the array of corresponding sizes.
    The extension types are zero-indexed but not in the array of corresponding sizes.
*/
enum IHTW_TYPE
{
    IHTW_NO_NAME = 0,
    IHTW_NO_NAME_BIG_ENDIAN = 0,
    IHTW_NO_NAME_LITTLE_ENDIAN = 63,

    IHTW_HAS_NAME = 127,
    IHTW_HAS_NAME_BIG_ENDIAN = 127,
    IHTW_HAS_NAME_LITTLE_ENDIAN = 191,

    IHTW_TYPE_UINT = 0,
    IHTW_UINT_8 = 0,
    IHTW_UINT_16 = 1,
    IHTW_UINT_32 = 2,
    IHTW_UINT_64 = 3,
    IHTW_UINT_128 = 4,

    IHTW_TYPE_SINT = 1,
    IHTW_SINT_8 = 5,
    IHTW_SINT_16 = 6,
    IHTW_SINT_32 = 7,
    IHTW_SINT_64 = 8,
    IHTW_SINT_128 = 9,

    IHTW_TYPE_ARRAY = 2, // Indicates there is supposed to be XYZ types ahead.
    IHTW_ARRAY_8 = 10,
    IHTW_ARRAY_16 = 11,
    IHTW_ARRAY_32 = 12,
    IHTW_ARRAY_64 = 13,
    IHTW_ARRAY_128 = 14,

    IHTW_TYPE_BIN = 3, // Also you can use this as a string.
    IHTW_BIN_8 = 15,
    IHTW_BIN_16 = 16,
    IHTW_BIN_32 = 17,
    IHTW_BIN_64 = 18,
    IHTW_BIN_128 = 19,

    IHTW_TYPE_FLOAT = 4,
    IHTW_FLOAT_8 = 20,
    IHTW_FLOAT_16 = 21,
    IHTW_FLOAT_32 = 22,
    IHTW_FLOAT_64 = 23,
    IHTW_FLOAT_128 = 24,
    IHTW_FLOAT_256 = 25,
    IHTW_FLOAT_512 = 26,

    IHTW_TYPE_BFLOAT = 5,
    IHTW_BFLOAT_8 = 27,
    IHTW_BFLOAT_16 = 28,
    IHTW_BFLOAT_32 = 29,
    IHTW_BFLOAT_64 = 30,
    IHTW_BFLOAT_128 = 31,
    IHTW_BFLOAT_256 = 32,
    IHTW_BFLOAT_512 = 33,

    IHTW_TYPE_POSIT = 6,
    IHTW_POSIT_8 = 34,
    IHTW_POSIT_16 = 35,
    IHTW_POSIT_32 = 36,
    IHTW_POSIT_64 = 37,
    IHTW_POSIT_128 = 38,
    IHTW_POSIT_256 = 39,
    IHTW_POSIT_512 = 40,

    IHTW_TYPE_QUIRE = 7,
    IHTW_QUIRE_8 = 41,
    IHTW_QUIRE_16 = 42,
    IHTW_QUIRE_32 = 43,
    IHTW_QUIRE_64 = 44,
    IHTW_QUIRE_128 = 45,
    IHTW_QUIRE_256 = 46,
    IHTW_QUIRE_512 = 47,

    IHTW_EXTENSION_TYPES_BEGIN = 48,
    IHTW_EXTENSION_0 = 48,
    IHTW_EXTENSION_1 = 49,
    IHTW_EXTENSION_2 = 50,
    IHTW_EXTENSION_3 = 51,
    IHTW_EXTENSION_4 = 52,
    IHTW_EXTENSION_5 = 53,
    IHTW_EXTENSION_6 = 54,
    IHTW_EXTENSION_7 = 55,
    IHTW_EXTENSION_8 = 56,
    IHTW_EXTENSION_9 = 57,
    IHTW_EXTENSION_10 = 58,
    IHTW_EXTENSION_11 = 59,
    IHTW_EXTENSION_12 = 60,
    IHTW_EXTENSION_13 = 61,
    IHTW_EXTENSION_14 = 62,
    IHTW_EXTENSION_15 = 63,
    IHTW_EXTENSION_TYPES_END = 63
};

enum IHTW_SIZE
{
    IHTW_SIZE_8 = 0,
    IHTW_SIZE_16 = 1,
    IHTW_SIZE_32 = 2,
    IHTW_SIZE_64 = 3,
    IHTW_SIZE_128 = 4,
    IHTW_SIZE_256 = 5,
    IHTW_SIZE_512 = 6
};

static IHTW_UINTPTR IHTW_TYPE_SIZES[48] = {
                                    1, 2, 4, 8, 16, // UINT
                                    1, 2, 4, 8, 16, // SINT
                                    1, 2, 4, 8, 16, // Array
                                    1, 2, 4, 8, 16, // Bin
                                    1, 2, 4, 8, 16, 32, 64, // Float
                                    1, 2, 4, 8, 16, 32, 64, // Bfloat
                                    1, 2, 4, 8, 16, 32, 64, // Posit
                                    1, 2, 4, 8, 16, 32, 64, // Quire                                
};

enum IHTW_ENDIANESS
{
    IHTW_BIG_ENDIAN = 0,
    IHTW_LITTLE_ENDIAN = 1,
    IHTW_UNKNOWN_ENDIAN = 2
};

union ihtw_tag
{
    uint8_t byte;
    struct
    {
        uint8_t type : 3;
        uint8_t size : 3;
        uint8_t has_name : 1;
        uint8_t endianess : 1;
    } bits;
};

struct ihtw_tag_info
{
    uint8_t format_version;
    uint8_t format_type;

    union ihtw_bit_order bit_order;
    uint8_t endianess;

    uint8_t* buffer;

    union ihtw_tag tag;
    uint8_t class_byte;

    uint8_t* name_buffer;
    uint8_t name_size;

    IHTW_UINTPTR size_tag;
    uint8_t* data;

    IHTW_UINTPTR total_size;
};

struct ihtw_match_tag_info
{
    uint8_t format_version;
    uint8_t match_format_version; // If = 2, any is accepted.

    uint8_t format_type;
    uint8_t match_format_type; // If = 2, any is accepted.

    union ihtw_bit_order bit_order;
    uint8_t match_bit_order; // If = 2, any is accepted.
    uint8_t endianess;
    uint8_t match_endianess; // If = 2, any is accepted.

    uint8_t* buffer;

    union ihtw_tag tag;
    uint8_t match_tag_type; // If = 2, any is accepted.
    uint8_t match_tag_size; // If = 2, any is accepted.
    uint8_t match_tag_has_name; // If = 2, any is accepted.
    uint8_t match_tag_endianess; // If = 2, any is accepted.

    uint8_t class_byte;
    uint8_t match_class_byte; // If = 2, any is accepted.

    uint8_t* name_buffer;
    uint8_t match_name_buffer; // If = 2, any is accepted.
    uint8_t name_size;
    uint8_t match_name_size; // If = 2, any is accepted.

    IHTW_UINTPTR size_tag;
    uint8_t match_size_tag; // If = 2, any is accepted.
    uint8_t* data;
    uint8_t match_data; // If = 2, any is accepted.

    IHTW_UINTPTR total_size;
    uint8_t match_total_size; // If = 2, any is accepted.
};

struct ihtw_context
{
    uint8_t format_version;
    uint8_t format_type;

    union ihtw_bit_order bit_order;

    uint8_t* buffer;
    IHTW_UINTPTR index;
    IHTW_UINTPTR size;

    uint8_t max_name_buffer_size;

    IHTW_UINTPTR max_tag_count;
    IHTW_UINTPTR type_counts[8];
};

enum IHTW_ERROR
{
    IHTW_ERROR_SUCCESS = 1,
    IHTW_ERROR_GENERIC_UNKNOWN = 0,
    IHTW_ERROR_RANGE_OUT_OF_BOUNDS = -1,
    IHTW_ERROR_TYPE_NOT_SUPPORTED = -2, // For example, an extension type.
    IHTW_ERROR_TYPE_SIZE_NOT_SUPPORTED = -3,
    IHTW_ERROR_TAG_INFO_MISMATCH = -4,
    IHTW_ERROR_NO_NAME_OR_NAME_TOO_LONG = -5,
    IHTW_ERROR_MAX_TAG_COUNT_REACHED = -6,
    IHTW_ERROR_CANT_CALCULATE_TAG_SIZE = -7,
    IHTW_ERROR_CANT_WRITE_BIN_TAG = -8,
};


uint8_t ihtw_memcmp(void* buffer1, void* buffer2, IHTW_UINTPTR size);

void ihtw_memset(void* buffer, uint8_t byte, IHTW_UINTPTR size);

void ihtw_memcpy(void* from, void* to, IHTW_UINTPTR size);


uint8_t ihtw_get_endianess();

#if defined(_MSC_VER)
#include <intrin.h>
#endif

uint16_t ihtw_bswap16(uint16_t uint16);

uint32_t ihtw_bswap32(uint32_t uint32);

uint64_t ihtw_bswap64(uint64_t uint64);

uint8_t ihtw_handle_bit_order(struct ihtw_context* context, uint8_t byte);


IHTW_UINTPTR ihtw_add_counts(IHTW_UINTPTR* counts, IHTW_UINTPTR how_many_counts);

uint8_t ihtw_range_within_bounds(struct ihtw_context* context, IHTW_UINTPTR length);

uint8_t ihtw_bit_field_tag_to_number_range_tag(union ihtw_tag* tag);

int8_t ihtw_get_tag_union(uint8_t tag_byte, union ihtw_tag* tag);

void* ihtw_memchr(const void* ptr, uint8_t character, IHTW_UINTPTR count);

int8_t ihtw_read_size_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info, uint8_t variable_size, uint8_t endianess);

int8_t ihtw_write_size_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info);

int8_t ihtw_try_read_next_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info);

int8_t ihtw_calculate_tag_size(struct ihtw_tag_info* tag_info);

int8_t ihtw_try_write_next_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info);

int8_t ihtw_compare_tag(struct ihtw_tag_info* tag_info, struct ihtw_match_tag_info* match_tag_info);

/* 
    Set tag_info to 0 to only get the tag count. *tag_count must be zero.
    If the tag limit is zero, it will count as many tags as possible.
    This will stop attempting to read after any error.
*/
int8_t ihtw_list_all_tags(struct ihtw_context* context, struct ihtw_tag_info* tag_info, IHTW_UINTPTR* tag_count, IHTW_UINTPTR tag_limit);

/*
    This will return the matches to a search query.
    You can only compare bin types with bin types.
    Set results to 0 to get the number of results.
*/
int8_t ihtw_search(struct ihtw_context* context, struct ihtw_match_tag_info* search_query, struct ihtw_tag_info* tag_info,
    IHTW_UINTPTR range_start, IHTW_UINTPTR range_end, IHTW_UINTPTR* results, IHTW_UINTPTR result_count);

void ihtw_reset_context(struct ihtw_context* context, uint8_t format_version, uint8_t format_type, uint8_t bit_order, IHTW_UINTPTR buffer, IHTW_UINTPTR size, uint8_t max_name_buffer_size,
    IHTW_UINTPTR max_tag_count);

// This will can add the 'IHTW' header, format version and type, and bit order. Set bit_order to 0 to not add the bit order.
int8_t ihtw_add_headers(struct ihtw_context* context, uint8_t add_header, uint8_t format_version, uint8_t format_type, uint8_t bit_order);

/*
    This can parse the heaer, format version, format type, and bit order. If this fails
    , it will return a bounds (out of range) error, or a generic error if the format version is different (or if the 'IHTW' header isn't found)
*/
int8_t ihtw_parse_headers(struct ihtw_context* context, uint8_t expect_header, uint8_t expect_bit_order);