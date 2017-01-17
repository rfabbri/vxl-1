
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_base.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_ihs_feature.h>

dbinfo_feature_base* dbinfo_feature_base::
feature_base_new()
{
  const dbinfo_intensity_feature* intf = this->cast_to_intensity_feature();
  const dbinfo_gradient_feature* gradf = this->cast_to_gradient_feature();
  const dbinfo_ihs_feature* ihsf = this->cast_to_ihs_feature();
  if(intf)
    return new dbinfo_intensity_feature();
  if(gradf)
    return new dbinfo_gradient_feature();
  if(ihsf)
    return new dbinfo_ihs_feature();
  return 0;
}


void dbinfo_feature_base::print(vcl_ostream& os) const
{
  os << this->is_a() << " [\n"
     << "margin " << margin_ << '\n';
  if(data_)
    data_->print(os);
  os << "]\n";
}


// The serialization can be done here in the base class, since for now   
// the sub-classes have no independent data of their own, just 
// more specialized structure

//: Binary save self to stream.
void dbinfo_feature_base::b_write(vsl_b_ostream &os) const
{
  dbinfo_feature_base& fb = (dbinfo_feature_base&)*this;  
  //Must have data
  assert(fb.data());
  vsl_b_write(os, fb.version());
  vsl_b_write(os, static_cast<int>(fb.format()));
  vsl_b_write(os, fb.margin());
  dbinfo_feature_data_base* fd = fb.data().ptr();
  vsl_b_write(os, fd);
}

//: Binary load self from stream.
void dbinfo_feature_base::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        int junk;
        vsl_b_read(is, junk);
        format_ = static_cast<dbinfo_feature_format>(junk);
        vsl_b_read(is, margin_);
        dbinfo_feature_data_base* fd=(dbinfo_feature_data_base*)0;
        vsl_b_read(is, fd);
        assert(fd);
        data_ = fd;
      }
    }
}
