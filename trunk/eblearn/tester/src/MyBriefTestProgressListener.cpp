#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>


CPPUNIT_NS_BEGIN


BriefTestProgressListener::BriefTestProgressListener()
    : m_lastTestFailed( false )
{
}


BriefTestProgressListener::~BriefTestProgressListener()
{
}


void 
BriefTestProgressListener::startTest( Test *test )
{
  stdCOut() << test->getName();
  stdCOut().flush();

  m_lastTestFailed = false;
}


void 
BriefTestProgressListener::addFailure( const TestFailure &failure )
{
  stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
  m_lastTestFailed  = true;
}


void 
BriefTestProgressListener::endTest( Test *test )
{
  if ( !m_lastTestFailed )
    stdCOut()  <<  " : \033[1;32m[OK]\033[0m";
  else 
    stdCOut()  <<  " \033[1;31m[Failed]\033[0m";
  stdCOut() << "\n";
}


CPPUNIT_NS_END

