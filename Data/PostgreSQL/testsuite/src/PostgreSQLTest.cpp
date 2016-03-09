//
// PostgreSQLTest.cpp
//
// $Id: //poco/1.4/Data/PostgreSQL/testsuite/src/PostgreSQLTest.cpp#1 $
//
// Copyright (c) 2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "PostgreSQLTest.h"
#include "Poco/CppUnit/TestCaller.h"
#include "Poco/CppUnit/TestSuite.h"
#include "Poco/Environment.h"
#include "Poco/String.h"
#include "Poco/Format.h"
#include "Poco/Tuple.h"
#include "Poco/NamedTuple.h"
#include "Poco/Exception.h"
#include "Poco/Data/LOB.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/PostgreSQL/Connector.h"
#include "Poco/Data/PostgreSQL/Utility.h"
#include "Poco/Data/PostgreSQL/PostgreSQLException.h"
#include "Poco/Nullable.h"
#include "Poco/Data/DataException.h"
#include <iostream>

using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::PostgreSQL::ConnectionException;
using Poco::Data::PostgreSQL::Utility;
using Poco::Data::PostgreSQL::StatementException;
using Poco::format;
using Poco::NotFoundException;
using Poco::Int32;
using Poco::Nullable;
using Poco::Tuple;
using Poco::NamedTuple;
using Poco::Environment;

Poco::SharedPtr<Poco::Data::Session> PostgreSQLTest::_pSession = 0;
Poco::SharedPtr<SQLExecutor> PostgreSQLTest::_pExecutor = 0;

//
// Connection string
std::string PostgreSQLTest::_dbConnString;

//
// Parameters for barebone-test
//
std::string PostgreSQLTest::getHost() {
	return "localhost";
}
std::string PostgreSQLTest::getPort() {
	return "5432";
}
std::string PostgreSQLTest::getBase(){
	return "postgres";
}
std::string PostgreSQLTest::getUser(){
	return "postgres";
}
std::string PostgreSQLTest::getPassword(){
	if (Environment::has("APPVEYOR"))
	{
		return "Password12!";
	}
	else
	{
		return "postgres";
	}
}



PostgreSQLTest::PostgreSQLTest(const std::string& name): 
	CppUnit::TestCase(name)
{
	PostgreSQL::Connector::registerConnector();
}


PostgreSQLTest::~PostgreSQLTest()
{
	PostgreSQL::Connector::unregisterConnector();
}


void PostgreSQLTest::dbInfo(Session& session)
{
	std::cout << "Server Info: "  << Utility::serverInfo(session)  << std::endl;
	std::cout << "Server Version: "   << Utility::serverVersion(session)   << std::endl;
	std::cout << "Host Info: "<< Utility::hostInfo(session)<< std::endl;
	std::cout << "Session Encoding: " << Utility::sessionEncoding(session) << std::endl;
}


void PostgreSQLTest::testConnectNoDB()
{
	std::string dbConnString;
	dbConnString +=  "host=" + getHost();
	dbConnString += " user=" + getUser();
	dbConnString +=	" password=" + getPassword();
	
	try
	{
		std::cout << "Attempting to Connect to [" << dbConnString << "] without database: " << std::endl;
		Session session(PostgreSQL::Connector::KEY, dbConnString);
		std::cout << "Connected to [" << dbConnString << "] without database." << std::endl;
		dbInfo(session);
		std::cout << "Disconnecting ..." << std::endl;
		session.close();
		std::cout << "Disconnected." << std::endl;
	}
	catch (ConnectionFailedException& ex)
	{
		std::cout << ex.displayText() << std::endl;
	}
	catch (ConnectionException& ex)
	{
		std::cout << ex.displayText() << std::endl;
	}
}
	
