#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

#ifndef _DBDET_FILTER_UTIL_H_
#define _DBDET_FILTER_UTIL_H_

template<typename T>
bool loadFromTabSpaced(const char * fileName, vnl_matrix<T> & out)
{
	vcl_ifstream in(fileName);
        vcl_vector<T> tempData;
        int nRow = 0, nCol = 0;
	if (in.good()) {
		while (!in.eof())
		{
			vcl_string line;
			vcl_getline(in, line);
			vcl_istringstream ss(line);
			int cols = 0;
			T val;
			while (ss >> val) {
				tempData.push_back(val);
                                cols++;
			}
                        if(cols > 0)
                        {
				nRow++;
                                nCol = cols;
                        }
		}
		in.close();
		if(nRow * nCol == tempData.size())
		{
			out.set_size(nRow, nCol);
		        out.set(&tempData[0]);
			return true;
		}		
	}
        else
        {
               return false;
        }
}

template<typename T>
vnl_matrix<T> padReflect(vnl_matrix<T> m, int border)
{
	vnl_matrix<T> ret = vnl_matrix<T>(m.rows() + 2 * border, m.cols() + 2 * border);
	border--;

	for (int i = 0; i < ret.rows(); ++i)
	{
		int ii = i <= border ? border - i : i - border - 1;
		ii = ii < m.rows() ? ii : 2 * m.rows() - ii - 1;
		for (int j = 0; j < ret.cols(); ++j)
		{
			int jj = j <= border ? border - j : j - border - 1;
			jj = jj < m.rows() ? jj : 2 * m.cols() - jj - 1;
			ret(i, j) = m(ii, jj);
		}
	}
	return ret;
}
#endif
