#include"cat_head.h"


void make_index()
{
    FILE* fp=fopen("tmp.txt","w+");
    fclose(fp);
    ///
    ///for linux
    system("ls|grep jpg > tmp.txt");

}
void build_global_data()
{

    global_data::heat_region=imread("heat_mask.png",0);
    fopen("shit","w+");
    if(fopen("back_ground_sample_mask.bmp","r"))
    {
        cout<<"it should be there...";
    }
    global_data::foreground_mask=imread("back_ground_sample_mask_.bmp",0);
    global_data::read_region_define();

}
void make_mask(string image_name)
{
    input_data input;
    processed_data output;
    string mask_post_fix="_mask.bmp";
    input.color_image=imread(image_name.c_str());
    cvtColor(input.color_image,input.gray_scale_image,CV_BGR2GRAY);
    Mat mask(input.color_image.size(),CV_8UC1,Scalar(0));

    for(region_define & def:global_data::region_list)
    {
        region r;
        region_factory::build_region(r,def,input.color_image);
        r.binarylize_with_k_means(input.color_image);
        r.add_to_mask(mask);
        output.regions.push_back(r);
    }
    output.reducted_mask=mask;
    imshow("bar",output.reducted_mask);
    image_name+=mask_post_fix;
    imwrite(image_name.c_str(),output.reducted_mask);
}

void clean_up()
{
    system("rm *_mask.bmp");

}
void pack_up()
{
    system("mkdir output");
    system("mv *_mask.bmp output");

}
int main()
{
    clean_up();
    build_global_data();
    make_index();


    string name;
    FILE* tmp_fp=fopen("tmp.txt","r");
    char buf[10086];
    while(fscanf(tmp_fp,"%s",buf)==1)
    {
        name=buf;
        make_mask(name);
    }
    fclose(tmp_fp);
    pack_up();
    return 0;
}