void PostgreSQLTest::testPostgreSQLOIDs()
{
	if (!_pSession) fail ("Test not available.");

	std::string tableCreateString = "CREATE TABLE Test ("
		"charcol			char,"
		"bigintcol          bigint,"
		"int8col            int8,"
		"bigserialcol       bigserial,"
		"serial8col         serial8,"
		"bit1col			bit(1),"
		"bit4col			bit(4),"
		"bit6col			bit(6),"
		"bit8col			bit(8),"
		"bit9col			bit(9),"
		"bitv1col			bit varying(1),"
		"bitv4col			bit varying(4),"
		"bitv6col			bit varying(6),"
		"bitv8col			bit varying(8),"
		"bitv9col			bit varying(9),"
		"booleancol			boolean,"
		"boolcol			bool,"
		"boxcol				box,"
		"byteacol			bytea,"
		"char1col			char(1),"
		"char4col			char(4),"
		"char6col			char(6),"
		"char8col			char(8),"
		"char9col			char(9),"
		"char12col			char(12),"
		"char256col			char(256),"
		"charv1col			char varying(1),"
		"charv4col			char varying(4),"
		"charv6col			char varying(6),"
		"charv8col			char varying(8),"
		"charv9col			char varying(9),"
		"charv12col			char varying(12),"
		"charv256col		char varying(256),"
		"cidrcol			cidr,"
		"circlecol			circle,"
		"datecol			date,"
		"doubleprecol		double precision,"
		"float8col			float8,"
		"inetcol			inet,"
		"integercol			integer,"
		"intcol				int,"
		"int4col			int4,"
		"jsoncol			json,"
//		"jsonbcol			jsonb,"  // requires 9.4
		"linecol			line,"
		"lsegcol			lseg,"
		"macaddrcol			macaddr,"
		"moneycol			money,"
		"numericcol			numeric(12,3),"
		"decimalcol			decimal(12,4),"
		"pathcol			path,"
//		"pglencol			pg_lsn,"  // requires 9.4
		"pointcol			point,"
		"polygoncol			polygon,"
		"realcol			real,"
		"float4col			float4,"
		"smallintcol		smallint,"
		"int2col			int2,"
		"smallserialcol		smallserial,"
		"serial2col			serial2,"
		"serialcol			serial,"
		"serial4col			serial4,"
		"textcol			text,"
		"timewtzcol			time(6) without time zone,"
		"tsquerycol			tsquery,"
		"tsvectorcol		tsvector,"
		"txidsnapshotcol	txid_snapshot,"
		"uuidcol			uuid,"
		"xmlcol				xml"
		")";
	
	Oid OIDArray[] = {
		1042,
		20,
		20,
		20,
		20,
		1560,
		1560,
		1560,
		1560,
		1560,
		1562,
		1562,
		1562,
		1562,
		1562,
		16,
		16,
		603,
		17,
		1042,
		1042,
		1042,
		1042,
		1042,
		1042,
		1042,
		1043,
		1043,
		1043,
		1043,
		1043,
		1043,
		1043,
		650,
		718,
		1082,
		701,
		701,
		869,
		23,
		23,
		23,
		114,
//		3802,   // requires 9.4
		628,
		601,
		829,
		790,
		1700,
		1700,
		602,
//		3220,
		600,
		604,
		700,
		700,
		21,
		21,
		21,
		21,
		23,
		23,
		25,
		1083,
		3615,
		3614,
		2970,
		2950,
		142
	};
	
	 _pExecutor->oidPostgreSQLTest(getHost(), getUser(), getPassword(), getBase(), getPort(), tableCreateString.c_str(), OIDArray);

}


void PostgreSQLTest::testBarebonePostgreSQL()
{
	if (!_pSession) fail ("Test not available.");
/*
	std::string tableCreateString = "CREATE TABLE Test "
		"(First VARCHAR(30),"
		"Second VARCHAR(30),"
		"Third VARCHAR(30),"
		"Fourth INTEGER,"
		"Fifth FLOAT)";

	_pExecutor->barebonePostgreSQLTest(POSTGRESQL_HOST, POSTGRESQL_USER, POSTGRESQL_PWD, POSTGRESQL_DB, POSTGRESQL_PORT, tableCreateString.c_str());
*/
}


void PostgreSQLTest::testSimpleAccess()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->simpleAccess();
}


void PostgreSQLTest::testComplexType()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->complexType();
}


void PostgreSQLTest::testSimpleAccessVector()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->simpleAccessVector();
}


void PostgreSQLTest::testComplexTypeVector()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->complexTypeVector();
}


void PostgreSQLTest::testInsertVector()
{
	if (!_pSession) fail ("Test not available.");

	recreateStringsTable();
	_pExecutor->insertVector();
}


void PostgreSQLTest::testInsertEmptyVector()
{
	if (!_pSession) fail ("Test not available.");

	recreateStringsTable();
	_pExecutor->insertEmptyVector();
}


void PostgreSQLTest::testInsertSingleBulk()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->insertSingleBulk();
}


