#include <stdio.h>
#include <Windows.h>
#include "ihtw-c.h"

// WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION!
// WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION!
// WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION!
// WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION!
// WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION! WARNING! CAUTION!
// 
// This can acquire display sensitive information.
// Be careful running this and sharing a screenshot or copying the results in text.
//
// This will most likely seem very tedious, and I might but probably update this a little here and there at least once every several months. (For the next 2 or 3 years)
// I apologize for any annoyance or inconvenience.

enum SERIALIZER_CONTEXTS
{
	MAIN_CONTEXT = 0,
	DESKTOP_INFO_LIST,
	DESKTOP_INFO,
	WINDOW_INFO_LIST
};

union ihtw_bit_order bit_order = { 0 };

uint8_t endianess = 0;

struct ihtw_context* contexts[5] = { 0 };

struct ihtw_tag_info ihtw_tag_info = { 0 };

enum SERIALIZER_OBJECT_IDS
{
	GENERIC_ERROR = 0,

	DESKTOP_OBJECT,
	DESKTOP_NAME_UTF16, // Will be type bin UINT16
	DESKTOP_INDEX_UINT32,


	WINDOW_OBJECT,
	WINDOW_NAME_UTF16, // Will be type bin UINT16
	WINDOW_INDEX_UINT32,
};

BOOL CALLBACK enum_desktop_proc(
	_In_ LPTSTR lpszDesktop,
	_In_ LPARAM lParam
)
{
	uint32_t* index = (uint32_t*)lParam;

	ihtw_tag_info.format_version = IHTW_FORMAT_VERSION;
	ihtw_tag_info.format_type = IHTW_WITH_CLASS_BYTE;

	ihtw_tag_info.bit_order.byte = bit_order.byte;
	ihtw_tag_info.endianess = endianess;

	ihtw_tag_info.tag.bits.type = IHTW_TYPE_BIN;
	ihtw_tag_info.tag.bits.size = IHTW_SIZE_16;
	ihtw_tag_info.tag.bits.has_name = 1;
	ihtw_tag_info.tag.bits.endianess = endianess;

	const char* object1_name = "DESKTOP_NAME_UTF16";
	ihtw_tag_info.name_buffer = object1_name;
	ihtw_tag_info.name_size = 19; // This includes the null byte, as the library does

	uint16_t str_write_size = (wcslen(lpszDesktop) + 1) * 2; // Add 1 for null byte as library does (while wcslen doesnt). Multiply by 2 becaue 1 wide char is 2 bytes always (UTF16)
	ihtw_tag_info.size_tag = str_write_size;
	ihtw_tag_info.data = (uint8_t*)lpszDesktop;

	ihtw_tag_info.class_byte = DESKTOP_NAME_UTF16;
	if (!ihtw_try_write_next_tag(&contexts[DESKTOP_INFO], &ihtw_tag_info))
	{
		printf("Failed to write individual desktop name tag.\n");
		system("pause");
	}

	ihtw_tag_info.tag.bits.type = IHTW_TYPE_UINT;
	ihtw_tag_info.tag.bits.size = IHTW_SIZE_32;
	ihtw_tag_info.tag.bits.has_name = 1;
	ihtw_tag_info.tag.bits.endianess = endianess;

	const char* object2_name = "DESKTOP_INDEX_UINT32";
	ihtw_tag_info.name_buffer = object2_name;
	ihtw_tag_info.name_size = 21; // This includes the null byte, as the library does

	ihtw_tag_info.data = (uint8_t*)index;

	ihtw_tag_info.class_byte = DESKTOP_INDEX_UINT32;
	if (!ihtw_try_write_next_tag(&contexts[DESKTOP_INFO], &ihtw_tag_info))
	{
		printf("Failed to write individual desktop index tag.\n");
		system("pause");
	}

	*index += 1;


	ihtw_tag_info.tag.bits.type = IHTW_TYPE_BIN;
	ihtw_tag_info.tag.bits.size = IHTW_SIZE_32;
	ihtw_tag_info.tag.bits.has_name = 1;
	ihtw_tag_info.tag.bits.endianess = endianess;

	const char* object3_name = "DESKTOP_OBJECT";
	ihtw_tag_info.name_buffer = object3_name;
	ihtw_tag_info.name_size = 15; // This includes the null byte, as the library does

	ihtw_tag_info.size_tag = (uint32_t)contexts[DESKTOP_INFO]->index;
	ihtw_tag_info.data = contexts[DESKTOP_INFO]->buffer;

	ihtw_tag_info.class_byte = DESKTOP_OBJECT;
	if (!ihtw_try_write_next_tag(&contexts[DESKTOP_INFO_LIST], &ihtw_tag_info))
	{
		printf("Failed to write individual desktop info list entry tag.\n");
		system("pause");
	}

	wprintf(L"Written Desktop info list entry: index %u named \"%s\"\n\t current DESKTOP_INFO_LIST context index: %llu bytes\n",
		*index, lpszDesktop, contexts[DESKTOP_INFO_LIST]->index);

	//ihtw_reset_context(&contexts[DESKTOP_INFO], IHTW_FORMAT_VERSION, IHTW_LIBRARY_VERSION
	//	, &contexts[DESKTOP_INFO]->bit_order, &contexts[DESKTOP_INFO]->buffer, 1000ull * 1000ull, 256, 1000ull);

	ihtw_memset(contexts[DESKTOP_INFO]->buffer, 0, contexts[DESKTOP_INFO]->index);
	contexts[DESKTOP_INFO]->index = 0;

	return TRUE;
}

