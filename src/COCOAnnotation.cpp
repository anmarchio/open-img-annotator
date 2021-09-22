#include "pch.h"
#include "COCOAnnotation.h"

void COCOAnnotation::getBboxAndAreaFromPolygon(wxImage image, wxPointList* points, vector<double>* bbox, double* area)
{	
	std::vector<cv::Point> shape;
	for (size_t i = 0; i < points->GetCount(); i++)
	{
		shape.push_back(Point(points->Item(i)->GetData()->x, points->Item(i)->GetData()->y));
	}
	Rect rect = cv::boundingRect(shape);
	
	bbox->push_back(double(rect.x));
	bbox->push_back(double(rect.y));
	bbox->push_back(double(rect.width));
	bbox->push_back(double(rect.height));

	*area = float(rect.area());
}

/**
	Area of Polygon
	Computing using the shoelace formula:
	https://en.wikipedia.org/wiki/Shoelace_formula
	Implementation has been performed 
	as proposed by 
	https://www.geeksforgeeks.org/area-of-a-polygon-with-given-n-ordered-vertices/
*/
double COCOAnnotation::getAreaFromPolygon(wxPointList* points)
{
	double area = 0.0;
	for (size_t i = 0; i < points->GetCount(); i++)
	{
		points->Item(i)->GetData()->x;
		points->Item(i)->GetData()->y;
	}
	// Calculate value of shoelace formula
	int j = points->GetCount() - 1;
	for (int i = 0; i < points->GetCount(); i++)
	{
		area += 
			(points->Item(j)->GetData()->x + 
			points->Item(i)->GetData()->x) * 
			(points->Item(j)->GetData()->y - 
			points->Item(i)->GetData()->y);
		j = i;  // j is previous vertex to i
	}

	// Return absolute value
	return abs(area / 2.0);
}

COCOAnnotation::COCOAnnotation()
{
	project_info = {
	 0,					// year
	 "0",				// version
	 "my-project-name", // description
	 "",				// contributor
	 "",				// url
	 0,					// date_created
	};
	/** 
		Default category;
		since category cannot be assigned 
		to images for reasons of simplicity,
		it will be assigned with a default
		value for reasons of compatiblity.
	*/
	category cat = {
		1,			// id
		"default",  // name
	};
	std::vector<category>::iterator it = categories.begin();
	it = categories.insert(it, cat);
}

string COCOAnnotation::toJson(wxImage currentImage, string imagePath, vector<PolygonShape> polygons)
{
	image img = {
		1,							// id
		currentImage.IsOk() ? currentImage.GetWidth() : 0,	// width
		currentImage.IsOk() ? currentImage.GetHeight() : 0,	// height
		imagePath					// file_name
	};

	vector<annotation>::iterator it_anno = annotations.begin();
	for (size_t i = 0; i < polygons.size(); i++)
	{
		annotation anno = {
			i,					// id
			0,					// iscrowd
			img.id,				// image_id
			categories.at(0).id	// category_id
		};

		vector<double> seg;
		for (size_t j = 0; j < polygons[i].points->GetCount(); j++)
		{
			seg.push_back(double(polygons[i].points->Item(j)->GetData()->x));
			seg.push_back(double(polygons[i].points->Item(j)->GetData()->y));
		}
		anno.segmentation = seg;
				
		getBboxAndAreaFromPolygon(currentImage, polygons[i].points, &anno.bbox, &anno.area);

		it_anno = annotations.insert(it_anno, anno);
	}

	/**
	COCO data format as given in
	https://cocodataset.org/#format-data

	{
		"info": info,
		"images": [image],
		"annotations": [annotation],
		"licenses": [license],
	}

	info{
		"year": int,
		"version": str,
		"description": str,
		"contributor": str,
		"url": str,
		"date_created": datetime,
	}

	image{
		"id": int,
		"width": int,
		"height": int,
		"file_name": str,
		"license": int,
		"flickr_url": str,
		"coco_url": str,
		"date_captured": datetime,
	}

	license{
		"id": int,
		"name": str,
		"url": str,
	}
	*/
	string json = "";

	// file start, info and image tag
	json = json + "{" +
		"\"info\": {" +
		"\"description\":\"" + project_info.description + "\"" +
		"}," +
		"\"images\": [{" +
		"\"id\":" + to_string(img.id) + "," +
		"\"width\":" + to_string(img.width) + "," +
		"\"height\":" + to_string(img.height) + "," +
		"\"file_name\":\"" + img.file_name + "\"" +
		"}],";

	// add annotations array
	json = json + "\"annotations\": [";
	for (size_t i = 0; i < annotations.size(); i++)
	{
		json = json + "{"
			"\"id\":" + to_string(i) + "," +
			"\"iscrowd\":" + to_string(annotations.at(i).iscrowd) + "," +
			"\"image_id\":" + to_string(annotations.at(i).image_id) + "," +
			"\"category_id\":\"" + to_string(annotations.at(i).category_id) + "\",";

		json = json + "\"segmentation\":[[";
		for(size_t j = 0; j < annotations.at(i).segmentation.size(); j++)
		{ 
			json = json + to_string(annotations.at(i).segmentation.at(j));
			if (j < annotations.at(i).segmentation.size() - 1) {
				json = json + ",";
			}
		}
		json = json + "]],";


		json = json + "\"bbox\":[";
		json = json + to_string(annotations.at(i).bbox.at(0)) + ",";
		json = json + to_string(annotations.at(i).bbox.at(1)) + ",";
		json = json + to_string(annotations.at(i).bbox.at(2)) + ",";
		json = json + to_string(annotations.at(i).bbox.at(3));
		json = json + "],";

		json = json + "\"area\":" + to_string(annotations.at(i).area) +
			"}";
	}
	json = json + "],";

	// add categories array
	json = json + "\"categories\": [{"
		"\"id\":" + to_string(categories.at(0).id) + "," +
		"\"name\":\"" + categories.at(0).name + "\"" + 
		"}]" +
		"}";
	return json;
}

void COCOAnnotation::deserializeFromJson()
{
	cout << "Not implemented.";
}
