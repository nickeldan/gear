#include <scrutiny/scrutiny.h>

#include <gear/config.h>
#include <gear/gear.h>

static size_t
shortExpander(size_t capacity)
{
    return (capacity < 10) ? capacity + 2 : 0;
}

static void
testInit(void)
{
    gear array;

    gearInit(&array, sizeof(int));

    SCR_ASSERT_EQ(array.item_size, sizeof(int));
    SCR_ASSERT_EQ(array.length, 0);
}

static void
testAppend(void)
{
    int num = 5;
    gear array;

    gearInit(&array, sizeof(int));

    SCR_ASSERT_EQ(gearAppend(&array, NULL), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearAppend(NULL, &num), GEAR_RET_BAD_USAGE);

    SCR_ASSERT_EQ(gearAppend(&array, &num), GEAR_RET_OK);
    SCR_ASSERT_EQ(array.length, 1);
    SCR_ASSERT_EQ(*(int *)GEAR_GET_ITEM(&array, 0), 5);

    gearReset(&array);
    SCR_ASSERT_EQ(array.length, 0);
    SCR_ASSERT_EQ(array.item_size, sizeof(int));
}

static void
testInvalidExpansion(void)
{
    gear array;

    gearInit(&array, sizeof(int));

    SCR_ASSERT_EQ(gearSetExpansion(NULL, 1, 1), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearSetExpansion(&array, 0, 1), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearSetExpansion(&array, 1, 0), GEAR_RET_BAD_USAGE);
}

static void
testExpander(void)
{
    int num = 5;
    gear array;

    gearInit(&array, sizeof(int));

    SCR_ASSERT_EQ(gearSetExpander(NULL, shortExpander), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearSetExpander(&array, NULL), GEAR_RET_BAD_USAGE);

    SCR_ASSERT_EQ(gearSetExpander(&array, shortExpander), GEAR_RET_OK);

    for (int k = 0; k < 10; k++) {
        SCR_ASSERT_EQ(gearAppend(&array, &num), GEAR_RET_OK);
    }

    SCR_ASSERT_EQ(gearAppend(&array, &num), GEAR_RET_NO_EXPANSION);

    gearReset(&array);
}

static void
testIteration(void)
{
    int count;
    int *num;
    gear array;

    gearInit(&array, sizeof(int));

    for (int k = 0; k < 5; k++) {
        SCR_ASSERT_EQ(gearAppend(&array, &k), GEAR_RET_OK);
    }

    count = 0;
    GEAR_FOR_EACH(&array, num)
    {
        SCR_ASSERT_EQ(*num, count);
        count++;
    }

    SCR_ASSERT_EQ(count, array.length);

    gearReset(&array);
}

static void
testIterationWithIndex(void)
{
    int count;
    int *num;
    size_t idx;
    gear array;

    gearInit(&array, sizeof(int));

    for (int k = 0; k < 5; k++) {
        SCR_ASSERT_EQ(gearAppend(&array, &k), GEAR_RET_OK);
    }

    count = 0;
    GEAR_FOR_EACH_WITH_INDEX(&array, num, idx)
    {
        SCR_ASSERT_EQ(*num, count);
        SCR_ASSERT_EQ(idx, count);
        count++;
    }

    SCR_ASSERT_EQ(count, array.length);

    gearReset(&array);
}

static void
testLoad(void)
{
    int nums[] = {1, 2, 3, 4, 5, 6};
    gear array;

    gearInit(&array, sizeof(int));

    SCR_ASSERT_EQ(gearLoad(NULL, nums, 6), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearLoad(&array, NULL, 6), GEAR_RET_BAD_USAGE);

    SCR_ASSERT_EQ(gearLoad(&array, nums, 3), GEAR_RET_OK);
    SCR_ASSERT_EQ(array.length, 3);
    SCR_ASSERT_MEM_EQ(GEAR_GET_ITEM(&array, 0), nums, sizeof(int) * 3);

    SCR_ASSERT_EQ(gearLoad(&array, &nums[3], 3), GEAR_RET_OK);
    SCR_ASSERT_EQ(array.length, 6);
    SCR_ASSERT_MEM_EQ(GEAR_GET_ITEM(&array, 0), nums, sizeof(int) * 6);

    gearReset(&array);
}

static void
testPop(void)
{
    int num = 0;
    int nums1[] = {1, 2, 3, 4, 5, 6}, nums2[] = {1, 2, 4, 5, 6}, nums3[] = {1, 2, 4, 6};
    gear array;

    gearInit(&array, sizeof(int));
    SCR_ASSERT_EQ(gearLoad(&array, nums1, 6), GEAR_RET_OK);

    SCR_ASSERT_EQ(gearPop(NULL, 0, NULL), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearPop(&array, 6, NULL), GEAR_RET_BAD_USAGE);

    SCR_ASSERT_EQ(gearPop(&array, 2, NULL), GEAR_RET_OK);
    SCR_ASSERT_EQ(array.length, 5);
    SCR_ASSERT_MEM_EQ(GEAR_GET_ITEM(&array, 0), nums2, sizeof(int) * 5);

    SCR_ASSERT_EQ(gearPop(&array, 3, &num), GEAR_RET_OK);
    SCR_ASSERT_MEM_EQ(GEAR_GET_ITEM(&array, 0), nums3, sizeof(int) * 4);
    SCR_ASSERT_EQ(num, 5);
}

static void
testConcatenate(void)
{
    int nums[] = {1, 2, 3, 4, 5, 6};
    gear array1, array2;

    gearInit(&array1, sizeof(int));
    gearInit(&array2, sizeof(char));

    SCR_ASSERT_EQ(gearConcatenate(&array1, &array2), GEAR_RET_BAD_USAGE);

    gearInit(&array2, sizeof(int));

    SCR_ASSERT_EQ(gearConcatenate(NULL, &array2), GEAR_RET_BAD_USAGE);
    SCR_ASSERT_EQ(gearConcatenate(&array1, NULL), GEAR_RET_BAD_USAGE);

    SCR_ASSERT_EQ(gearLoad(&array1, nums, 3), GEAR_RET_OK);
    SCR_ASSERT_EQ(gearLoad(&array2, &nums[3], 3), GEAR_RET_OK);

    SCR_ASSERT_EQ(gearConcatenate(&array1, &array2), GEAR_RET_OK);
    SCR_ASSERT_EQ(array1.length, 6);
    SCR_ASSERT_MEM_EQ(GEAR_GET_ITEM(&array1, 0), nums, sizeof(int) * 6);

    gearReset(&array1);
    gearReset(&array2);
}

int
main()
{
    int ret;
    scrRunner *runner;
    scrGroup *group;

    runner = scrRunnerCreate();

    group = scrGroupCreate(runner, NULL, NULL);
    scrGroupAddTest(group, "Initialize", testInit, 0, 0);
    scrGroupAddTest(group, "Append", testAppend, 0, 0);
    scrGroupAddTest(group, "Invalid expansion", testInvalidExpansion, 0, 0);
    scrGroupAddTest(group, "Expander", testExpander, 0, 0);
    scrGroupAddTest(group, "Iteration", testIteration, 0, 0);
    scrGroupAddTest(group, "Iteration with index", testIterationWithIndex, 0, 0);
    scrGroupAddTest(group, "Load", testLoad, 0, 0);
    scrGroupAddTest(group, "Pop", testPop, 0, 0);
    scrGroupAddTest(group, "Concatenate", testConcatenate, 0, 0);

    ret = scrRunnerRun(runner, NULL, NULL);
    scrRunnerDestroy(runner);
    return ret;
}
