#pragma once
#include "pch.h"

#include "COCOAnnotation.h"

using namespace std;

namespace OpenImgAnnotatorTest
{
	class OpenImgAnnotatorTester final
	{
	public:
		void TestCOCOAnnotation()
		{
			const string path = "samples/road_tiny.jpg";
			// load dummy image
			wxImage img;
			img.LoadFile(path, wxBITMAP_TYPE_ANY);

			// Create dummy polygon
			std::vector<PolygonShape> polygons;
			vector<PolygonShape>::iterator it = polygons.begin();

			PolygonShape* psh = new PolygonShape();
			psh->insertPoint(new wxPoint(10, 10));
			psh->insertPoint(new wxPoint(20, 10));
			psh->insertPoint(new wxPoint(20, 20));
			psh->insertPoint(new wxPoint(10, 20));
			polygons.push_back(*psh);

			// convert to json
			COCOAnnotation* coco = new COCOAnnotation();
			string output = coco->toJson(img, path, polygons);

			const string test = "{\"info\": {\"description\":\"my-project-name\"},\"images\": [{\"id\":1,\"width\":267,\"height\":402,\"file_name\":\"samples/road_tiny.jpg\"}],\"annotations\": [{\"id\":0,\"iscrowd\":0,\"image_id\":1,\"category_id\":\"1\",\"segmentation\":[[10.000000,10.000000,20.000000,10.000000,20.000000,20.000000,10.000000,20.000000]],\"bbox\":[0.000000,0.000000,267.000000,402.000000],\"area\":107334.000000}],\"categories\": [{\"id\":1,\"name\":\"default\"}]}";
			assert(output == test);
		}
	};
}