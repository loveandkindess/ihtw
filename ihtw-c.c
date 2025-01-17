#include "ihtw-c.h"

uint8_t ihtw_memcmp(void* buffer1, void* buffer2, IHTW_UINTPTR size)
{
    for (IHTW_UINTPTR i = 0; i < size; i++)
    {
        if (*(uint8_t*)((IHTW_UINTPTR)buffer1 + i) != *(uint8_t*)((IHTW_UINTPTR)buffer2 + i))
        {
            return 0;
        }
    }

    return 1;
}

void ihtw_memset(void* buffer, uint8_t byte, IHTW_UINTPTR size)
{
    for (IHTW_UINTPTR i = 0; i < size; i++)
    {
        *(uint8_t*)((IHTW_UINTPTR)buffer + i) = byte;
    }
}

void ihtw_memcpy(void* from, void* to, IHTW_UINTPTR size)
{
    for (IHTW_UINTPTR i = 0; i < size; i++)
    {
        *(uint8_t*)((IHTW_UINTPTR)to + i) = *(uint8_t*)((IHTW_UINTPTR)from + i);
    }
}


uint8_t ihtw_get_endianess()
{
    volatile uint32_t i = 0x01234567;
    // return 0 for big endian, 1 for little endian.
    return (*((uint8_t*)(&i))) == 0x67;
}

#if UINTPTR_MAX >= 0xFFFFu
uint16_t ihtw_bswap16(uint16_t uint16)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap16(uint16);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap16(uint16);
#elif defined(_MSC_VER)
    return _byteswap_ushort(uint16);
#endif
}
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
uint32_t ihtw_bswap32(uint32_t uint32)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap32(uint32);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap32(uint32);
#elif defined(_MSC_VER)
    return _byteswap_ulong(uint32);
#endif
}
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
uint64_t ihtw_bswap64(uint64_t uint64)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap64(uint64);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap64(uint64);
#elif defined(_MSC_VER)
    return _byteswap_uint64(uint64);
#endif
}
#endif

uint8_t ihtw_handle_bit_order(struct ihtw_context* context, uint8_t byte)
{
    union ihtw_bit_order local;
    local.bits.first_half = 1;
    local.bits.second_half = 2;

    if (local.byte != context->bit_order.byte)
    {
        return (ihtw_bit_swap_lookup_table[byte & 0b1111] << 4) | ihtw_bit_swap_lookup_table[byte >> 4];
    }
    return byte;
}


IHTW_UINTPTR ihtw_add_counts(IHTW_UINTPTR* counts, IHTW_UINTPTR how_many_counts)
{
    IHTW_UINTPTR result = 0;

    for (IHTW_UINTPTR i = 0; i < how_many_counts; i++)
    {
        result += counts[i];
    }

    return result;
}

uint8_t ihtw_range_within_bounds(struct ihtw_context* context, IHTW_UINTPTR length)
{
    if ((context->buffer + context->index + length)
    > (context->buffer + context->size))
    {
        return 0;
    }

    return 1;
}

uint8_t ihtw_bit_field_tag_to_number_range_tag(union ihtw_tag* tag)
{
    uint8_t result = 0;

    if (tag->bits.has_name)
    {
        result += IHTW_HAS_NAME;
    }

    if (tag->bits.endianess)
    {
        result += 63;
    }

    switch (tag->bits.type)
    {
    case IHTW_TYPE_UINT:
        result += 0;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_SINT:
        result += 5;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_ARRAY:
        result += 10;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_BIN:
        result += 15;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_FLOAT:
        result += 20;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_BFLOAT:
        result += 27;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_POSIT:
        result += 34;
        result += tag->bits.size;
        break;
    case IHTW_TYPE_QUIRE:
        result += 41;
        result += tag->bits.size;
        break;
    }

    return result;
}

