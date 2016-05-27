#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>

#ifndef _DBDET_TEXTON_CLASSIFIER_H_
#define _DBDET_TEXTON_CLASSIFIER_H_

class TextonClassifier {

	vnl_matrix<double> classes;
public:

	TextonClassifier(const char * fileName);

	int numClasses();

	vnl_matrix<int> classify(vcl_vector<vnl_matrix<double> > decomposed);
};

#endif
