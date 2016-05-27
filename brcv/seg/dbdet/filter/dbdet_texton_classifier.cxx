#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_limits.h>
#include "dbdet_texton_classifier.h"
#include "dbdet_filter_util.h"

TextonClassifier::TextonClassifier(const char * fileName)
{
	classes = vnl_matrix<double>(1, 1);
        bool load = loadFromTabSpaced(fileName, classes);
	assert(load);
}

int TextonClassifier::numClasses()
{
	return classes.cols();
}

vnl_matrix<int> TextonClassifier::classify(vcl_vector<vnl_matrix<double> > decomposed)
{
	assert(decomposed.size() == classes.rows() && decomposed.size() > 0);

	int xs = decomposed[0].cols() * decomposed[0].rows();
	int ys = classes.cols();

	vnl_matrix<double> dec(decomposed.size(), xs);
	for (int i = 0; i < decomposed.size(); ++i)
	{
		dec.set_row(i, (decomposed[i]).data_block());
	}

	vcl_vector<double> x2(xs, 0.);
	vcl_vector<double> y2(ys, 0.);

	for (int i = 0; i < dec.rows(); ++i)
	{
		for (int j = 0; j < dec.cols(); ++j)
		{
			x2[j] += dec(i, j) * dec(i, j);
		}
	}

	for (int i = 0; i < classes.rows(); ++i)
	{
		for (int j = 0; j < classes.cols(); ++j)
		{
			y2[j] += classes(i, j) * classes(i, j);
		}
	}


	vnl_matrix<int> classified(decomposed[0].rows(), decomposed[0].cols());

	for (int i = 0; i < xs; ++i)
	{
		vcl_vector<double> d(ys, 0);
		int idxi = i / classified.rows();
		int idxj = i % classified.rows();

		for (int k = 0; k < dec.rows(); ++k)
		{
			for (int j = 0; j < ys; ++j)
			{
				d[j] += dec(k, i) * classes(k, j);
			}
		}
		double dist = vcl_numeric_limits<double>::max();
		for (int j = 0; j < ys; ++j)
		{
			double tmp = -2. * d[j] + x2[i] + y2[j];
			if (tmp < dist)
			{
				dist = tmp;
				classified(idxi, idxj) = j;
			}
		}
	}

	return classified;
}