void PostgreSQLTest::testInsertSingleBulkVec()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->insertSingleBulkVec();
}


void PostgreSQLTest::testLimit()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limits();
}


void PostgreSQLTest::testLimitZero()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limitZero();
}


void PostgreSQLTest::testLimitOnce()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limitOnce();
	
}


void PostgreSQLTest::testLimitPrepare()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limitPrepare();
}



void PostgreSQLTest::testPrepare()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->prepare();
}


void PostgreSQLTest::testSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->setSimple();
}


void PostgreSQLTest::testSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->setComplex();
}


void PostgreSQLTest::testSetComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->setComplexUnique();
}

void PostgreSQLTest::testMultiSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->multiSetSimple();
}


void PostgreSQLTest::testMultiSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->multiSetComplex();
}


void PostgreSQLTest::testMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->mapComplex();
}


void PostgreSQLTest::testMapComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->mapComplexUnique();
}


void PostgreSQLTest::testMultiMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->multiMapComplex();
}


void PostgreSQLTest::testSelectIntoSingle()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->selectIntoSingle();
}


void PostgreSQLTest::testSelectIntoSingleStep()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->selectIntoSingleStep();
}


void PostgreSQLTest::testSelectIntoSingleFail()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->selectIntoSingleFail();
}


void PostgreSQLTest::testLowerLimitOk()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->lowerLimitOk();
}


void PostgreSQLTest::testSingleSelect()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->singleSelect();
}


void PostgreSQLTest::testLowerLimitFail()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->lowerLimitFail();
}


void PostgreSQLTest::testCombinedLimits()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->combinedLimits();
}



void PostgreSQLTest::testRange()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->ranges();
}


void PostgreSQLTest::testCombinedIllegalLimits()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->combinedIllegalLimits();
}



void PostgreSQLTest::testIllegalRange()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->illegalRange();
}


void PostgreSQLTest::testEmptyDB()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->emptyDB();
}


void PostgreSQLTest::testDateTime()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonDateTimeTable();
	_pExecutor->dateTime();
	recreatePersonDateTable();
	_pExecutor->date();
	recreatePersonTimeTable();
	_pExecutor->time();
}


void PostgreSQLTest::testBLOB()
{
	if (!_pSession) fail ("Test not available.");
	
	recreatePersonBLOBTable();
	_pExecutor->blob();

	const std::size_t maxFldSize = 65534;
	_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize-1));
	recreatePersonBLOBTable();

	try
	{
		_pExecutor->blob(maxFldSize);
		fail ("must fail");
	}
	catch (DataException&) 
	{
		_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize));
	}

	recreatePersonBLOBTable();
	_pExecutor->blob(maxFldSize);

	recreatePersonBLOBTable();

	try
	{
		_pExecutor->blob(maxFldSize+1);
		fail ("must fail");
	}
	catch (DataException&) { }
}

void PostgreSQLTest::testCLOBStmt()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonCLOBTable();
	_pExecutor->clobStmt();
}

void PostgreSQLTest::testBLOBStmt()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonBLOBTable();
	_pExecutor->blobStmt();
}

void PostgreSQLTest::testUnsignedInts()
{
	if (!_pSession) fail ("Test not available.");

	recreateUnsignedIntsTable();
	_pExecutor->unsignedInts();
}


void PostgreSQLTest::testFloat()
{
	if (!_pSession) fail ("Test not available.");

	recreateFloatsTable();
	_pExecutor->floats();
}


void PostgreSQLTest::testDouble()
{
	if (!_pSession) fail ("Test not available.");

	recreateFloatsTable();
	_pExecutor->doubles();
}


void PostgreSQLTest::testTuple()
{
	if (!_pSession) fail ("Test not available.");

	recreateTuplesTable();
	_pExecutor->tuples();
}


void PostgreSQLTest::testTupleVector()
{
	if (!_pSession) fail ("Test not available.");

	recreateTuplesTable();
	_pExecutor->tupleVector();
}


void PostgreSQLTest::testInternalExtraction()
{
	if (!_pSession) fail ("Test not available.");

	recreateVectorsTable();
	_pExecutor->internalExtraction();
}


void PostgreSQLTest::testNull()
{
	if (!_pSession) fail ("Test not available.");

	recreateVectorsTable();
	_pExecutor->doNull();
}


