#include "test_flash.hpp"

#include "chprintf.h"
#include "string.h"

#include "io/io.hpp"


BaseSequentialStream* chStdout = (BaseSequentialStream*) &SD_OUTPUT;

extern uint8_t __config_start;   // configured via linker script when building binaries.
extern uint8_t __config_end;


#define TEST_FIRST_FREE_SECTOR 5
#define TEST_READWRITE_TESTCASE_COUNT 8
#define TEST_BASE 0x08060000
#define TEST_DATASIZE sizeof(flashdata_t)

#define PRINTABLE_CHARACTER_FIRST 0x20
#define PRINTABLE_CHARACTER_LAST 0x7e
#define PRINTABLE_CHARACTER_COUNT (PRINTABLE_CHARACTER_LAST - PRINTABLE_CHARACTER_FIRST + 1)

#define COLORIZE_GREEN(message) "\033[1;32m" message "\033[0m"
#define COLORIZE_RED(message) "\033[1;31m" message "\033[0m"

#define TEST_VERIFY(message, statement) \
do { \
    chprintf(chStdout, "%s - " message "\r\n", \
        statement ? COLORIZE_GREEN("PASS") : COLORIZE_RED("FAIL")); \
} while (0)


static void fillWithPattern(char* buffer, size_t size)
{
    /* Generate a pattern with printable characters to ease debugging) */
    int c;
    for (c = 0; c < (int)size; ++c)
        buffer[c] = (c % PRINTABLE_CHARACTER_COUNT) + PRINTABLE_CHARACTER_FIRST;
}

// test simple scenarion: reading and writing 32 bit buffer
void testFlashRWSingle(void)
{
	flashsector_t sector = EE_flashSectorAt( (flashaddr_t)&__config_start );

    chprintf( chStdout, "address: %x - %x -> %d \r\n", (flashaddr_t)&__config_start, (flashaddr_t)&__config_end, sector );
	EE_flashSectorErase( sector);

    char writeBuffer[32];
    fillWithPattern(writeBuffer, 32);
    uint8_t i=0;
    chprintf( chStdout, "\"" );
    for ( ; i<32; ++i)
        chprintf( chStdout, "%c", writeBuffer[i] );
    chprintf( chStdout, "\"\r\n" );


    char readBufferBefore[32];
    char readBufferAfter[32];

    EE_flashRead( (flashaddr_t)&__config_start, readBufferBefore, 32 );

    chprintf( chStdout, "\"" );
    for ( i=0; i<32; ++i)
        chprintf( chStdout, "%c", readBufferBefore[i] );
    chprintf( chStdout, "\"\r\n" );

    int err;
    err = EE_flashWrite( (flashaddr_t)&__config_start, writeBuffer, 32);
	if ( err )
		chprintf(chStdout, "Error writing flash: %d\r\n", err);
	else
		chprintf(chStdout, "Buffer written to flash\r\n");

    err = EE_flashRead( (flashaddr_t)&__config_start, readBufferAfter, 32 );
	if ( err )
		chprintf(chStdout, "Error reading flash: %d\r\n", err);
	else
		chprintf(chStdout, "Buffer read to flash\r\n");

    chprintf( chStdout, "\"" );
    for ( i=0; i<32; ++i)
        chprintf( chStdout, "%c", readBufferAfter[i] );
    chprintf( chStdout, "\"\r\n" );
}



 // The test_flashreadwrite_data struct defines a test case data for the read/write test.
struct test_flashreadwrite_data
{
    const char* testName; 	// Name of the test case
    flashaddr_t from; 		// First address to be read/write (inclusive)
    flashaddr_t to; 		// Last address to be read/write (exclusive)
};

