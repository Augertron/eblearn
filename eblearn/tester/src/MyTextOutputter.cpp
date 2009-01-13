#include "MyTextOutputter.h"
#include <cppunit/TestResultCollector.h>

using namespace CppUnit;

MyTextOutputter::MyTextOutputter( TestResultCollector *result,
                              OStream &stream )
  : TextOutputter(result, stream)
{
}

MyTextOutputter::~MyTextOutputter()
{
}

void 
MyTextOutputter::printHeader()
{
  if ( m_result->wasSuccessful() ) {
    m_stream << "\033[1;32m[OK]\033[0m  (";
    m_stream << m_result->runTests () << " tests)" ;
  } else {
    //    printFailureWarning();
    printStatistics();
  }
}

void 
MyTextOutputter::printStatistics()
{
  m_stream << "\033[1;32m[OK:\033[0m ";
  m_stream <<  m_result->runTests() 
    - m_result->testFailures() - m_result->testErrors();
  m_stream << "\033[1;32m]\033[0m ";
  m_stream << " \033[1;31m[Failed: \033[0m"  <<  m_result->testFailures();
  m_stream << "\033[1;31m]\033[0m ";
  m_stream << " \033[1;31m[Errors:\033[0m "  <<  m_result->testErrors();
  m_stream << "\033[1;31m]\033[0m ";
  m_stream  <<  "\n";
}

void 
MyTextOutputter::printFailureWarning()
{
  m_stream  << "\033[1;31m!!!FAILURES!!!\033[0m\n";
}
