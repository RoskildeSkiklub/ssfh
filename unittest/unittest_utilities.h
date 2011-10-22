#ifndef UNITTEST_UTILITIES_H
#define UNITTEST_UTILITIES_H
// Qt unit testing sucks somwhat...
// This defines a machro that checks for a throw
#define QVERIFY_THROW(expression, ExpectedExceptionType) \
 do \
 { \
 bool caught_ = false; \
 try { expression; } \
 catch (ExpectedExceptionType const&) { caught_ = true; } \
 catch (...) {} \
 if (!QTest::qVerify(caught_, #expression ", " #ExpectedExceptionType, "", __FILE__, __LINE__))\
 return; \
 } while(0)
#endif // UNITTEST_UTILITIES_H
