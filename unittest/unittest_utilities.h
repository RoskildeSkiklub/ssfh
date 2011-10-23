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
 if (!QTest::qVerify(caught_, #expression ", Exception.type == " #ExpectedExceptionType, "", __FILE__, __LINE__))\
 return; \
 } while(0)

#define QVERIFY_EXCEPTION_STATUS(expression, StatusCode) \
 do \
 { \
 bool caught_ = false; \
 long statuscode_ = -1; \
 try { expression; } \
 catch (Exception const& ex) { caught_ = true; statuscode_ = ex.getStatusCode(); } \
 catch (...) {} \
 if (!QTest::qVerify(caught_, #expression ", Exception.type == Exception", "", __FILE__, __LINE__))\
 return; \
 if (!QTest::qVerify(statuscode_ == StatusCode, #expression ", Exception.getStatusCode() == " #StatusCode, "", __FILE__, __LINE__))\
 return; \
 } while(0)

#endif // UNITTEST_UTILITIES_H
