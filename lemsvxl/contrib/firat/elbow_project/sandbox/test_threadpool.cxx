/*
 * test_threadpool.cxx
 *
 *  Created on: Oct 16, 2011
 *      Author: firat
 */

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/threadpool.hpp>

using namespace std;
using namespace boost::threadpool;

//
// Helpers
boost::mutex m_io_monitor;

void print(vcl_string text)
{
  boost::mutex::scoped_lock lock(m_io_monitor);
  vcl_cout << text << vcl_endl;
}

void task(int a)
{
	for(int i = 0; i < a*10000; i++)
	{
		5*5;
	}
	vcl_string x = "q";
	x[0] = a + '0';
	print(x);
}

int main()
{
	pool tp(2);
	for(int a = 9; a >= 0; a--)
	{
		tp.schedule(boost::bind(task, a));
	}
}



