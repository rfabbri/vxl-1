//:
//\file
//\brief File for me to experiment with dbpro.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//
#include <testlib/testlib_test.h>
#include <dbpro/dbpro_process.h>
#include <vcl_iostream.h>
#include <dbpro/dbpro_observer.h>
#include <dbpro/tests/dbpro_sample_processes.h>
#include <vcl_vector.h>

//: The simplest possible source; just returns what it holds. Each call to
// execute just returns the same thing.
template <class T>
class dbpro_single_input : public dbpro_source
{
 public:
  dbpro_single_input(T d) : data(d) {}

  //: Execute the process
  dbpro_signal execute()
  {
    output(0, data);
    return DBPRO_VALID;
  }

  T data;
};


template <class T>
class dbpro_concat : public dbpro_sink
{
 public:
  dbpro_concat(unsigned n) : n_(n), data() { data.reserve(n);}

  //: Execute the process
  dbpro_signal execute()
  {
    for (unsigned i=0; i < n_; ++i) {
      assert(input_type_id(i) == typeid(T));
      data.push_back(input<T>(i));
    }
    return DBPRO_VALID;
  }

  unsigned n_; 
  vcl_vector<T> data;
};

static dbpro_mutex print_mutex;

template <class T>
class dbpro_sum_expensive : public dbpro_filter
{
 public:
  dbpro_sum_expensive(unsigned n_iter) : n_it_(n_iter) {}
  //: Execute the process
  dbpro_signal execute()
  {
    assert(input_type_id(0) == typeid(T));
    assert(input_type_id(1) == typeid(T));
    T val1 = input<T>(0);
    T val2 = input<T>(1);

    print_mutex.lock();
    vcl_cout << "Thread entered it = " << n_it_ << vcl_endl;
    print_mutex.unlock();
    for (unsigned i=0; i < n_it_; ++i)
      for (unsigned j=0; j < 100; ++j)
        for (unsigned k=0; k < 100; ++k) {
          double a = vcl_sin((double)i/(double)j* (double)k);
          a++;
        }
    print_mutex.lock();
    vcl_cout << "Thread exited it = " << n_it_ << vcl_endl;
    print_mutex.unlock();

    output(0, T(val1+val2));
    return DBPRO_VALID;
  }

  unsigned n_it_;
};

MAIN( test_dbpro_process )
{
  START ("ric's dbpro process experiments");

  vcl_vector<int> data;
  data.push_back(4);
  data.push_back(5);
  data.push_back(10);
  data.push_back(3);
  data.push_back(6);


  assert(!data.empty());
  vcl_vector<dbpro_process_sptr> sources;
  sources.reserve(data.size());

  vcl_vector<dbpro_process_sptr> sums;
  sums.reserve(data.size()-1);

  
  dbpro_concat<int> *out_ptr = new dbpro_concat<int>(data.size()-1);
  dbpro_process_sptr out(out_ptr);

  for (unsigned i=0; i < data.size(); ++i) {
    // Create source processes that just return an int.
    sources.push_back(new dbpro_single_input<int>(data[i]));

    if (i > 0) {
      // Connect each source to a process that sums the value with the previous
      sums.push_back(new dbpro_sum_expensive<int>((data.size()-i+1)*1000));
      sums.back()->connect_input(0,sources[i-1],0);
      sums.back()->connect_input(1,sources[i],0);
      out->connect_input(i-1, sums.back(), 0);
      // connect the output of each sum to a sink process that puts them all
      // into a single vector.
    }
  }

  out->run(1);

  vcl_cout << "Resulting data output\n";
  for(unsigned int i=0; i<out_ptr->data.size(); ++i)
    vcl_cout << out_ptr->data[i] << " ";
  vcl_cout << vcl_endl;

  SUMMARY();
}
