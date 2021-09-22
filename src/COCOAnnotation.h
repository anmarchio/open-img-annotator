#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>

#include "PolygonShape.h"

using namespace std;
using namespace cv;

/*!
	COCOAnnotation allows to create an 
	object according to the COCO data 
	format as defined in
	https://cocodataset.org/#format-data
*/
class COCOAnnotation
{
	/** info struct
	 *  In this project, only the description 
		data tag will be filled.
	 */
	struct info {
		int year;
		string version;
		string description;
		string contributor;
		string url;
		int date_created;
	};

	/** image struct
	 *  as officially defined.
		Only the most relevant properties
		will be effectively used.
	 */
	struct image {
		int id;
		int width;
		int height;
		string file_name;
		int license;
		string flickr_url;
		string coco_url;
		int date_captured;
	};

	/** annotation struct
	 *  The variables should be filled
		with data in order to ensure
		compatibility with external software.
	 */
	struct annotation {
		int id;
		bool iscrowd;
		int image_id;
		int category_id;
		vector<double> segmentation;
		vector<double> bbox;
		double area;
	};

	/** category struct
	 *  This program only uses plain
		polygon annotation and does not
		explicitely identify categories.
		Therefore categories will be filled
		with default values.
	 */
	struct category {
		int id;
		string name;
	};

	/** license struct
	 *  This is included for the
		sake of copmleteness 
		but never used.
	 */
	struct license {
		int id;
		string name;
		string url;
	};

	info project_info;
	vector<image> images;
	vector<annotation> annotations;
	vector<category> categories;

private:
	double getAreaFromPolygon(wxPointList* points);
	void getBboxAndAreaFromPolygon(wxImage image, wxPointList* points, vector<double>* bbox, double* area);

public:
	COCOAnnotation();

	string toJson(wxImage currentImage, string imagePath, vector<PolygonShape> polygons);
	void deserializeFromJson();
};