void PostgreSQLTest::testSessionTransaction()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->sessionTransaction(_dbConnString);
}


void PostgreSQLTest::testTransaction()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->transaction(_dbConnString);
}


void PostgreSQLTest::testReconnect()
{
	if (!_pSession) fail ("Test not available.");

	recreatePersonTable();
	_pExecutor->reconnect();
}


void PostgreSQLTest::testNullableInt()
{
	if (!_pSession) fail ("Test not available.");

	recreateNullableIntTable();

	Nullable<Int32> i1(1);
	Nullable<Int32> i2;

	int id = 1;
	*_pSession << "INSERT INTO NullableIntTest VALUES($1, $2)", use(id), use(i1), now;
	id = 2;
	*_pSession << "INSERT INTO NullableIntTest VALUES($1, $2)", use(id), use(i2), now;
	id = 3;
	i2 = 3;
	*_pSession << "INSERT INTO NullableIntTest VALUES($1, $2)", use(id), use(i2), now;

	int count = 0;
	*_pSession << "SELECT COUNT(*) FROM NullableIntTest", into(count), now;
	assert (count == 3);

	Nullable<Int32> ci1;
	Nullable<Int32> ci2;
	Nullable<Int32> ci3;
	id = 1;
	*_pSession << "SELECT Value FROM NullableIntTest WHERE Id = $1", into(ci1), use(id), now;
	assert (ci1 == i1);
	id = 2;
	*_pSession << "SELECT Value FROM NullableIntTest WHERE Id = $1", into(ci2), use(id), now;
	assert (ci2.isNull());
	assert (!(0 == ci2));
	assert (0 != ci2);
	assert (!(ci2 == 0));
	assert (ci2 != 0);
	ci2 = 10;
	assert (10 == ci2);
	assert (ci2 == 10);
	assert (!ci2.isNull());
	id = 3;
	*_pSession << "SELECT Value FROM NullableIntTest WHERE Id = $1", into(ci3), use(id), now;
	assert (!ci3.isNull());
	assert (ci3 == 3);
	assert (3 == ci3);
}


void PostgreSQLTest::testNullableString()
{
	if (!_pSession) fail ("Test not available.");

	recreateNullableStringTable();

	Int32 id = 0;
	Nullable<std::string> address("Address");
	Nullable<Int32> age = 10;
	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(id), use(address), use(age), now;
	id++;
	address = null;
	age = null;
	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(id), use(address), use(age), now;

	Nullable<std::string> resAddress;
	Nullable<Int32> resAge;
	*_pSession << "SELECT Address, Age FROM NullableStringTest WHERE Id = $1", into(resAddress), into(resAge), use(id), now;
	assert(resAddress == address);
	assert(resAge == age);
	assert(resAddress.isNull());
	assert(null == resAddress);
	assert(resAddress == null);

	resAddress = std::string("Test");
	assert(!resAddress.isNull());
	assert(resAddress == std::string("Test"));
	assert(std::string("Test") == resAddress);
	assert(null != resAddress);
	assert(resAddress != null);
}


void PostgreSQLTest::testTupleWithNullable()
{
	if (!_pSession) fail ("Test not available.");

	recreateNullableStringTable();

	typedef Poco::Tuple<Int32, Nullable<std::string>, Nullable<Int32> > Info;

	Info info(0, std::string("Address"), 10);
	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(info), now;
	
	info.set<0>(info.get<0>()++);
	info.set<1>(null);
	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(info), now;

	info.set<0>(info.get<0>()++);
	info.set<1>(std::string("Address!"));
	info.set<2>(null);
	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(info), now;

	std::vector<Info> infos;
	infos.push_back(Info(10, std::string("A"), 0));
	infos.push_back(Info(11, null, 12));
	infos.push_back(Info(12, std::string("B"), null));

	*_pSession << "INSERT INTO NullableStringTest VALUES($1, $2, $3)", use(infos), now;

	std::vector<Info> result;

	*_pSession << "SELECT Id, Address, Age FROM NullableStringTest", into(result), now;

	assert(result[0].get<1>() == std::string("Address"));
	assert(result[0].get<2>() == 10);

	assert(result[1].get<1>() == null);
	assert(result[1].get<2>() == 10);

	assert(result[2].get<1>() == std::string("Address!"));
	assert(result[2].get<2>() == null);

	assert(result[3].get<1>() == std::string("A"));
	assert(result[3].get<2>() == 0);

	assert(result[4].get<1>() == null);
	assert(result[4].get<2>() == 12);

	assert(result[5].get<1>() == std::string("B"));
	assert(result[5].get<2>() == null);

}


