#ifndef __PTL_UNITTEST_H_
#define __PTL_UNITTEST_H_

#include <test/unittest.h>

using namespace Prot;

#ifdef PTUT_ASSERT
#define EXPECT_EQ A_EQ
#define EXPECT_NE A_NE
#define EXPECT_GT A_GT
#define EXPECT_LT A_LT
#define EXPECT_GE A_GE
#define EXPECT_LE A_LE
#define EXPECT_STREQ A_STREQ
#define EXPECT_STRNE A_STRNE
#define EXPECT_MEMEQ A_MEMEQ
#define EXPECT_MEMNE A_MEMNE
#define EXPECT_TRUE A_TRUE
#define EXPECT_FALSE A_FALSE
#else
#define EXPECT_EQ T_EQ
#define EXPECT_NE T_NE
#define EXPECT_GT T_GT
#define EXPECT_LT T_LT
#define EXPECT_GE T_GE
#define EXPECT_LE T_LE
#define EXPECT_STREQ T_STREQ
#define EXPECT_STRNE T_STRNE
#define EXPECT_MEMEQ T_MEMEQ
#define EXPECT_MEMNE T_MEMNE
#define EXPECT_TRUE T_TRUE
#define EXPECT_FALSE T_FALSE

#endif

#endif