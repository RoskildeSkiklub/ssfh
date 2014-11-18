/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

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