void PostgreSQLTest::dropTable(const std::string& tableName)
{
	try { *_pSession << format("DROP TABLE IF EXISTS %s", tableName), now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("dropTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("dropTable()"); }
}


void PostgreSQLTest::recreatePersonTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Age INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonTable()"); }
}


void PostgreSQLTest::recreatePersonBLOBTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Image BYTEA)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonBLOBTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonBLOBTable()"); }
}

void PostgreSQLTest::recreatePersonCLOBTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Story Text)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonCLOBTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonCLOBTable()"); }
}


void PostgreSQLTest::recreatePersonDateTimeTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Birthday TIMESTAMP)", now; } // DATETIME NOT SUPPORTED BY POSTGRES
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonDateTimeTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonDateTimeTable()"); }
}


void PostgreSQLTest::recreatePersonDateTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Birthday DATE)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonDateTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonDateTable()"); }
}


void PostgreSQLTest::recreatePersonTimeTable()
{
	dropTable("Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Birthday TIME)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreatePersonTimeTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreatePersonTimeTable()"); }
}


void PostgreSQLTest::recreateIntsTable()
{
	dropTable("Strings");
	try { *_pSession << "CREATE TABLE Strings (str INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateIntsTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateIntsTable()"); }
}


void PostgreSQLTest::recreateStringsTable()
{
	dropTable("Strings");
	try { *_pSession << "CREATE TABLE Strings (str VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateStringsTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateStringsTable()"); }
}


void PostgreSQLTest::recreateUnsignedIntsTable()
{
	dropTable("Strings");
	try { *_pSession << "CREATE TABLE Strings (str BIGINT)", now; } // UNSIGNED QULIFIER NOT SUPPORTED BY POSTGRES
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateUnsignedIntegersTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateUnsignedIntegersTable()"); }
}


void PostgreSQLTest::recreateFloatsTable()
{
	dropTable("Strings");
	try { *_pSession << "CREATE TABLE Strings (str FLOAT)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateFloatsTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateFloatsTable()"); }
}


void PostgreSQLTest::recreateTuplesTable()
{
	dropTable("Tuples");
	try { *_pSession << "CREATE TABLE Tuples "
		"(i0 INTEGER, i1 INTEGER, i2 INTEGER, i3 INTEGER, i4 INTEGER, i5 INTEGER, i6 INTEGER, "
		"i7 INTEGER, i8 INTEGER, i9 INTEGER, i10 INTEGER, i11 INTEGER, i12 INTEGER, i13 INTEGER,"
		"i14 INTEGER, i15 INTEGER, i16 INTEGER, i17 INTEGER, i18 INTEGER, i19 INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateTuplesTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateTuplesTable()"); }
}


void PostgreSQLTest::recreateNullableIntTable()
{
	dropTable("NullableIntTest");
	try { 
		*_pSession << "CREATE TABLE NullableIntTest (Id INTEGER, Value INTEGER)", now;
	}
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateNullableIntTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateNullableIntTable()"); }
}


void PostgreSQLTest::recreateNullableStringTable()
{
	dropTable("NullableStringTest");
	try { 
		*_pSession << "CREATE TABLE NullableStringTest (Id INTEGER, Address VARCHAR(30), Age INTEGER)", now;
	}
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateNullableStringTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateNullableStringTable()"); }
}


