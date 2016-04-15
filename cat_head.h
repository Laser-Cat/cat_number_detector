#ifndef CAT_HEAD_H
#define CAT_HEAD_H
#include <iostream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/ml/ml.hpp>
#include<opencv2/opencv.hpp>
#include<bits/stdc++.h>

using namespace std;
using namespace cv;

typedef vector<int > vi;
typedef vector<Point> connected_component;
typedef vector<connected_component> connected_components;
typedef vector<float> vf;



double euclid_distance(Vec3b a, Vec3b b);

struct region_define
{
    Rect area;
    string title;
    vector<Rect> foreground_candidate_areas;
    vector<Point> background_points;
};
struct global_data
{
protected:
    static Mat visit;
public:
    static Mat heat_region;
    static Mat foreground_mask;

    static vector<region_define> region_list;

    static bool is_ok(Point p)
    {
        if(p.x<0||p.y<0)
        {
            return false;
        }
        if(p.x>=heat_region.cols||p.y>=heat_region.rows)
        {
            return false;
        }
        return true;
    }
    static Rect bfs(Point start_point,Mat & visit,uchar VISITED)
    {
        vector<Point> points;

        if(visit.at<uchar>(start_point)==VISITED)
        {
            return Rect(0,0,0,0);
        }
        region_define tmpdef;

        int dir_x[]={-1,1,0,0};
        int dir_y[]={0,0,-1,1};

        points.push_back(start_point);
        visit.at<uchar>(start_point)=VISITED;
        queue<Point >q;
        q.push(start_point);
        Point current,next;
        while (!q.empty())
        {
            current=q.front();
            q.pop();
            for(int i=0;i<4;i++)
            {
                next=current;
                next.x+=dir_x[i];
                next.y+=dir_y[i];
                if(!is_ok(next))
                {
                    continue;
                }
                if(visit.at<uchar>(next)==VISITED)
                {
                    continue;
                }

                visit.at<uchar>(next)=VISITED;
                q.push(next);
                points.push_back(next);
            }
        }
        return boundingRect(points);
    }

    static void read_region_define()
    {
        visit=heat_region.clone();
        imshow("bar",visit);
        waitKey(0);
        for(int x=0;x<heat_region.cols;x++)
        {
            for (int y = 0; y < heat_region.rows; ++y)
            {
                Point current(x,y);
                if(heat_region.at<uchar>(current)==255)
                {
                    region_define def;
                    def.area=bfs(current,visit,0);
                    if(def.area.area()==0)
                    {
                        continue;
                    }
                    def.title="undefined";
                    region_list.push_back(def);
                }
            }
        }
        visit=foreground_mask.clone();
        for(region_define & def:region_list)
        {
            for(int x=def.area.tl().x;x<def.area.br().x;x++)
            {
                for (int y = def.area.tl().y;y<def.area.br().y; ++y)
                {
                    Point current(x,y);
                    if(foreground_mask.at<uchar>(current)==0)
                    {
                        Rect candidate;
                        candidate=bfs(current,visit,255);
                        def.foreground_candidate_areas.push_back(candidate);
                    }
                }
            }
        }

    }
    static void read_region_define(string config_file_name);
};

struct region
{
    region_define defination;
    Vec3f mean_bg_color;
    double max_euclidian_distance;
    double variance;
    int bg_pixel_count;
    connected_components the_texts;
    vector<Point> fg;
    vector<Point> bg;

    ///here we binarylize it with a k-means method based on assumption that the background color and foreground color 
    /// tends to be two seprate point sets on color space;
    void binarylize_with_k_means(Mat & color_image)
    {
        vector<Vec3f >colors;
        vector<Point> locs;
        vector<int > labels;
        
        for(int x=defination.area.tl().x;x<defination.area.br().x;x++)
        {
            for(int y=defination.area.tl().y;y<defination.area.br().y;y++)
            {
                Point current_point(x,y);
                locs.push_back(current_point);
                labels.push_back(0);
                Vec3f current_color;
                current_color[0]=color_image.at<Vec3b>(current_point)[0];
                current_color[1]=color_image.at<Vec3b>(current_point)[1];
                current_color[2]=color_image.at<Vec3b>(current_point)[2];
                colors.push_back(current_color);
            }
        }
        Mat sample(Size(3,colors.size()),CV_32FC1,Scalar(0));
        for(int i=0;i<colors.size();i++)
        {
            for(int j=0;j<3;j++)
            {
                sample.at<float>(Point(j,i))=colors[i][j];
            }
        }
        Mat color_center;
        kmeans(sample,2,labels,TermCriteria(),100,cv::KMEANS_PP_CENTERS,color_center);
        int ind=0;
        for(Point& p:locs)
        {
            if(labels[ind])
            {
                fg.push_back(p);
            }
            else {
                bg.push_back(p);
            }
            ind++;
        }
        Vec3f center[2];
        for(int i=0;i<2;i++)
        {
            for(int j=0;j<3;j++)
            {
                center[i][j]=color_center.at<float>(Point(j,i));
            }
        }

        if(euclid_distance(center[1],mean_bg_color)<euclid_distance(center[0],mean_bg_color))
        {
            fg.swap(bg);
        }
        the_texts.push_back(fg);
    }
    void binarylize_with_color()
    {
        
    }
    void add_to_mask(Mat & mask)
    {
        for(connected_component& c:the_texts)
        {
            for(Point& p:c)
            {
                mask.at<uchar> (p)=255;
            }
        }
    }

};

struct region_factory
{

    static void build_region(region& r,region_define &d,Mat& color_image)
    {
        sample_background(r,d,color_image);
        r.defination=d;
    }
protected:
    static void sample_background(region &r,region_define &d,Mat& color_image)
    {
        Vec3b bg_max;
        Vec3b bg_min;
        int bg_pixel_count=0;
        for(int x=d.area.tl().x;x<d.area.br().x;x++)
        {
            for(int y=d.area.tl().y;y<d.area.br().y;y++)
            {
                Point current(x,y);
                Vec3b current_color=color_image.at<Vec3b>(current);
                if(global_data::heat_region.at<uchar>(current)==0)
                {
                    continue;
                }
                for(int i=0;i<3;i++)
                {
                    r.mean_bg_color[i]+=current_color[i];
                    bg_max[i]=max(bg_max[i],current_color[i]);
                    bg_min[i]=min(bg_min[i],current_color[i]);
                }
                bg_pixel_count++;
            }
        }
        r.mean_bg_color[0]/=r.bg_pixel_count;
        r.mean_bg_color[1]/=r.bg_pixel_count;
        r.mean_bg_color[2]/=r.bg_pixel_count;
        r.max_euclidian_distance=euclid_distance(bg_max,bg_min);
    }
};

struct input_data
{
    Mat color_image;
    Mat gray_scale_image;
};
struct processed_data
{
    vector<region> regions;
    Mat reducted_mask;
    /// \brief components
    ///for mser and advanced functions
    connected_components components;
};
#endif // CAT_HEAD_H
