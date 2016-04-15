#ifndef CAT_CLASSIFIER_H
#define CAT_CLASSIFIER_H
#include"cat_head.h"

struct cat_classifier_result
{
    double confidence;
    char what;
};
///
/// \brief The cat_classifier class
///the plan: test 0-9 classifier and
/// if it looks like some number, keep
/// else drop
class cat_classifier
{
    Mat uniform_to_32_32(connected_component)
    {

    }
    //vf get_

public:
    void load();
    void save();
    void train();
    cat_classifier_result predict();
    cat_classifier();
};

#endif // CAT_CLASSIFIER_H
