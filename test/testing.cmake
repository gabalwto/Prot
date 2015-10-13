SET(FOLDER_NAME test)

IF (NOT BUILD_TWOPACK)
IF (BUILD_STANDALONE)
  SET(DEPENDENT_LIBS
    system
    time
    synchronization
  )
  SET(TARGET_FOLOER "Prot/Test")
ELSE (BUILD_STANDALONE)
  SET(DEPENDENT_LIBS prot)
  SET(TARGET_FOLOER "Test/Prot")
ENDIF (BUILD_STANDALONE)

SET(PTUTEST_SRCS
test/unittest.h
test/unittest.cpp
)
ADD_LIBRARY(ptutest STATIC ${PTUTEST_SRCS})
TARGET_LINK_LIBRARIES(ptutest ${DEPENDENT_LIBS})
SET_TARGET_PROPERTIES(ptutest PROPERTIES FOLDER "${TARGET_FOLOER}")

ADD_LIBRARY(ptutest_main STATIC test/unittest_main.cpp)
TARGET_LINK_LIBRARIES(ptutest_main ptutest)
SET_TARGET_PROPERTIES(ptutest_main PROPERTIES FOLDER "${TARGET_FOLOER}")
INSTALL(TARGETS ptutest ptutest_main DESTINATION "lib")

ADD_EXECUTABLE(unittest_test test/unittest_test.cpp)
TARGET_LINK_LIBRARIES(unittest_test ptutest ptutest_main)
SET_TARGET_PROPERTIES(unittest_test PROPERTIES FOLDER "${TARGET_FOLOER}")
ADD_TEST(unittest_test unittest_test)
ENDIF (NOT BUILD_TWOPACK)

INSTALL(FILES test/ptutest.h DESTINATION "include")
INSTALL(FILES test/unittest.h DESTINATION "include/test")