void PostgreSQLTest::recreateVectorsTable()
{
	dropTable("Vectors");
	try { *_pSession << "CREATE TABLE Vectors (i0 INTEGER, flt0 FLOAT, str0 VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.displayText() << std::endl; fail ("recreateVectorsTable()"); }
	catch(StatementException& se){ std::cout << se.displayText() << std::endl; fail ("recreateVectorsTable()"); }
}


void PostgreSQLTest::setUp()
{
}


void PostgreSQLTest::tearDown()
{
	dropTable("Person");
	dropTable("Strings");
}


CppUnit::Test* PostgreSQLTest::suite()
{
	PostgreSQL::Connector::registerConnector();

	_dbConnString += "host=" + getHost();
	_dbConnString += " user=" + getUser();
	_dbConnString += " password=" + getPassword();
	_dbConnString += " dbname=" + getBase();
	_dbConnString += " port=" + getPort();

	try
	{
		_pSession = new Session(PostgreSQL::Connector::KEY, _dbConnString);
	}
	catch (ConnectionFailedException& ex)
	{
		std::cout << ex.displayText() << std::endl;
		return 0;
	}

	std::cout << "*** Connected to [" << "PostgreSQL" << "] test database." << std::endl;
	dbInfo(*_pSession);

	_pExecutor = new SQLExecutor("PostgreSQL SQL Executor", _pSession);

	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("PostgreSQLTest");

	CppUnit_addTest(pSuite, PostgreSQLTest, testConnectNoDB);
	CppUnit_addTest(pSuite, PostgreSQLTest, testPostgreSQLOIDs);
	//CppUnit_addTest(pSuite, PostgreSQLTest, testBarebonePostgreSQL);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSimpleAccess);
	CppUnit_addTest(pSuite, PostgreSQLTest, testComplexType);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSimpleAccessVector);
	CppUnit_addTest(pSuite, PostgreSQLTest, testComplexTypeVector);
	CppUnit_addTest(pSuite, PostgreSQLTest, testInsertVector);
	CppUnit_addTest(pSuite, PostgreSQLTest, testInsertEmptyVector);
	CppUnit_addTest(pSuite, PostgreSQLTest, testInsertSingleBulk);
	CppUnit_addTest(pSuite, PostgreSQLTest, testInsertSingleBulkVec);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLimit);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLimitOnce);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLimitPrepare);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLimitZero);
	CppUnit_addTest(pSuite, PostgreSQLTest, testPrepare);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSetSimple);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSetComplex);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSetComplexUnique);
	CppUnit_addTest(pSuite, PostgreSQLTest, testMultiSetSimple);
	CppUnit_addTest(pSuite, PostgreSQLTest, testMultiSetComplex);
	CppUnit_addTest(pSuite, PostgreSQLTest, testMapComplex);
	CppUnit_addTest(pSuite, PostgreSQLTest, testMapComplexUnique);
	CppUnit_addTest(pSuite, PostgreSQLTest, testMultiMapComplex);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSelectIntoSingle);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSelectIntoSingleStep);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSelectIntoSingleFail);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLowerLimitOk);
	CppUnit_addTest(pSuite, PostgreSQLTest, testLowerLimitFail);
	CppUnit_addTest(pSuite, PostgreSQLTest, testCombinedLimits);
	CppUnit_addTest(pSuite, PostgreSQLTest, testCombinedIllegalLimits);
	CppUnit_addTest(pSuite, PostgreSQLTest, testRange);
	CppUnit_addTest(pSuite, PostgreSQLTest, testIllegalRange);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSingleSelect);
	CppUnit_addTest(pSuite, PostgreSQLTest, testEmptyDB);
	CppUnit_addTest(pSuite, PostgreSQLTest, testDateTime);
	//CppUnit_addTest(pSuite, PostgreSQLTest, testBLOB);
	CppUnit_addTest(pSuite, PostgreSQLTest, testCLOBStmt);
	CppUnit_addTest(pSuite, PostgreSQLTest, testBLOBStmt);
	CppUnit_addTest(pSuite, PostgreSQLTest, testUnsignedInts);
	CppUnit_addTest(pSuite, PostgreSQLTest, testFloat);
	CppUnit_addTest(pSuite, PostgreSQLTest, testDouble);
	CppUnit_addTest(pSuite, PostgreSQLTest, testTuple);
	CppUnit_addTest(pSuite, PostgreSQLTest, testTupleVector);
	CppUnit_addTest(pSuite, PostgreSQLTest, testInternalExtraction);
	CppUnit_addTest(pSuite, PostgreSQLTest, testNull);
	CppUnit_addTest(pSuite, PostgreSQLTest, testNullableInt);
	CppUnit_addTest(pSuite, PostgreSQLTest, testNullableString);
	CppUnit_addTest(pSuite, PostgreSQLTest, testTupleWithNullable);
	CppUnit_addTest(pSuite, PostgreSQLTest, testSessionTransaction);
	CppUnit_addTest(pSuite, PostgreSQLTest, testTransaction);
	CppUnit_addTest(pSuite, PostgreSQLTest, testReconnect);

	return pSuite;
}