int main()
{
	for (int i = 0; i < 5; i++)
	{
		// Step 1: Allocate buffers and setup limits
		// I don't think you normally need a 1MB buffer, its just to avoid the insufficent buffer size error with Windows APIs.
		contexts[i] = (struct ihtw_context*)malloc(sizeof(struct ihtw_context));
		memset(contexts[i], 0, sizeof(struct ihtw_context));

		memset(&contexts[i]->type_counts[0], 0, sizeof(IHTW_UINTPTR) * 8);

		if (!contexts[i])
		{
			printf("Insufficient memory to allocate struct ihtw_context.\n");
			system("pause");
		}

		contexts[i]->buffer = malloc(1000 * 1000);
		if (!contexts[i]->buffer)
		{
			printf("Insufficient memory to allocate 1mb buffer.\n");
			system("pause");
		}
		contexts[i]->size = 1000 * 1000;

		// I think in windows currently its 255-256 bytes NORMALLY. IIRC in some cases it's above 255/256 bytes like with file directories and/or symbolic links?
		// This library includes the null byte in ANSI string searches with a custom memchr to avoid using standard library.
		contexts[i]->max_name_buffer_size = 256;
		contexts[i]->max_tag_count = 1000ull;


		// Step 2: (optional) add IHTW header.
		// This step is optional if the all clients and servers are only using IHTW  (specific configuration) to serialize data.
		// This does not include the null byte in the string, as it's a header
		memcpy((contexts[i]->buffer + contexts[i]->index), &IHTW_HEADER, sizeof(IHTW_HEADER));
		contexts[i]->index += 4;


		// Step 3: (optional) add IHTW format version.
		// This step is optional if the all clients and servers are only using IHTW (specific configuration) to serialize data.
		contexts[i]->format_version = IHTW_FORMAT_VERSION;
		*(uint8_t*)(contexts[i]->buffer + contexts[i]->index) = IHTW_FORMAT_VERSION;
		contexts[i]->index += 1;


		// Step 4: (optional) add IHTW format type. In this example we will use the class byte
		// This step is optional if the all clients and servers are only using IHTW  (specific configuration) to serialize data.
		contexts[i]->format_type = IHTW_WITH_CLASS_BYTE;
		*(uint8_t*)(contexts[i]->buffer + contexts[i]->index) = IHTW_FORMAT_VERSION;
		contexts[i]->index += 1;


		// Step 5: (almost always needed) add the bit order.
		// Unless all clients and servers are using the same EXACT compiler (version + configuration) and architecture,
		// then you dont need to add this, but I would.
		// Soon I'll most likely add a macro to toggle this on and off, but I wouldn't refuse to use this.
		bit_order.bits.first_half = 1;
		bit_order.bits.second_half = 2;

		contexts[i]->bit_order.byte = bit_order.byte;
		*(uint8_t*)(contexts[i]->buffer + contexts[i]->index) = bit_order.byte;
		contexts[i]->index += 1;
	}

	endianess = ihtw_get_endianess();

	// Step 7: Enumerate Desktops
	uint32_t index = 0;

	if (!EnumDesktopsW(NULL, &enum_desktop_proc, (LPARAM)&index))
	{
		printf("EnumDesktopsW failed.\n");
		system("pause");
	}

	printf("program is finished.. system(\"pause\");.\n");
	system("pause");
}