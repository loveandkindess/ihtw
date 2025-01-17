# ihtw
**I** **H**ope **T**his **W**orks, the world's fastest data serialization library. Faster than a millisecond in most use cases.

Better than Cap 'n Proto, MessagePack, Avro, and Protobufs. This is like MessagePack with names and endianess handling. Plus zero-copy (at least in C)
This also supports Brainfloat, The Unum format (Posits and Quires). **Sounds too good to be true?** Not without tons of boilerplate, memory corruption.

Now why is it called "I hope this works"? It is so verbose you'll most likely improperly use it, and I hacked it together over a couple weeks. No formal testing.
There was constant memory corruption and hundreds of lines of boilerplate.

## **CHECK OUT ihtw-test.c for some very very performant and verbose code you'll probably never want to see again**
## PLEASE FIX ANY ERRORS AND GIVE FEEDBACK
This is the basics of the format:

```
"IHTW" header a little endian uint32
Format version u8 (1 for now)
Format type u8 (1 for now)
Optional bit order
```
These parameters or must be constantly sent/memorized or agreed upon by the endpoints during design

These are the types
```
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
```

These are the types:
```
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

    IHTW_TYPE_BFLOAT = 5, //brainfloat
    IHTW_BFLOAT_8 = 27,
    IHTW_BFLOAT_16 = 28,
    IHTW_BFLOAT_32 = 29,
    IHTW_BFLOAT_64 = 30,
    IHTW_BFLOAT_128 = 31,
    IHTW_BFLOAT_256 = 32,
    IHTW_BFLOAT_512 = 33,

    IHTW_TYPE_POSIT = 6, // https://posithub.org/about
    IHTW_POSIT_8 = 34,
    IHTW_POSIT_16 = 35,
    IHTW_POSIT_32 = 36,
    IHTW_POSIT_64 = 37,
    IHTW_POSIT_128 = 38,
    IHTW_POSIT_256 = 39,
    IHTW_POSIT_512 = 40,

    IHTW_TYPE_QUIRE = 7, // https://posithub.org/about
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
```

```
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
```

```
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
```