// Test cases for the read/write test.
// "big data" tests read/write more than sizeof(flashdata_t) bytes while
// "tiny data" tests read/write sizeof(flashdata_t) bytes at most.
struct test_flashreadwrite_data flashreadwrite_testcases[TEST_READWRITE_TESTCASE_COUNT] =
{
    { "aligned start, aligned end, big data",          TEST_BASE +  1 * TEST_DATASIZE + 0, TEST_BASE +  4 * TEST_DATASIZE + 0 },
    { "aligned start, not aligned end, big data",      TEST_BASE +  4 * TEST_DATASIZE + 0, TEST_BASE +  6 * TEST_DATASIZE + 1 },
    { "not aligned start, not aligned end, big data",  TEST_BASE +  6 * TEST_DATASIZE + 1, TEST_BASE +  9 * TEST_DATASIZE - 1 },
    { "not aligned start, aligned end, big data",      TEST_BASE +  9 * TEST_DATASIZE - 1, TEST_BASE + 11 * TEST_DATASIZE + 0 },
    { "aligned start, aligned end, tiny data",         TEST_BASE + 11 * TEST_DATASIZE + 0, TEST_BASE + 12 * TEST_DATASIZE + 0 },
    { "aligned start, not aligned end, tiny data",     TEST_BASE + 12 * TEST_DATASIZE + 0, TEST_BASE + 12 * TEST_DATASIZE + 1 },
    { "not aligned start, not aligned end, tiny data", TEST_BASE + 12 * TEST_DATASIZE + 1, TEST_BASE + 13 * TEST_DATASIZE - 1 },
    { "not aligned start, aligned end, tiny data",     TEST_BASE + 13 * TEST_DATASIZE - 1, TEST_BASE + 13 * TEST_DATASIZE + 0 }
};


 // Execute the read/write test for several scenarios.
void testFlashRWMulti(void)
{
    // Erase all sectors needed by the test cases
    flashsector_t sector;
    for (sector = TEST_FIRST_FREE_SECTOR; sector < EE_FLASH_SECTOR_COUNT; ++sector)
        EE_flashSectorErase(sector);

    // Execute the test cases
    int i;
    for (i = 0; i < TEST_READWRITE_TESTCASE_COUNT; ++i)
    {
        const struct test_flashreadwrite_data* testcase = &flashreadwrite_testcases[i];

        chprintf( chStdout, "\r\n> test flash read/write: %s\r\n\r\n", testcase->testName);

        char writeBuffer[testcase->to - testcase->from];
        fillWithPattern(writeBuffer, testcase->to - testcase->from);
        char readBufferBefore[32];
        char readBufferAfter[32];
        TEST_VERIFY("read previous data (before target)", EE_flashRead(testcase->from - sizeof(readBufferBefore), readBufferBefore, sizeof(readBufferBefore)) == EE_FLASH_RETURN_SUCCESS);
        TEST_VERIFY("read previous data (after target)", EE_flashRead(testcase->to, readBufferAfter, sizeof(readBufferAfter)) == EE_FLASH_RETURN_SUCCESS);

        TEST_VERIFY("write data to flash", EE_flashWrite(testcase->from, writeBuffer, testcase->to - testcase->from) == EE_FLASH_RETURN_SUCCESS);
        TEST_VERIFY("previous data not overwritten (before target)", EE_flashCompare(testcase->from - sizeof(readBufferBefore), readBufferBefore, sizeof(readBufferBefore)) == TRUE);
        TEST_VERIFY("previous data not overwritten (after target)", EE_flashCompare(testcase->to, readBufferAfter, sizeof(readBufferAfter)) == TRUE);
        TEST_VERIFY("compare written data", EE_flashCompare(testcase->from, writeBuffer, testcase->to - testcase->from) == TRUE);

        char readBuffer[testcase->to - testcase->from];
        memset(readBuffer, '#', testcase->to - testcase->from);
        TEST_VERIFY("read back written data", EE_flashRead(testcase->from, readBuffer, testcase->to - testcase->from) == EE_FLASH_RETURN_SUCCESS);
        TEST_VERIFY("compare read and write data", memcmp(readBuffer, writeBuffer, testcase->to - testcase->from) == 0);
    }
}
