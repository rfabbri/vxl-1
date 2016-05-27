#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>

#include "dbdet_filter_util.h"
#include "dbdet_filter_bank.h"

Filter2d::Filter2d(const char * fname) {
	m = vnl_matrix<double>(1,1);
        bool load = loadFromTabSpaced(fname, m);
	assert(load && m.cols() % 2 == 1 && m.rows() % 2 == 1);
}

bool Filter2d::isEmpty()
{
	return (m.cols() == 0 && m.rows() == 0);
}

int Filter2d::size()
{
	return m.cols();
}

double Filter2d::flipped(int i, int j)
{
	return m(m.rows() - i - 1, m.cols() - j - 1);
}

vnl_matrix<double> Filter2d::applyPadded(vnl_matrix<double> image, int border)
{
	vnl_matrix<double> ret = vnl_matrix<double>(image.rows() - 2 * border, image.cols() - 2 * border);

	int halfC = m.cols() / 2;
	int halfR = m.rows() / 2;

	for (int i = 0; i < ret.rows(); ++i)
	{
		int ib = i + border;
		for (int j = 0; j < ret.cols(); ++j)
		{
			int jb = j + border;
			ret(i, j) = 0.0;
			for (int ii = -halfR; ii <= halfR; ++ii)
			{
				for (int jj = -halfC; jj <= halfC; ++jj)
				{
					ret(i, j) += image(ib + ii, jb + jj) * flipped(ii + halfR, jj + halfC);
				}
			}
		}
	}
	return ret;
}

vnl_matrix<double> Filter2d::applyPadded13(vnl_matrix<double> image, int border)
{
	vnl_matrix<double> ret = vnl_matrix<double>(image.rows() - 2 * border, image.cols() - 2 * border);

	const int halfC = 6;
	const int halfR = 6;

	for (int i = 0; i < ret.rows(); ++i)
	{
		int ib = i + border;
		for (int j = 0; j < ret.cols(); ++j)
		{
			int jb = j + border;
			ret(i, j) = 0.0;
			for (int ii = -halfR; ii <= halfR; ++ii)
			{
				for (int jj = -halfC; jj <= halfC; ++jj)
				{
					ret(i, j) += image(ib + ii, jb + jj) * flipped(ii + halfR, jj + halfC);
				}
			}
		}
	}
	return ret;
}

vnl_matrix<double> Filter2d::applyPadded19(vnl_matrix<double> image, int border)
{
	vnl_matrix<double> ret = vnl_matrix<double>(image.rows() - 2 * border, image.cols() - 2 * border);

	const int halfC = 9;
	const int halfR = 9;

	for (int i = 0; i < ret.rows(); ++i)
	{
		int ib = i + border;
		for (int j = 0; j < ret.cols(); ++j)
		{
			int jb = j + border;
			ret(i, j) = 0.0;
			for (int ii = -halfR; ii <= halfR; ++ii)
			{
				for (int jj = -halfC; jj <= halfC; ++jj)
				{
					ret(i, j) += image(ib + ii, jb + jj) * flipped(ii + halfR, jj + halfC);
				}
			}
		}
	}
	return ret;
}




FilterBank::FilterBank(vcl_string baseDir)
{
	filtersMaxSize = 0;
	vcl_ifstream filterFiles((baseDir + "/filter_list.txt").c_str());
	if (filterFiles.good())
	{
		vcl_string line;
		while (!filterFiles.eof())
		{
			vcl_getline(filterFiles, line);
			if(line.length() > 0) 
			{
				line = baseDir + "/" + line;
				Filter2d f = Filter2d(line.c_str());
				if (!f.isEmpty())
				{
					filters.push_back(f);
					filtersMaxSize = f.size() > filtersMaxSize ? f.size() : filtersMaxSize;
				}
			}
		}
	}
}

int FilterBank::numFilters()
{
	return filters.size();
}

vcl_vector<vnl_matrix<double> > FilterBank::decompose(vnl_matrix<double> image)
{
	vcl_vector<vnl_matrix<double> > ret;
	ret.resize(filters.size());
	int padSize = filtersMaxSize / 2;
	vnl_matrix<double> padded = padReflect(image, padSize);

	for (int i = 0; i < filters.size(); ++i)
	{
		switch (filters[i].size())
		{
		case 13:
			ret[i] = filters[i].applyPadded13(padded, padSize);
			break;
		case 19:
			ret[i] = filters[i].applyPadded19(padded, padSize);
			break;
		default:
			ret[i] = filters[i].applyPadded(padded, padSize);
		}
	}
	return ret;
}