int8_t ihtw_get_tag_union(uint8_t tag_byte, union ihtw_tag* tag)
{
    uint8_t result = 0;

    if (tag_byte > IHTW_HAS_NAME)
    {
        tag->bits.has_name = 1;
        result += 128;
    }

    if (tag->bits.endianess)
    {
        result += 63;
    }

    switch (tag->bits.type)
    {
    case IHTW_TYPE_UINT:
        tag->bits.type = IHTW_TYPE_UINT;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_SINT:
        tag->bits.type = IHTW_TYPE_SINT;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_ARRAY:
        tag->bits.type = IHTW_TYPE_ARRAY;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_BIN:
        tag->bits.type = IHTW_TYPE_BIN;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_FLOAT:
        tag->bits.type = IHTW_TYPE_FLOAT;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_BFLOAT:
        tag->bits.type = IHTW_TYPE_BFLOAT;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_POSIT:
        tag->bits.type = IHTW_TYPE_POSIT;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    case IHTW_TYPE_QUIRE:
        tag->bits.type = IHTW_TYPE_QUIRE;
        tag->bits.size = (tag_byte - result) / 8;
        break;
    default:
        return IHTW_ERROR_TYPE_NOT_SUPPORTED;
    }

    return IHTW_ERROR_SUCCESS;
}

void* ihtw_memchr(const void* ptr, uint8_t character, IHTW_UINTPTR count)
{
    if (count != 0) {
        const unsigned char* p = ptr;

        do {
            if (*p++ == character)
                return ((void*)(p));
        } while (--count != 0);
    }
    return 0;
}

// attempt to Read size tag + buffer
int8_t ihtw_read_size_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info, uint8_t variable_size, uint8_t endianess)
{
    if (!ihtw_range_within_bounds(context, IHTW_TYPE_SIZES[variable_size]))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    /*
       I just added this to be safe, I don't know how it could be reached
       without adding some sort of extension handling.
    */
    if (endianess == IHTW_UNKNOWN_ENDIAN)
    {
        return IHTW_ERROR_GENERIC_UNKNOWN;
    }

    switch (IHTW_TYPE_SIZES[variable_size])
    {
    case IHTW_SIZE_8:
        tag_info->size_tag = (IHTW_UINTPTR) * (uint8_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 1);

        if (!ihtw_range_within_bounds(context, IHTW_TYPE_SIZES[variable_size] + tag_info->size_tag))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        return IHTW_ERROR_SUCCESS;
        break;

#if UINTPTR_MAX >= 0xFFFFu
    case IHTW_SIZE_16:
        tag_info->size_tag = (IHTW_UINTPTR) * (uint16_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 2);

        if ((endianess != IHTW_UNKNOWN_ENDIAN) && (ihtw_get_endianess() != endianess))
        {
            tag_info->size_tag = (IHTW_UINTPTR) * (uint16_t*)ihtw_bswap16(*(uint16_t*)tag_info->size_tag);
        }

        if (!ihtw_range_within_bounds(context, IHTW_TYPE_SIZES[variable_size] + tag_info->size_tag))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        return IHTW_ERROR_SUCCESS;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
    case IHTW_SIZE_32:
        tag_info->size_tag = (IHTW_UINTPTR) * (uint32_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 4);

        if ((endianess != IHTW_UNKNOWN_ENDIAN) && (ihtw_get_endianess() != endianess))
        {
            tag_info->size_tag = (IHTW_UINTPTR) * (uint32_t*)ihtw_bswap32(*(uint32_t*)tag_info->size_tag);
        }

        if (!ihtw_range_within_bounds(context, IHTW_TYPE_SIZES[variable_size] + tag_info->size_tag))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        return IHTW_ERROR_SUCCESS;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
    case IHTW_SIZE_64:
        tag_info->size_tag = (IHTW_UINTPTR) * (uint64_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 8);

        if ((endianess != IHTW_UNKNOWN_ENDIAN) && (ihtw_get_endianess() != endianess))
        {
            tag_info->size_tag = (IHTW_UINTPTR) * (uint64_t*)ihtw_bswap64(*(uint64_t*)tag_info->size_tag);
        }

        if (!ihtw_range_within_bounds(context, IHTW_TYPE_SIZES[variable_size] + tag_info->size_tag))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        return IHTW_ERROR_SUCCESS;
        break;
#endif

    default:
        return IHTW_ERROR_TYPE_SIZE_NOT_SUPPORTED;
        break;
    }
}

