#include "pch.h"
#include "COCOAnnotation.h"

void COCOAnnotation::getBboxAndAreaFromPolygon(wxImage image, wxPointList* points, vector<double>* bbox, double* area)
{	
	Mat tmpImg = Mat(image.GetHeight(), image.GetWidth(), CV_8UC1);
	for (size_t i = 0; i < points->GetCount(); i++)
	{
		tmpImg.at<Vec3b>(Point(points->Item(i)->GetData()->x, points->Item(i)->GetData()->y)) = Vec3b(255, 255, 255);
	}
	Rect rect = boundingRect(tmpImg);
	
	vector<double> bb;
	vector<double>::iterator it_bb = bb.begin();
	bb.insert(it_bb, double(rect.x));
	bb.insert(it_bb, double(rect.y));
	bb.insert(it_bb, double(rect.width));
	bb.insert(it_bb, double(rect.height));
	bbox = &bb;

	*area = rect.area * 1.0;
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

void COCOAnnotation::toJson(wxImage currentImage, string imagePath, vector<PolygonShape> polygons)
{
	image img = {
		1,							// id
		currentImage.GetWidth(),	// width
		currentImage.GetHeight(),	// height
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
		vector<double>::iterator it_seg = seg.begin();
		for (size_t j = 0; j < polygons[i].points->GetCount(); j++)
		{
			seg.insert(it_seg, polygons[i].points->Item(j)->GetData()->x);
			seg.insert(it_seg, polygons[i].points->Item(j)->GetData()->y);
		}
		anno.segmentation = seg;
				
		getBboxAndAreaFromPolygon(currentImage, polygons[i].points, &anno.bbox, &anno.area);

		it_anno = annotations.insert(it_anno, anno);
	}
}

void COCOAnnotation::deserializeFromJson()
{
	cout << "Not implemented.";
}
