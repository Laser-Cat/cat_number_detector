#include "cat_head.h"

double euclid_distance(Vec3b a, Vec3b b)
{
    double ans=0;
    double tmp;
    for(int i=0;i<3;i++)
    {
        tmp=a[i]-b[i];
        tmp*=tmp;
        ans+=tmp;
    }
    return sqrt(ans);
}

Mat global_data::heat_region;
Mat global_data::foreground_mask;
vector<region_define> global_data::region_list;
Mat global_data::visit;