// attempt to write size tag + buffer
int8_t ihtw_write_size_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info)
{
    switch (tag_info->tag.bits.size)
    {
    case IHTW_SIZE_8:
        *(uint8_t*)(context->buffer + context->index) = (uint8_t)tag_info->size_tag;
        context->index += 1;
        ihtw_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 1), tag_info->size_tag);
        context->index += tag_info->size_tag;
        break;

#if UINTPTR_MAX >= 0xFFFFu
    case IHTW_SIZE_16:
        if ((ihtw_get_endianess() != tag_info->endianess))
        {
            *(uint16_t*)(context->buffer + context->index) = ihtw_bswap16(tag_info->size_tag);
        }

        context->index += 2;
        ihtw_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 2), tag_info->size_tag);
        context->index += tag_info->size_tag;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
    case IHTW_SIZE_32:
        if ((ihtw_get_endianess() != tag_info->endianess))
        {
            *(uint32_t*)(context->buffer + context->index) = ihtw_bswap32(tag_info->size_tag);
        }

        context->index += 4;
        ihtw_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 4), tag_info->size_tag);
        context->index += tag_info->size_tag;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
    case IHTW_SIZE_64:
        if ((ihtw_get_endianess() != tag_info->endianess))
        {
            *(uint64_t*)(context->buffer + context->index) = ihtw_bswap64(tag_info->size_tag);
        }

        context->index += 8;
        ihtw_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 8), tag_info->size_tag);
        context->index += tag_info->size_tag;
        break;
#endif

    default:
        return IHTW_ERROR_TYPE_SIZE_NOT_SUPPORTED;
        break;
    }
}

int8_t ihtw_try_read_next_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info)
{
    if (!ihtw_range_within_bounds(context, 1))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    IHTW_UINTPTR start = (context->buffer + context->index);

    IHTW_UINTPTR size = IHTW_TYPE_SIZES[tag_info->tag.bits.size];

    tag_info->format_version = IHTW_FORMAT_VERSION;

    switch (context->format_type)
    {
    case IHTW_BIT_FIELD_ONE_BYTE:
        tag_info->format_type = IHTW_BIT_FIELD_ONE_BYTE;

        tag_info->tag.byte = ihtw_handle_bit_order(context, 
            *(uint8_t*)(context->buffer + context->index));

        context->index++;

        break;
    case IHTW_NUMBER_RANGE_ONE_BYTE:
        tag_info->format_type = IHTW_NUMBER_RANGE_ONE_BYTE;

        if (ihtw_get_tag_union(*(uint8_t*)(context->buffer + context->index)
            , &tag_info->bit_order) != IHTW_ERROR_SUCCESS)
        {
            return IHTW_ERROR_TYPE_NOT_SUPPORTED;
        }

        context->index++;

        break;
    case IHTW_WITH_CLASS_BYTE:
        tag_info->format_type = IHTW_WITH_CLASS_BYTE;

        tag_info->tag.byte = ihtw_handle_bit_order(context,
            *(uint8_t*)(context->buffer + context->index));

        context->index++;

        tag_info->class_byte = *(uint8_t*)(context->buffer + context->index);

        context->index++;
        break;
    }

    if (tag_info->tag.bits.has_name)
    {
        
        IHTW_UINTPTR name_size = ihtw_memchr((context->buffer + context->index), '\0',
            (context->max_name_buffer_size > ((context->buffer + context->size) - (context->buffer + context->index))) 
            ? context->max_tag_count : (context->buffer + context->size) - (context->buffer + context->index));

        if (name_size != NULL)
        {
            tag_info->name_buffer = (uint8_t*)(context->buffer + context->index);
            tag_info->name_size = name_size;
            context->index += (IHTW_UINTPTR)name_size;
        }
        else
        {
            return IHTW_ERROR_NO_NAME_OR_NAME_TOO_LONG;
        }
    }

    if (tag_info->tag.bits.type == IHTW_TYPE_BIN)
    {
        IHTW_UINTPTR size_tag_size = IHTW_TYPE_SIZES[tag_info->tag.bits.size];
        if (!ihtw_range_within_bounds(context, size_tag_size))
        {
            return 0;
        }

        if (ihtw_read_size_tag(context, tag_info, tag_info->tag.bits.size
            , tag_info->tag.bits.endianess) != IHTW_ERROR_SUCCESS)
        {
            return IHTW_ERROR_TYPE_SIZE_NOT_SUPPORTED;
        }

        context->index += size_tag_size;
        context->index += tag_info->size_tag;

        tag_info->total_size = (context->buffer + context->index) - start;
        
        return IHTW_ERROR_SUCCESS;
    }

    if (!ihtw_range_within_bounds(context, size))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    tag_info->data = (context->buffer + context->index);
    context->index += size;

    tag_info->total_size = (context->buffer + context->index) - start;

    if ((ihtw_add_counts(&context->type_counts[0], 8) + 1) > context->max_tag_count)
    {
        return IHTW_ERROR_MAX_TAG_COUNT_REACHED;
    }

    return IHTW_ERROR_SUCCESS;
}

int8_t ihtw_calculate_tag_size(struct ihtw_tag_info* tag_info)
{
    tag_info->total_size += 1;

    if (tag_info->format_type == IHTW_WITH_CLASS_BYTE)
    {
        tag_info->total_size += 1;
    }

    if (tag_info->tag.bits.has_name)
    {
        /* 
           If the memory was zeroed around initialization, was it forgotten?
           Was the data corrupted?
           Or in general is the data valid?
        */
        if ((tag_info->name_buffer == 0) || (tag_info->name_size == 0))
        {
            return IHTW_ERROR_NO_NAME_OR_NAME_TOO_LONG;
        }

        tag_info->total_size += tag_info->name_size;
    }

    if (tag_info->tag.bits.type == IHTW_TYPE_BIN)
    {
        tag_info->total_size += IHTW_TYPE_SIZES[tag_info->tag.bits.type] + tag_info->size_tag;
        return IHTW_ERROR_SUCCESS;
    }

    tag_info->total_size += IHTW_TYPE_SIZES[tag_info->tag.bits.type];
    return IHTW_ERROR_SUCCESS;
}

int8_t ihtw_try_write_next_tag(struct ihtw_context* context, struct ihtw_tag_info* tag_info)
{
    if ((ihtw_add_counts(&context->type_counts[0], 8) + 1) > context->max_tag_count)
    {
        return IHTW_ERROR_MAX_TAG_COUNT_REACHED;
    }

    if (!ihtw_calculate_tag_size(tag_info))
    {
        return IHTW_ERROR_CANT_CALCULATE_TAG_SIZE;
    }

    if (!ihtw_range_within_bounds(context, tag_info->total_size))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    tag_info->format_version = IHTW_FORMAT_VERSION;

    /*
        Since VS Studio 2022 and/or or the C standard(s) dont allow declarations in 
        switch cases. This is in case of the bit field format type so we can
        reverse the bit order if needed.
    */
    union ihtw_bit_order local;
    local.bits.first_half = 1;
    local.bits.second_half = 2;

    uint8_t reverse_tag = tag_info->tag.byte;

    switch (context->format_type)
    {
    case IHTW_BIT_FIELD_ONE_BYTE:
        if (local.byte != context->bit_order.byte)
        {
            return (ihtw_bit_swap_lookup_table[reverse_tag & 0b1111] << 4) | ihtw_bit_swap_lookup_table[reverse_tag >> 4];
        }
        else
        {
            *(uint8_t*)(context->buffer + context->index) = tag_info->tag.byte;
        }

        context->index++;
        break;
    case IHTW_NUMBER_RANGE_ONE_BYTE:
        *(uint8_t*)(context->buffer + context->index) = tag_info->tag.byte;
        context->index++;
        break;
    case IHTW_WITH_CLASS_BYTE:
        if (local.byte != context->bit_order.byte)
        {
            return (ihtw_bit_swap_lookup_table[reverse_tag & 0b1111] << 4) | ihtw_bit_swap_lookup_table[reverse_tag >> 4];
        }
        else
        {
            *(uint8_t*)(context->buffer + context->index) = tag_info->tag.byte;
        }

        context->index++;

        *(uint8_t*)(context->buffer + context->index) = tag_info->class_byte;
        
        context->index++;
        break;
    }

    if (tag_info->tag.bits.has_name)
    {
        if (tag_info->name_size > context->max_name_buffer_size)
        {
            return 0;
        }

        ihtw_memcpy(tag_info->name_buffer, (IHTW_UINTPTR*)(context->buffer + context->index), tag_info->name_size);
        context->index += tag_info->name_size;
    }

    if (tag_info->tag.bits.type == IHTW_TYPE_BIN)
    {
        if (ihtw_write_size_tag(context, tag_info) != IHTW_ERROR_SUCCESS)
        {
            return IHTW_ERROR_CANT_WRITE_BIN_TAG;
        }
    }

    ihtw_memcpy(tag_info->data
        , *(uint8_t*)(context->buffer + context->index)
        , IHTW_TYPE_SIZES[tag_info->tag.bits.size]);

    context->index += IHTW_TYPE_SIZES[tag_info->tag.bits.size];

    context->type_counts[tag_info->tag.bits.type]++;

    return IHTW_ERROR_SUCCESS;
}

int8_t ihtw_compare_tag(struct ihtw_tag_info* tag_info, struct ihtw_match_tag_info* match_tag_info)
{
    if (match_tag_info->match_format_version != 2)
    {
        if (match_tag_info->format_version != tag_info->format_version)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_format_type != 2)
    {
        if (match_tag_info->format_type != tag_info->format_type)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_endianess != 2)
    {
        if (match_tag_info->endianess != tag_info->endianess)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_bit_order != 2)
    {
        if (match_tag_info->bit_order.byte != tag_info->bit_order.byte)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_tag_type != 2)
    {
        if (match_tag_info->tag.bits.type != tag_info->tag.bits.type)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_tag_size != 2)
    {
        if (match_tag_info->tag.bits.size != tag_info->tag.bits.size)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_tag_has_name != 2)
    {
        if (match_tag_info->tag.bits.has_name != tag_info->tag.bits.type)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_tag_endianess != 2)
    {
        if (match_tag_info->tag.bits.endianess != tag_info->tag.bits.endianess)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_class_byte != 2)
    {
        if (match_tag_info->class_byte != tag_info->class_byte)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_name_size != 2)
    {
        if (match_tag_info->name_size != tag_info->name_size)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_name_buffer != 2)
    {
        if (!ihtw_memcmp(match_tag_info->name_buffer
            , tag_info->name_buffer
            , match_tag_info->name_size))
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_size_tag != 2)
    {
        if (match_tag_info->size_tag != tag_info->size_tag)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }
    if (match_tag_info->match_data != 2)
    {
        if ((match_tag_info->tag.bits.type == IHTW_TYPE_BIN != match_tag_info->tag.bits.type == IHTW_TYPE_BIN)
            && (match_tag_info->size_tag != tag_info->size_tag))
        {
            if (!ihtw_memcmp(match_tag_info->data, tag_info->data, match_tag_info->size_tag))
            {
                return IHTW_ERROR_TAG_INFO_MISMATCH;
            }
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    if (match_tag_info->match_total_size != 2)
    {
        if (match_tag_info->total_size != tag_info->total_size)
        {
            return IHTW_ERROR_TAG_INFO_MISMATCH;
        }
    }

    return IHTW_ERROR_SUCCESS;
}

int8_t ihtw_list_all_tags(struct ihtw_context* context, struct ihtw_tag_info* tag_info, IHTW_UINTPTR* tag_count, IHTW_UINTPTR tag_limit)
{
    struct ihtw_context context_copy = { 0 };
    ihtw_memcpy((void*)context, (void*)&context_copy, sizeof(context_copy));

    struct ihtw_tag_info current_tag = { 0 };

    do
    {
        *tag_count++;

        if (tag_info)
        {
            ihtw_memcpy((void*)&current_tag, (void*)&tag_info[*tag_count], sizeof(struct ihtw_tag_info));
        }
    } while ((*tag_count < tag_limit) && (ihtw_try_read_next_tag(&context_copy, &current_tag) == 1));

    return (*tag_count > 0) ? 1 : 0;
}

int8_t ihtw_search(struct ihtw_context* context, struct ihtw_match_tag_info* search_query, struct ihtw_tag_info* tag_info,
    IHTW_UINTPTR range_start, IHTW_UINTPTR range_end, IHTW_UINTPTR* results, IHTW_UINTPTR result_count)
{
    uint8_t count_results = 0;
    if (!results)
    {
        count_results = 1;
    }

    uint8_t found_a_match = 0;

    for (IHTW_UINTPTR i = range_start; i < range_end; i++)
    {
        if (!ihtw_compare_tag(&tag_info[i], search_query))
        {
            continue;
        }

        found_a_match = 1;

        if (!results)
        {
            result_count++;
        }
        else
        {
            results[i] = &tag_info[i];
        }
    }

    return (found_a_match) ? 1 : 0;
}

void ihtw_reset_context(struct ihtw_context* context, uint8_t format_version, uint8_t format_type, uint8_t bit_order, IHTW_UINTPTR buffer, IHTW_UINTPTR size, uint8_t max_name_buffer_size,
    IHTW_UINTPTR max_tag_count)
{
    ihtw_memset(context, 0, sizeof(struct ihtw_context));

    context->format_version = format_version;
    context->format_type = format_type;

    context->bit_order.byte = bit_order;

    context->buffer = buffer;
    context->index = 0;
    context->size = size;

    context->max_name_buffer_size = max_name_buffer_size;
    context->max_tag_count = max_tag_count;
}

int8_t ihtw_add_headers(struct ihtw_context* context, uint8_t add_header, uint8_t format_version, uint8_t format_type, uint8_t bit_order)
{
    if (add_header)
    {
        if (!ihtw_range_within_bounds(context, sizeof(IHTW_HEADER)))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        memcpy((context->buffer + context->index), IHTW_HEADER, sizeof(IHTW_HEADER));
        context->index += 4;
    }
    
    if (!ihtw_range_within_bounds(context, 2))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    *(uint8_t*)(context->buffer + context->index) = IHTW_FORMAT_VERSION;
    context->index += 1;

    *(uint8_t*)(context->buffer + context->index) = IHTW_FORMAT_VERSION;
    context->index += 1;

    if (bit_order)
    {
        if (!ihtw_range_within_bounds(context, 1))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        *(uint8_t*)(context->buffer + context->index) = bit_order;
        context->index += 1;
    }
    
    return IHTW_ERROR_SUCCESS;
}

int8_t ihtw_parse_headers(struct ihtw_context* context, uint8_t expect_header, uint8_t expect_bit_order)
{
    if (expect_header)
    {
        if (!ihtw_range_within_bounds(context, sizeof(IHTW_HEADER)))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        if (!ihtw_memcmp((context->buffer + context->index), IHTW_HEADER, sizeof(IHTW_HEADER)))
        {
            return IHTW_ERROR_GENERIC_UNKNOWN;
        }

        context->index += 4;
    }

    if (!ihtw_range_within_bounds(context, 2))
    {
        return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
    }

    context->format_version = *(uint8_t*)(context->buffer + context->index);
    context->index += 1;

    context->format_type =  *(uint8_t*)(context->buffer + context->index);
    context->index += 1;

    if (expect_bit_order)
    {
        if (!ihtw_range_within_bounds(context, 1))
        {
            return IHTW_ERROR_RANGE_OUT_OF_BOUNDS;
        }

        context->bit_order.byte = *(uint8_t*)(context->buffer + context->index);
        context->index += 1;
    }

    return IHTW_ERROR_SUCCESS;
}