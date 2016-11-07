#ifndef dbseg_seg_object_txx_
#define dbseg_seg_object_txx_

#include <math.h>
#include "dbseg_seg_object.h"
//#include <vil/vil_save.h>

template <class T>
dbseg_seg_object<T>::dbseg_seg_object():
labeled_image(vil_image_view<int>()), image(vil_image_view<T>()) {
    tree=0;
    neighborhood=0;
    idCount=1;
    spine_parts.resize(10);
    spine_parts.assign(10, 0);

    disk_cornersX.resize(8);
    disk_cornersX.assign(8, 0);
    disk_cornersY.resize(8);
    disk_cornersY.assign(8, 0);

    disk_proX.resize(4);
    disk_proX.assign(4, 0);

    disk_proY.resize(4);
    disk_proY.assign(4, 0);

    disk_pro_polys.resize(4);
    for (int i = 0; i < 4; i++) {
        disk_pro_polys[i].new_sheet();
    }

    totalPixels = 0;

}

template <class T>
dbseg_seg_object<T>::dbseg_seg_object(vil_image_view<int> labeled, vil_image_view<T> original):
labeled_image(labeled), image(original)
{
    idCount=1;

    totalPixels = 0;
    initial_scan();
    vcl_cout << "Segmentation Structure Generation Complete" << vcl_endl;
     


}


//old version
template <class T>
dbseg_seg_object<T>::dbseg_seg_object(vil_image_view<int> labeled, 
            vil_image_view<T> original, 
            list<int> validL, 
            vector<dbseg_seg_spatial_object<T>*> objL, 
            vector<int> parentL, vector<list<int>> childL, 
            vector<list<int>> depthL,
            vector<list<int>> neighborL) : labeled_image(labeled), image(original)
{
    spine_parts.resize(10);
    spine_parts.assign(10,0);
    disk_cornersX.resize(8);
    disk_cornersX.assign(8, 0);
    disk_cornersY.resize(8);
    disk_cornersY.assign(8, 0);
    disk_pro_polys.resize(4);
    for (int i = 0; i < 4; i++) {
        disk_pro_polys[i].new_sheet();
    }

    disk_proX.resize(4);
    disk_proX.assign(4, 0);
    disk_proY.resize(4);
    disk_proY.assign(4, 0);

    idCount = validL.back() + 1;
    totalPixels = 0;
    neighborhood = new dbseg_seg_neighborhood(neighborL);
    tree = new dbseg_seg_tree<T>(validL, objL, parentL, childL, depthL);
    
}

template <class T>
dbseg_seg_object<T>::dbseg_seg_object(vil_image_view<int> labeled, 
            vil_image_view<T> original, 
            list<int> validL, 
            vector<dbseg_seg_spatial_object<T>*> objL, 
            vector<int> parentL, vector<list<int>> childL, 
            vector<list<int>> depthL,
            vector<list<int>> neighborL, 
            vector<int> spineP,
            vector<double> disk_cornersXL, 
            vector<double> disk_cornersYL) : labeled_image(labeled), image(original), spine_parts(spineP), disk_cornersX(disk_cornersXL), disk_cornersY(disk_cornersYL)
{
    idCount = validL.back() + 1;
    totalPixels = 0;
    
    disk_proX.resize(4);
    disk_proX.assign(4, 0);

    disk_proY.resize(4);
    disk_proY.assign(4, 0);

    disk_pro_polys.resize(4);
    for (int i = 0; i < 4; i++) {
        disk_pro_polys[i].new_sheet();
    }

    neighborhood = new dbseg_seg_neighborhood(neighborL);
    tree = new dbseg_seg_tree<T>(validL, objL, parentL, childL, depthL);


}

template <class T>
dbseg_seg_object<T>::~dbseg_seg_object() {
    delete tree;
    delete neighborhood;
}

//sets up the outline and the neighbors
template <class T>
void dbseg_seg_object<T>::trace(int o, TraceType type) {

    vcl_vector<vsol_point_2d_sptr> l;
    vcl_vector<vsol_point_2d_sptr>::iterator i2;

    int offsetX = tree->get_object_list()[o]->get_left();
    int offsetY = tree->get_object_list()[o]->get_top();

    set<int> traceBases;
    // if it's a base object
    if (tree->get_child_count(o) == 0) {
        traceBases.insert(o);
    }
    else {
        traceBases = tree->get_base_children(o);
    }

    //only do dbdet_contour on initial scan
    //if (type == INIT) {


    vil_image_view<bool>* binaryImage = new vil_image_view<bool>(tree->get_object_list()[o]->get_right()-offsetX+4, tree->get_object_list()[o]->get_bottom()-offsetY+4, 1);


    int tempDepth = tree->get_object_list()[o]->get_depth();
    //initialize to false
    for (int i = 0; i < binaryImage->ni(); i++) {
        for (int j = 0; j < binaryImage->nj(); j++) {
            (*binaryImage)(i, j) = false;
        }
    }
    
    //fill in values
    

    for (int i = (tree->get_object_list())[o]->get_left(); i <= (tree->get_object_list())[o]->get_right(); i++) {
        for (int j = (tree->get_object_list())[o]->get_top(); j <= (tree->get_object_list())[o]->get_bottom(); j++) {
            if (traceBases.find(labeled_image(i, j)) != traceBases.end()) {
                (*binaryImage)(i-offsetX+2,j-offsetY+2) = true;
                totalPixels++;
            
            }
            /* old way
            list<int>::iterator k;
            for (k = traceBases.begin(); k != traceBases.end(); ++k) {        
                if (labeled_image(i, j) == *k) {
                    (*binaryImage)(i-offsetX+2,j-offsetY+2) = true;
                    totalPixels++;
                }
            }*/
        }
    }
    //do the tracing
    dbdet_contour_tracer ctracer;

    //ctracer.set_sigma(s);
    ctracer.trace(*binaryImage);
    
    l = ctracer.largest_contour();

    tree->get_object_list()[o]->clear_outline();
    
    
    //create outline
    for (i2 = l.begin(); i2 != l.end(); ++i2) {
        tree->get_object_list()[o]->add_point((*i2)->x()+offsetX-2, (*i2)->y()+offsetY-2);

    }
    //}

    /*if (type == CREATE) {
        //create the outline in create_region()


    }*/
    //add neighbors
    set<int> tempNeighbors;
    if (type == INIT) {
        for (i2 = l.begin(); i2 != l.end(); ++i2) {
            for (int j = (int)(*i2)->x()+offsetX-3; j < (int)(*i2)->x()+offsetX; j++) {
                for (int k = (int)(*i2)->y()-3+offsetY; k < (int)(*i2)->y()+offsetY; k++) {
                    if (j != (int)(*i2)->x()-2+offsetX || (int)k != (*i2)->y()-2+offsetY) {
                        if (j > 0 && k > 0 && j < labeled_image.ni() && k < labeled_image.nj()) {
                            list<int>::iterator m;
                            bool lookOutside = true;
                            if (traceBases.find(labeled_image(j, k)) != traceBases.end()) {
                                lookOutside = false;
                            }
                            /*for (m = traceBases.begin(); m != traceBases.end(); ++m) {
                                if (labeled_image(j,k)==*m) {
                                    lookOutside = false;
                                }
                            }*/
                            if (lookOutside) {
                                tempNeighbors.insert(get_pixel_ID(j, k, tree->get_object_list()[o]->get_depth()));
                            }
                        }
                    }

                }
            }
            
        }
    }
    if (type == CREATE) {
        //set up neighbors in create_region()


    }
    //add inner contours if this is not the initial first scan
    if (type == CREATE || type == EDIT || type == RETRACE) {
        int k = o;
        int kL = tree->get_object_list()[k]->get_left();
        int kR = tree->get_object_list()[k]->get_right();
        int kT = tree->get_object_list()[k]->get_top();
        int kB = tree->get_object_list()[k]->get_bottom();
        vgl_polygon<double>* outerCont = tree->get_object_list()[k]->get_outline().front();
        list<int> tempIDList = tree->get_valid_IDs();
        list<int>::iterator q;
        for (q = tempIDList.begin(); q != tempIDList.end(); ++q) {
            //if it's not the same object and the large object doesn't already surround the small one and it's in the same depth level
            if (k != *q && !tree->get_object_list()[k]->surrounds(*q) && tree->get_object_list()[k]->get_depth() == tree->get_object_list()[*q]->get_depth()) {
                int qL = tree->get_object_list()[*q]->get_left();
                int qR = tree->get_object_list()[*q]->get_right();
                int qT = tree->get_object_list()[*q]->get_top();
                int qB = tree->get_object_list()[*q]->get_bottom();
                //if this region's bounding box is contained in the larger region's bounding box
                if (kL <= qL && kR >= qR && kT <= qT && kB >= qB) {
                    vgl_polygon<double>* innerCont = tree->get_object_list()[*q]->get_outline().front();
                    
                    bool containsEveryPoint = true;
                    for (int j = 0; j < (*innerCont)[0].size(); j++) {
                        if (!outerCont->contains((*innerCont)[0][j].x(), (*innerCont)[0][j].y())) {
                            containsEveryPoint = false;
                            break;
                        }
                    }
                    if (containsEveryPoint) {
                        tree->get_object_list()[k]->add_inner_outline(innerCont, *q);
                        //neighborhood->add_neighbor(*q,k);
                    }


                }


            }
        }
        /*old way
        vector<int> areaCount;
        areaCount.resize(idCount+1);
        areaCount.assign(idCount+1,0);
        //scan through the bounding box to find points contained in the region that are not part of the region
        for (int i = (tree->get_object_list())[o]->get_left(); i <= (tree->get_object_list())[o]->get_right(); i++) {
            for (int j = (tree->get_object_list())[o]->get_top(); j <= (tree->get_object_list())[o]->get_bottom(); j++) {
                int tempID = get_pixel_ID(i,j,(tree->get_object_list())[o]->get_depth());
                if (tempID != o) { // if the point is not a member of the region
                    if (!tree->get_object_list()[o]->surrounds(tempID)) { // and it is not already part of the contours
                        if (((tree->get_object_list())[o]->get_outline()).front()->contains(i,j)) { // and this point is contained by the region
                            areaCount[tempID]++;
                            if (areaCount[tempID] > vgl_area(*((tree->get_object_list()[tempID]->get_outline()).front()))-50) { //if all of this region is contained then add it
                                tree->get_object_list()[o]->add_inner_outline(get_object_list()[tempID]->get_outline().front(), tempID);
                                neighborhood->add_neighbor(tempID, o);
                            }
                        }
                    }
                }
            }
        }*/
    }

    if (type != INIT) {
        vcl_cout << "Contour of region " << o << " has " << l.size() << " points and " << tempNeighbors.size() << " adjacent regions" << vcl_endl;
    }
    set<int>::iterator m;
    for (m = tempNeighbors.begin(); m != tempNeighbors.end(); ++m) {
        neighborhood->add_neighbor(*m, o);
    }
    if (tempNeighbors.size() == 0) {
        neighborhood->add_no_neighbors(o);
    }

}


template <class T>
bool dbseg_seg_object<T>::add_child(int c, int p) {
    bool b = tree->add_child(c, p);
    dbseg_seg_object::create_bounding_box(p);
    dbseg_seg_object::trace(p, ADD);
    return b;
}

template <class T>
bool dbseg_seg_object<T>::remove_child(int c, int p) {
    bool b = tree->remove_child(c, p);
    if (b) {
        dbseg_seg_object::create_bounding_box(p);
        dbseg_seg_object::trace(p, REMOVE);
    }
    return b;
}

template <class T>
list<vgl_polygon<double>*> dbseg_seg_object<T>::get_outline(int o) {
    return (tree->get_object_list())[o]->get_outline();
}

template <class T>
list<vsol_point_2d_sptr> dbseg_seg_object<T>::get_pixels(int o) {
    //another choice would be to use the polygon contains method
    
    list<vsol_point_2d_sptr> l;
    //vgl_polygon<double> poly = *(tree->get_object_list())[o]->get_outline();
    int tempDep = tree->get_object_list()[o]->get_depth();
    for (int i = (tree->get_object_list())[o]->get_left(); i <= (tree->get_object_list())[o]->get_right(); i++) {
        for (int j = (tree->get_object_list())[o]->get_top(); j <= (tree->get_object_list())[o]->get_bottom(); j++) {
            if (get_pixel_ID(i, j, tempDep) == o) {
                l.push_back(new vsol_point_2d(i, j));
            }
        }
    }

    return l;
}

template <class T>
void dbseg_seg_object<T>::create_bounding_box(int o) {
    unsigned int t = labeled_image.nj();
    unsigned int b = 0;
    unsigned int l = labeled_image.ni();
    unsigned int r = 0;

    double count = 0;
    vector<float> tempData;
    tempData.resize(image.nplanes());
    for (int i = 0; i < image.nplanes(); i++) {
        tempData[i] = 0;
    }
    
    int rb = 0;
    int gb = 0;
    int bb = 0;

    //if this is the base level then rescan bounding box and mean intensity from image
    /*if (tree->get_child_count(o) == 1) {
        for (unsigned int i = 0; i < labeled_image.ni(); i++) {
            for (unsigned int j = 0; j < labeled_image.nj(); j++) {
                if (labeled_image(i,j)==o) {
                    //mean intensity
                    count++;
                    rb+=image(i, j, 0);
                    gb+=image(i, j, 1);
                    bb+=image(i, j, 2);

                    //bounding box
                    if (i < l) {
                        l = i;
                    }
                    if (i > r) {
                        r = i;
                    }
                    if (j < t) {
                        t = j;
                    }
                    if (j > b) {
                        b = j;
                    }
                }
            }
        }
    }*/
    //if this has children then compose the bounding box from the boxes of the children and similar with mean intensity
    //else {
    list<int>::iterator i;
    list<int> tempList = tree->get_children_list()[o];
    int childlistsize = tempList.size();
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        //mean intensity
        int tempArea = get_area(*i);
        count += tempArea;
        for (int k = 0; k < image.nplanes(); k++) {
            tempData[k] += tree->get_object_list()[*i]->get_data()[k] * tempArea;
        }
        /*rb += (tree->get_object_list())[*i]->get_r() * tempArea;
        gb += (tree->get_object_list())[*i]->get_g() * tempArea;
        bb += (tree->get_object_list())[*i]->get_b() * tempArea;
        */
        
        //bounding box
        if ((tree->get_object_list())[*i]->get_left() < l) {
            l = (tree->get_object_list())[*i]->get_left();
        }
        if ((tree->get_object_list())[*i]->get_right() > r) {
            r = (tree->get_object_list())[*i]->get_right();
        }
        if ((tree->get_object_list())[*i]->get_top() < t) {
            t = (tree->get_object_list())[*i]->get_top();
        }
        if ((tree->get_object_list())[*i]->get_bottom() > b) {
            b = (tree->get_object_list())[*i]->get_bottom();
        }

    }    
    //}
    
    vector<T> tempData2;
    for (int k = 0; k < image.nplanes(); k++) {
        tempData[k] = tempData[k] / count;
        tempData2.push_back(tempData[k]);
    }
    tree->get_object_list()[o]->set_data(tempData2);
    //(tree->get_object_list())[o]->set_mean_intensity(rb/count, gb/count, bb/count);
    //vcl_cout << "mean intensity of object " << o << " set to R: " << tree->get_object_list()[o]->get_r() << " G: " << tree->get_object_list()[o]->get_g() << " B: " << tree->get_object_list()[o]->get_r() << vcl_endl;
    (tree->get_object_list())[o]->set_bounding_box(t, b, l, r);

}

template <class T>
void dbseg_seg_object<T>::initial_scan() {
    int numSegs = 0;
    vul_timer t;
    vul_timer t1;
    t.mark();
    t1.mark();
    
    for (unsigned int i = 0; i < labeled_image.ni(); i++) {
        for (unsigned int j = 0; j < labeled_image.nj(); j++) {
            if (labeled_image(i,j) > numSegs) {
                numSegs = labeled_image(i, j);
            }

        }
    }
    numSegs++;
    vcl_cout << "Beginning Segmentation Object Generation, " << numSegs << " regions found..." << vcl_endl;
    
    tree = new dbseg_seg_tree<T>(numSegs);
    neighborhood = new dbseg_seg_neighborhood(numSegs);

    list<int> l;
    tree->get_depth_lists().push_back(l);
    tree->get_depth_lists().push_back(l);

    for (idCount = 1; idCount < numSegs; idCount++) {
        (tree->get_object_list()).push_back(new dbseg_seg_spatial_object<T>(idCount, 1, image.nplanes()));
        tree->add_valid_ID(idCount);
        tree->create_space();
        /*    random color generator
        srand(time(NULL));
        r[idCount] = rand() % 200 + 50;
        srand(time(NULL));
        g[idCount] = rand() % 200 + 50;
        srand(time(NULL));
        b[idCount] = rand() % 200 + 50;
        */
        
        (tree->get_depth_lists())[1].push_back(idCount);
    }

    //sets up bounding boxes for each region and trace the outlines
    vector<int> tops(numSegs+1);
    vector<int> lefts(numSegs+1);
    vector<int> rights(numSegs+1);
    vector<int> bottoms(numSegs+1);

    /*vector<int> rCount(numSegs+1);
    vector<int> gCount(numSegs+1);
    vector<int> bCount(numSegs+1);
    */

    vector<vector<float>> tCount(numSegs+1);

    vector<int> aCount(numSegs+1);

    for (int i = 0; i <= numSegs; i++) {
        tops[i] = labeled_image.nj();
        lefts[i]= labeled_image.ni();
        rights[i] = 0;
        bottoms[i] = 0;
        tCount[i].resize(image.nplanes());
        for (int j = 0; j < image.nplanes(); j++) {
            tCount[i][j] = 0;
        }
        
        /*tCount[i] = 0;
        rCount[i]=0;
        gCount[i]=0;
        bCount[i]=0;*/
        aCount[i]=0;
        
    }
    for (unsigned int i = 0; i < labeled_image.ni(); i++) {
        for (unsigned int j = 0; j < labeled_image.nj(); j++) {
            int num = labeled_image(i,j);
            aCount[num]++;
            for (int k = 0; k < image.nplanes(); k++) {
                tCount[num][k]+=image(i,j,k);
            }
            
            /*if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
                rCount[num]+=image(i,j,0);
                gCount[num]+=image(i,j,1);
                bCount[num]+=image(i,j,2);
            }
            else {
                tCount[num]+=image(i,j);
            }*/

            if (i < lefts[labeled_image(i,j)]) {
                lefts[labeled_image(i,j)] = i;
            }

            if (i > rights[labeled_image(i,j)]) {
                rights[labeled_image(i,j)] = i;
            }

            if (j < tops[labeled_image(i,j)]) {
                tops[labeled_image(i,j)] = j;
            }

            if (j > bottoms[labeled_image(i,j)]) {
                bottoms[labeled_image(i,j)] = j;
            }
        }
    }
    vcl_cout << "Bounding boxes created. Time elapsed: " << t1.real()/1000 << " seconds. Beginning contour tracing..." << vcl_endl;
    t1.mark();
    int block = numSegs / 10;
    for (int i = 1; i < numSegs; i++) {
        if (block != 0) {
            if (i % block == 0) {
                vcl_cout << i / block * 10 << "% ";
            }
        }

        int t = tops[i];
        int b = bottoms[i];
        int l = lefts[i];
        int r = rights[i];
        (tree->get_object_list())[i]->set_bounding_box(t, b, l, r);
        //average values and set data
        vector<T> tempTCount;
        for (int k = 0; k < image.nplanes(); k++) {
            tCount[i][k] = tCount[i][k] / aCount[i];
            tempTCount.push_back(tCount[i][k]);
        }
        
        tree->get_object_list()[i]->set_data(tempTCount);
        /*if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
            (tree->get_object_list())[i]->set_data(   (rCount[i]/aCount[i], gCount[i]/aCount[i], bCount[i]/aCount[i]);
        
        (tree->get_object_list())[i]->set_mean_intensity(rCount[i]/aCount[i], gCount[i]/aCount[i], bCount[i]/aCount[i]);
        */
        
        //vcl_cout << "mean intensity of object " << i << " set to R: " << tree->get_object_list()[i]->get_r() << " G: " << tree->get_object_list()[i]->get_g() << " B: " << tree->get_object_list()[i]->get_r() << vcl_endl;

        //trace the outlines (which sets up the neighborhood)
        trace(i, INIT);
        //((tree->get_object_list())[i])->set_bounding_box(tops[i], bottoms[i], left[i], rights[i]);
    }
    vcl_cout << vcl_endl<< "Outer contours traced.  Time elapsed: " << t1.real()/1000 << " seconds." << vcl_endl;
    t1.mark();
    vcl_cout << "Adding inner contours..." << vcl_endl;
    //add in any inner contours
    int innerContCount = 0;
    int innerContTest = 0;
    int testAfter1 = 0;
    for (int k = 1; k < numSegs; k++) {
        if (block != 0) {
            if (k % block == 0) {
                vcl_cout << k / block * 10 << "% ";
            }
        }

        vector<int> areaCount;
        areaCount.resize(numSegs+1);
        areaCount.assign(numSegs+1,0);
        //scan through the bounding box to find points contained in the region that are not part of the region
        int kL = tree->get_object_list()[k]->get_left();
        int kR = tree->get_object_list()[k]->get_right();
        int kT = tree->get_object_list()[k]->get_top();
        int kB = tree->get_object_list()[k]->get_bottom();
        vgl_polygon<double>* outerCont = tree->get_object_list()[k]->get_outline().front();
        
        list<int> tempNList = neighborhood->get_neighbors(k);
        list<int>::iterator q2;
        for (q2 = tempNList.begin(); q2 != tempNList.end(); ++q2) {
            int q = *q2;
        //for (int q = 1; q < numSegs; q++) {
            //if it's not the same object and the large object doesn't already surround the small one
            if (k != q && !tree->get_object_list()[k]->surrounds(q)) {
                int qL = tree->get_object_list()[q]->get_left();
                int qR = tree->get_object_list()[q]->get_right();
                int qT = tree->get_object_list()[q]->get_top();
                int qB = tree->get_object_list()[q]->get_bottom();
                //if this region's bounding box is contained in the larger region's bounding box
                if (kL <= qL && kR >= qR && kT <= qT && kB >= qB) {
                    vgl_polygon<double>* innerCont = tree->get_object_list()[q]->get_outline().front();
                    innerContTest++;
                    bool containsEveryPoint = true;
                    for (int j = 0; j < (*innerCont)[0].size(); j++) {
                        if (!outerCont->contains((*innerCont)[0][j].x(), (*innerCont)[0][j].y())) {
                            containsEveryPoint = false;
                            break;
                        }
                        testAfter1++;
                    }
                    if (containsEveryPoint) {
                        innerContCount++;
                        tree->get_object_list()[k]->add_inner_outline(innerCont, q);
                        neighborhood->add_neighbor(q,k);
                    }


                }


            }
        }
        
        
        /*   old way
        for (int i = (tree->get_object_list())[k]->get_left(); i <= (tree->get_object_list())[k]->get_right(); i++) {
            for (int j = (tree->get_object_list())[k]->get_top(); j <= (tree->get_object_list())[k]->get_bottom(); j++) {
                if (labeled_image(i,j) != k) { // if the point is not a member of the region
                    if (!tree->get_object_list()[k]->surrounds(labeled_image(i,j))) { // and it is not already part of the contours
                        if (((tree->get_object_list())[k]->get_outline()).front()->contains(i,j)) { // and this point is contained by the region
                            areaCount[labeled_image(i,j)]++;
                            if (areaCount[labeled_image(i,j)] > vgl_area(*((tree->get_object_list()[labeled_image(i,j)]->get_outline()).front()))-50) { //if all of this region is contained then add it
                                tree->get_object_list()[k]->add+_inner_outline(get_object_list()[labeled_image(i,j)]->get_outline().front(), labeled_image(i,j));
                                neighborhood->add_neighbor(labeled_image(i,j), k);
                            }
                        }
                    }
                }
            }
        }*/
        //vcl_cout << "Region " << k << " complete" << vcl_endl;
    }
    vcl_cout << vcl_endl << "Inner Cont Count: " << innerContCount << "  test times: " << innerContTest << "  test 1 point: " << testAfter1 << vcl_endl;
    vcl_cout << vcl_endl << "Initial Scan Complete.  Total time elapsed: " << t.real()/1000 << " seconds." << vcl_endl;
    tops.~vector();
    lefts.~vector();
    bottoms.~vector();
    rights.~vector();
    
}

template <class T>
vector<dbseg_seg_spatial_object<T>*>& dbseg_seg_object<T>::get_object_list() {
    return tree->get_object_list();
}

template <class T>
vector<int>& dbseg_seg_object<T>::get_parent_list() {
    return tree->get_parent_list();
}

template <class T>
vector<list<int>>& dbseg_seg_object<T>::get_children_list() {
    return tree->get_children_list();
}

template <class T>
vil_image_view<int>& dbseg_seg_object<T>::get_labeled_image() {
    return labeled_image;
}

template <class T>
dbseg_seg_tree<T>* dbseg_seg_object<T>::get_tree() {
    return tree;
}

template <class T>
list<int> dbseg_seg_object<T>::get_neighbors(int o) {
// make sure to delete these lists after you use them

    //if it's a base object then return neighbor list
    if (tree->get_child_count(o) == 0) {
        list<int> l = (neighborhood->get_neighbors(o));
        
        return l;
    }
    //otherwise compile the list   
    return dbseg_seg_object::get_neighbors(o, (tree->get_object_list())[o]->get_depth());


}

template <class T>
list<int> dbseg_seg_object<T>::get_neighbors(int o, int d) {

    list<int> l;
    set<int> baseChildren = tree->get_base_children(o);
    list<int>::iterator i;
    set<int>::iterator i3;
    
    for (i3 = baseChildren.begin(); i3 != baseChildren.end(); ++i3) { // go through all of the base children to look at their neighbors
        list<int> baseNeighbors = neighborhood->get_neighbors(*i3);
        list<int>::iterator j;
        for (j = baseNeighbors.begin(); j != baseNeighbors.end(); ++j) { // go through all of the base neighbors to find the corresponding object at the right depth
            dbseg_seg_spatial_object<T>* obj = (tree->get_object_list())[*j];
            while (obj->get_depth() < d) { // traverse tree up to correct depth
                obj=(tree->get_object_list())[tree->get_parent(obj->get_id())];
            }
            l.remove(obj->get_id());
            l.push_back(obj->get_id());
        }
    }


    //remove things that are inside the object
    for (i = l.begin(); i != l.end();) {
        bool found = false;
        dbseg_seg_spatial_object<T>* obj = (tree->get_object_list())[*i];
        while (tree->has_parent(obj->get_id())) { // traverse tree up to correct depth
            if (tree->get_parent(obj->get_id()) == o) {
                found = true;
                break;
            }
            obj=(tree->get_object_list())[tree->get_parent(obj->get_id())];
        }        
        if (found) {
            l.erase(i);
        }
        else {
            i++;
        }

    }

    return l;
}


template <class T>
dbseg_seg_neighborhood* dbseg_seg_object<T>::get_neighborhood() {
    return neighborhood;
}

template <class T>
vil_image_view<T>& dbseg_seg_object<T>::get_image() {
    return image;
}

template <class T>
vector<list<int>>& dbseg_seg_object<T>::get_neighbor_list() {
    return neighborhood->get_neighbor_list();
}

template <class T>
int dbseg_seg_object<T>::get_pixel_ID(int x, int y, int l) {
    //start at the base level
    
    dbseg_seg_spatial_object<T>* o = (get_object_list())[labeled_image(x,y)];

    int tempid = o->get_id();
    int p = tree->get_parent(tempid);
    //iterate up the parents of the tree until you find the desired depth
    int depthCheck = o->get_depth();
    while (depthCheck > l) {
        o=(get_object_list())[tree->get_parent(o->get_id())];
        p = tree->get_parent(o->get_id());
        depthCheck = o->get_depth();

    }

    return o->get_id();
}

template <class T>
vil_image_view<vxl_byte> dbseg_seg_object<T>::get_display(int dep, int editing) {
    vil_image_view<vxl_byte>* img = new vil_image_view<vxl_byte>(image.ni(), image.nj(), 3);// = image;
    //vil_image_view<int> lab = labeled_image;
    //int num = (tree->get_depth_list())[dep].size();

    /*
    vector<int> rD;
    vector<int> gD;
    vector<int> bD;
    vector<int> count;
    for (int i = 0; i <= idCount; i++) {
        count[i]=0;
        rD[i]=0;
        gD[i]=0;
        bD[i]=0;
    }

    list<int>::iterator k;
    
    /*for (k = (tree->get_depth_list())[dep].begin(); k != (tree->get_depth_list())[dep].end(); k++) {
        list<int>::iterator m;
        int count = 0;
        for (m = tree->get_base_children(*k).begin(); m != tree->get_base_children(*k).end(); m++) {
            count++;
            rD[
        }
    }
    for (unsigned int i = 0; i < img.ni(); i++) {
        for (unsigned  int j = 0; j < img.nj(); j++) {
            int curr = seg_object::get_pixel_ID(i, j, dep);
            count[curr]++;
            rD[curr]+=image(i, j).r;
            gD[curr]+=image(i, j).g;
            bD[curr]+=image(i, j).b;
        }
    }
            

    */

    /*list<int>::iterator k;
    list<int>::iterator l;

    for (k = (tree->get_depth_list())[dep].begin(); k != (tree->get_depth_list())[dep].end(); k++) {
        

    }
    */

    //another option would be to use the get_pixels lists which is based on the polygon contains() method
    if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        for (unsigned int i = 0; i < img->ni(); i++) {
            for (unsigned  int j = 0; j < img->nj(); j++) {
                int tempID = dbseg_seg_object::get_pixel_ID(i, j, dep);
                if (tempID == editing) { //if an object is being edited, don't display the object, display its children
                    tempID = dbseg_seg_object::get_pixel_ID(i,j,dep+1);
                }

                    
                (*img)(i,j,0)=get_object_list()[tempID]->get_data()[0];
                (*img)(i,j,1)=get_object_list()[tempID]->get_data()[1];
                (*img)(i,j,2)=get_object_list()[tempID]->get_data()[2];


                //    , (seg_object::get_object_list())[tempID]->get_g(), (seg_object::get_object_list())[tempID]->get_b());
            }
        }
    }
    else {
        vcl_cerr << "Error - Unable to display data type." << vcl_endl;
    }

    return *img;
}

template <class T>
bool dbseg_seg_object<T>::create_region(list<int> l) {
    //check if the regions are continuous
    vul_timer t;
    t.mark();
    
    discontinuous = is_continuous(l);
    if (discontinuous != 0) {
        vcl_cerr << "Error creating region - selected sections do not form a continuous region" << vcl_endl;
        return false;
    }
    vcl_cout << "Creating Region - New Region is continuous" << vcl_endl;
    //check to see that if the region includes a section that surrounds another section




    //int tempID = ++idCount;
    tree->add_valid_ID(idCount);
    
    (dbseg_seg_object::get_object_list()).push_back(new dbseg_seg_spatial_object<T>(idCount, 1, image.nplanes()));
    tree->create_space();
    neighborhood->create_space();
    tree->get_depth_lists()[1].push_back(idCount);
    //list<int> blankList;
    //tree->get_parent_list().push_back(0);   done in tree::set_parent
    //tree->get_children_list().push_back(blankList); done in tree::add_child

    

    //list<int> nList;
    
    
    list<int>::iterator i;
    for (i = l.begin(); i != l.end(); i++) {
        tree->add_child(*i, idCount);
        /*list<int> tempnList = neighborhood->get_neighbors(*i);
        list<int>::iterator j;
        for (j = tempnList.begin(); j != tempnList.end(); ++j) {
            nList.push_back(*j);
        }*/
        
    }
    reset_neighbors(idCount, l);
    


    dbseg_seg_object::create_bounding_box(idCount);
    
    //set up outline
    vgl_polygon<double> tempOutline;
    set<int> alreadyDone;
    set<int> toDo;
    int start;
    for (i = l.begin(); i != l.end(); ++i) {
        if (i == l.begin()) {
            alreadyDone.insert(*i);
            start = *i;
        }
        else {
            toDo.insert(*i);
        }
    }
    /*growOutline(tempOutline, alreadyDone, toDo, start);
    for (int j = 0; j < tempOutline[0].size(); j++) {
        tree->get_object_list()[idCount]->add_point(tempOutline[0][j].x(), tempOutline[0][j].y());
    }*/

    

    dbseg_seg_object::trace(idCount, CREATE);
    
    
    
    
    
    //set<int> alreadyReTraced;
    //check to see if any existing regions contained these children inside of them and if so then retrace them
    
    //not sure this needs to be done
    /*
    list<int> valid = tree->get_valid_IDs();
    for (i = valid.begin(); i != valid.end(); ++i) {
        list<int>::iterator j;
        for (j = l.begin(); j != l.end(); ++j) {
            if (tree->get_object_list()[*i]->surrounds(*j)) {
                trace(*i);
                j = l.end();
                break;
            }
        }

    }*/
    vcl_cout << "Region Successfully Created.  Time Elapsed: " << t.real()/1000 << " seconds." << vcl_endl;
    idCount++;
    return true;
}    

template <class T>
list<list<vgl_polygon<double>*>> dbseg_seg_object<T>::get_outlines(int dep, list<int> outL, bool all) {  //for display
    list<list<vgl_polygon<double>*>> l;
    list<int>::iterator i;
    list<int> tempList = outL;//tree->get_valid_IDs();
    //cl_cout << "size of depth list: " << tempList.size() << " at depth " << dep << vcl_endl;
    for (i = (tempList.begin()); i != tempList.end(); ++i) {
        /*if (i == tree->get_depth_list()[dep].begin()) {
            ++i;
        }*/
        //if you're on this depth level or you are at a shallower depth but you have no children
        if (tree->get_object_list()[*i]->get_depth() == dep || (tree->get_object_list()[*i]->get_depth() < dep && tree->get_child_count(*i) == 0) ) {
            if (all || is_outlined(*i)) {
                l.push_back(tree->get_object_list()[*i]->get_outline());
            }
        }
    }
    return l;
}

template <class T>
double dbseg_seg_object<T>::get_area(int o) {
    double a = vgl_area<double>(*(tree->get_object_list()[o]->get_outline().front()));
    set<int> tempList = tree->get_object_list()[o]->get_contained();
    set<int>::iterator i;
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        a -= get_area(*i);
    }
    
    return a;
}

template <class T>
void dbseg_seg_object<T>::toggle_outlined(int o) {
    (tree->get_object_list())[o]->toggle_outlined();
    update_outlined(o);
}

template <class T>
void dbseg_seg_object<T>::toggle_highlighted(int o) {
    (tree->get_object_list())[o]->toggle_highlighted();
}

template <class T>
bool dbseg_seg_object<T>::is_outlined(int o) {
    return (tree->get_object_list())[o]->is_outlined();
}

template <class T>
bool dbseg_seg_object<T>::is_highlighted(int o) {
    return (tree->get_object_list())[o]->is_highlighted();
}

template <class T>
list<int> dbseg_seg_object<T>::get_depth_list(int dep) {
    return tree->get_depth_lists()[dep];
}

template <class T>
int dbseg_seg_object<T>::is_continuous(std::list<int> l, int d) {
    list<int>::iterator i;
    //bool temp;
    set<int> tempDone;
    set<int> tempPossible;
    set<int> toDo;

    //  new way
    int start;
    
    for (i = l.begin(); i != l.end(); ++i) {
        if (i == l.begin()) {
            start = *i;
        }
        else {
            tempPossible.insert(*i);
        }
    }
    i = l.begin();
    i++;
    for (i = i; i != l.end(); ++i) {
        if (!continuousR(start, *i, tempPossible, tempDone, d)) {
            i--;
            if (i == l.begin()) {
                return *i;
            }
            else {
                i++;
                return *i;
            }
        }
        tempDone.clear();
    }
    return 0;
    

    

/*  old way
    for (i = l.begin(); i != l.end(); ++i) {
        list<int>::iterator j;
        list<int> tempList = tree->get_base_children(*i);
        toDo.insert(tempList.front());                    //insert one base child from each region
        for (j = tempList.begin(); j != tempList.end(); ++j) {
            tempPossible.insert(*j);
        }
    }

    int start = tree->get_base_children(l.front()).front();
    //l.pop_front();
    tempPossible.erase(tempPossible.find(start));
    
    set<int>::iterator i2;
    tempDone.insert(start);
    for (i2 = toDo.begin(); i2 != toDo.end(); ++i2) {
        if (!continuousR(start, *i2, tempPossible, tempDone)) {
            return false;
        }
    }
    return true;
*/
}

template <class T>
bool dbseg_seg_object<T>::continuousR(int curr, int goal, std::set<int> possible, std::set<int>& done, int d) {
    list<int>::iterator i;
    //set<int>::iterator j;
    //bool temp = false;
    int tempDepth = tree->get_object_list()[curr]->get_depth();
    done.insert(curr);
    for (i = neighborhood->get_neighbors(curr).begin(); i != neighborhood->get_neighbors(curr).end(); ++i) {
        bool condition;
        if (d == 0) {
            condition = tree->get_object_list()[*i]->get_depth() == tempDepth;
        }
        else {
            condition = (tree->get_object_list()[*i]->get_depth() == d) || (tree->get_object_list()[*i]->get_depth() == d+1);
        }
        if (condition) {
            if (*i == goal) {
                return true;
            }
            if (possible.find(*i) != possible.end()) { //if the neighbor is a possible place to go to
                if (done.find(*i) == done.end()) { // and it hasn't already been searched
                    if (continuousR(*i, goal, possible, done, d)) { // then look there
                        return true;
                    }
                }
            }
        }
    }
    return false;

}

template <class T>
list<int>& dbseg_seg_object<T>::get_valid_IDs() {
    return tree->get_valid_IDs();
}

template <class T>
int dbseg_seg_object<T>::get_max_depth() const {
    int temp = 0;
    list<int>::const_iterator i;
    list<int> tempList = tree->get_valid_IDs();
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        if (tree->get_object_list()[*i]->get_depth() > temp) {
            temp = tree->get_object_list()[*i]->get_depth();
        }
    }
    return temp;
}

template <class T>
bool dbseg_seg_object<T>::edit_children(std::list<int> l, int p) {
    //check if this region should be deleted because it has either 1 or 0 chlidren
    if (l.size() == 0) {
        
        //return false;

    }

    //check if these new children form a continuous region
    if (l.size() > 1) {
        discontinuous = is_continuous(l, tree->get_object_list()[p]->get_depth());
        if (discontinuous != 0) {
            vcl_cerr << "Error editing region:  Selected sections form a non-continuous region" << vcl_endl;
            return false;
        }    
    }    
    
    list<int>::iterator i;
    set<int> oldChildrenSet;
    set<int> newChildrenSet;
    list<int> oldChildrenList = tree->get_children(p);
    for (i = oldChildrenList.begin(); i != oldChildrenList.end(); ++i) {
        oldChildrenSet.insert(*i);
    }
    

    //add in objects
    for (i = l.begin(); i != l.end(); ++i) {
        if (tree->get_parent(*i) != tree->get_parent(p) && tree->get_parent(*i) != p) {
            vcl_cerr << "Error editing region:  Selected region " << *i << " is a child of " << tree->get_parent(*i) << " not " << tree->get_parent(p) << vcl_endl;
            return false;
        }
        newChildrenSet.insert(*i);
        if (oldChildrenSet.find(*i) == oldChildrenSet.end()) { //if it's not part of the previous children then add it
            tree->add_child(*i, p);
        }
    }
    bool temp = true;
    //remove objects
    for (i = oldChildrenList.begin(); i != oldChildrenList.end(); ++i) {
        if (newChildrenSet.find(*i) == newChildrenSet.end()) { //if it's not part of the new children then remove it
            if(!tree->remove_child(*i, p)) { //if this object was destroyed then set temp to false to it can be returned
                temp = false;
            }
        }

    }
    //if there is now only one child left then the region is not valid so destroy it
    if (tree->get_child_count(p) == 1) {
        temp = false;
        tree->remove_child(tree->get_children(p).front(), p);
        vcl_cerr << "Editing Region:  Selected regions do not warrent higher level containment.  Region removed." << vcl_endl;
    }


    if (!temp) { // if the object was destroyed then return
        vcl_cerr << "Editing Region:  Selected regions do not warrent higher level containment.  Region removed." << vcl_endl;
        return false;
    }
    
    //reset neighborhood
    reset_neighbors(p, l);
    
    //otherwise trace around the new object
    create_bounding_box(p);
    trace(p, EDIT);
    return true;

    //I don't think this is necessary because any children removed from this parent will still be part of their grandparent's family
    /*
    //traverse up depth levels checking that the regions will be continuous
    dbseg_seg_spatial_object* o = (get_object_list())[p];
    while (o->get_depth() > 1) {
        o=(get_object_list())[tree->get_parent(o->get_id())];
        //check if these new children will form a region continuous with the children of the parent at this depth level
        if (!is_continuous(tree->get_base_children(o->get_id(), p))) {
            vcl_cout << "Error editing region:  Selected sections form a non-continuous region in tree structure at depth: " << o->get_depth() << vcl_endl;
            return false;
        }
    }
    */
        


}

template <class T>
vcl_ostream& dbseg_seg_object<T>::save_text(vcl_ostream &os) {
    /*
            version
            nplanes
            n (n valid IDs)
            n parentIDs
  {n lines} j (j IDs of n's children)
  {n lines} j (j IDs of n's neighbors)
            maxDepth
  {n sets}  (objectText)
            width height
  {height}  (width of labels)           labeled image
  {height}  (width of R values)
  {height}  (width of G values)         original image
  {height}  (width of B values)
    */
    double version = 1.1;
    os << version << " ";
    os << spine_parts.size() << " ";
    for (int j = 0; j < spine_parts.size(); j++) {
        os << spine_parts[j] << " ";
    }

    os << disk_cornersX.size() << " ";
    for (int j = 0; j < disk_cornersX.size(); j++) {
        os << disk_cornersX[j] << " ";
        os << disk_cornersY[j] << " ";
    }



    os << image.nplanes() << " ";

    os << tree->get_valid_IDs().size();
    list<int>::iterator i;
    list<int> tempList = tree->get_valid_IDs();
    
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        os << " " << *i;
    }
    os << vcl_endl;
    
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        if (i != tempList.begin()) {
            os << " ";
        }
        os << tree->get_parent(*i);
    }
    os << vcl_endl;
    
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        os << tree->get_child_count(*i);
        list<int> temp2 = tree->get_children(*i);
        list<int>::iterator k;
        for (k = temp2.begin(); k != temp2.end(); ++k) {
            os << " " << *k;
        }
        os << vcl_endl;
    }

    for (i = tempList.begin(); i != tempList.end(); ++i) {
        os << neighborhood->get_neighbors(*i).size();
        list<int> temp2 = neighborhood->get_neighbors(*i);
        list<int>::iterator k;
        for (k = temp2.begin(); k != temp2.end(); ++k) {
            os << " " << *k;
        }
        os << vcl_endl;
    }

    os << get_max_depth() << vcl_endl;

    for (i = tempList.begin(); i != tempList.end(); ++i) {
        tree->get_object_list()[*i]->save_text(os);
    }

    os << labeled_image.ni() << " " << labeled_image.nj() << vcl_endl;

    for (int x = 0; x < labeled_image.ni(); x++) {
        for (int y = 0; y < labeled_image.nj(); y++) {
            if (y != 0) {
                os << " ";
            }
            os << labeled_image(x,y);
        }
        os << vcl_endl;
    }
    os << vcl_endl;

    for (int rgb = 0; rgb < 3; rgb++) {
        for (int x = 0; x < labeled_image.ni(); x++) {
            for (int y = 0; y < labeled_image.nj(); y++) {
                if (y != 0) {
                    os << " ";
                }
                os << (int)image(x,y,rgb);
            }
            os << vcl_endl;
        }
        os << vcl_endl;
    }

    //vil_save(image, "bobbefore.jpg");

    return os;

}

template <class T>
vgl_polygon<double>& dbseg_seg_object<T>::growOutline(vgl_polygon<double>& out, std::set<int>& alreadyDone, std::set<int>& toDo, int curr) {
    vgl_polygon<double> tempPoly = *(tree->get_object_list()[curr]->get_outline().front());
    out = vgl_clip(out, tempPoly, vgl_clip_type_union);
    alreadyDone.insert(curr);
    //toDo.erase(toDo.find(curr));
    set<int>::iterator i;
    bool stop = false;
    while (!stop) {
        set<int> tempToDo = toDo;
        for (i = tempToDo.begin(); i != tempToDo.end(); ++i) {
            if (alreadyDone.find(*i) == alreadyDone.end()) {
                if (neighborhood->is_neighbors(curr, *i)) {
                    out = vgl_clip(out, growOutline(out, alreadyDone, toDo, *i), vgl_clip_type_union);
                    break;
                }
            }

        }
        stop = i == tempToDo.end();
    }

    return out;
}

template <class T>
void dbseg_seg_object<T>::reset_neighbors(int o, list<int> l) {
    list<int> nList;
    
    
    list<int>::iterator i;
    for (i = l.begin(); i != l.end(); i++) {
        //tree->add_child(*i, idCount);
        list<int> tempnList = neighborhood->get_neighbors(*i);
        list<int>::iterator j;
        for (j = tempnList.begin(); j != tempnList.end(); ++j) {
            nList.push_back(*j);
        }
        
    }
    
    //remove regions from neighborlist that are inside the region
    for (i = l.begin(); i != l.end(); ++i) {
        list<int> toRemove;
        tree->get_descendants(toRemove, *i);
        list<int>::iterator j;
        for (j = toRemove.begin(); j != toRemove.end(); ++j) {
            nList.remove(*j);
        }
    }

    neighborhood->clear_neighbors(o);
    //set up neighborhood relationships
    for (i = nList.begin(); i != nList.end(); ++i) {
        neighborhood->add_neighbor(*i, o);
        //neighborhood->add_neighbor(o, *i);
    }
}

template <class T>
int dbseg_seg_object<T>::get_discontinuous() {
    return discontinuous;
}

template <class T>
int dbseg_seg_object<T>::get_child_count(int o) {
    return tree->get_child_count(o);
}

template <class T>
list<int> dbseg_seg_object<T>::get_outlined_regions() {
    return outlinedRegions;
}

template <class T>
void dbseg_seg_object<T>::update_outlined(int o) {
    if (is_outlined(o)) {
        outlinedRegions.push_back(o);
    }
    else {
        outlinedRegions.remove(o);
    }
}



//spine
/*template <class T>
SpinePart dbseg_seg_object<T>::num_to_enum(int i) {
    switch (i) {
        case 0:
            return C1;
        case 1:
            return D1;
        case 2:
            return C2;
        case 3:
            return D2;
        case 4:
            return C3;
        case 5:
            return D3;
        case 6:
            return C4;
        case 7:
            return D4;
        case 8:
            return C5;
        case 9:
            return SPINE;
    }
    return null;

}*/

template <class T>
void dbseg_seg_object<T>::set_spine_part(SpinePart p, int o) {
    switch (p) {
        case C1:
            spine_parts[0] = o;
            break;
        case D1:
            spine_parts[1] = o;
            break;
        case C2:
            spine_parts[2] = o;
            break;
        case D2:
            spine_parts[3] = o;
            break;
        case C3:
            spine_parts[4] = o;
            break;
        case D3:
            spine_parts[5] = o;
            break;
        case C4:
            spine_parts[6] = o;
            break;
        case D4:
            spine_parts[7] = o;
            break;
        case C5:
            spine_parts[8] = o;
            break;
        case SPINE:
            spine_parts[9] = o;
            break;
    }
}

template <class T>
int dbseg_seg_object<T>::get_spine_part(SpinePart p) {
    switch (p) {
        case C1:
            return spine_parts[0];
        case D1:
            return spine_parts[1];
        case C2:
            return spine_parts[2];
        case D2:
            return spine_parts[3];
        case C3:
            return spine_parts[4];
        case D3:
            return spine_parts[5];
        case C4:
            return spine_parts[6];
        case D4:
            return spine_parts[7];
        case C5:
            return spine_parts[8];
        case SPINE:
            return spine_parts[9];
    }
}

template <class T>
int dbseg_seg_object<T>::get_spine_part(int p) {
    return spine_parts[p];
}

template <class T>
void dbseg_seg_object<T>::set_disk_corner(int p, bool bottom, double x, double y) {
    disk_cornersX[(p-1)*2+bottom] = x;
    disk_cornersY[(p-1)*2+bottom] = y;
    

}

template <class T>
void dbseg_seg_object<T>::set_disk_pro(int p, double x, double y) {
    disk_proX[p-1] = x;
    disk_proY[p-1] = y;
}


template <class T>
void dbseg_seg_object<T>::set_disk_pro_poly(int p, vgl_polygon<double> poly) {
    disk_pro_polys[p-1] = poly;
}

template <class T>
vgl_polygon<double> dbseg_seg_object<T>::get_disk_pro_poly(int p) {
    return disk_pro_polys[p-1];
}


template <class T>
void dbseg_seg_object<T>::get_disk_corners(int p, double& x1, double& y1, double& x2, double& y2) {
    //p is 1, 2, 3, or 4
    x1 = disk_cornersX[(p-1)*2];
    x2 = disk_cornersX[(p-1)*2+1];
    y1 = disk_cornersY[(p-1)*2];
    y2 = disk_cornersY[(p-1)*2+1];
}

template <class T>
void dbseg_seg_object<T>::get_disk_pro(int p, double& x, double& y) {
    //p is 1, 2, 3, or 4
    x = disk_proX[p-1];
    y = disk_proY[p-1];
}


    enum SpineParts {
        C1,
        D1,
        C2,
        D2,
        C3,
        D3,
        C4,
        D4,
        C5,
        SPINE
    };

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
template <class T>
void dbseg_seg_object<T>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
    vsl_b_write(os, image.nplanes());
    list<int> tempValidIDs = tree->get_valid_IDs();
    vsl_b_write(os, tempValidIDs.size());
    list<int>::iterator i;
    vcl_vector<int> tempIDsVector;
    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        //vsl_b_write(os, static_cast<short>(rand()%200));
        tempIDsVector.push_back(*i);
    }
    vsl_b_write(os, tempIDsVector);
    /*vector<int> parent_list;
    vector<list<int>> children_list;
    vector<dbseg_seg_spatial_object<T>*> object_list;
    vector<list<int>> neighbor_list;

    parent_list.resize(idCount);
    children_list.resize(idCount);
    object_list.resize(idCount);
    neighbor_list.resize(idCount);*/
/*
    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        vsl_b_write(os, tree->get_parent(*i));
    }

    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        list<int> tempChildList = tree->get_children(*i);
        vsl_b_write(os, tree->get_child_count(*i));
        list<int>::iterator i2;
        for (i2 = tempChildList.begin(); i2 != tempChildList.end(); ++i2) {
            vsl_b_write(os, *i2);
        }
    }

    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        list<int>::iterator i2;
        list<int> tempNList = neighborhood->get_neighbors(*i);
        vsl_b_write(os, tempNList.size());
        for (i2 = tempNList.begin(); i2 != tempNList.end(); ++i2) {
            vsl_b_write(os, *i2);
        }
    }

    vsl_b_write(os, get_max_depth());
    int testdepth = get_max_depth();

    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        dbseg_seg_spatial_object<T>* tempObj = tree->get_object_list()[*i];

        //vsl_b_write(os, tempPoly);



        vsl_b_write(os, tempObj->get_depth());
        vsl_b_write(os, tempObj->get_bottom());
        vsl_b_write(os, tempObj->get_top());
        vsl_b_write(os, tempObj->get_left());
        vsl_b_write(os, tempObj->get_right());
        for (int k = 0; k < image.nplanes(); k++) {
            T testcheck = tempObj->get_data()[k];
            vsl_b_write(os, tempObj->get_data()[k]);
        }

        set<int> tempContained = tempObj->get_contained();
        int tempNumContained = tempContained.size();
        vsl_b_write(os, tempNumContained);
        set<int>::iterator setItr;
        for (setItr = tempContained.begin(); setItr != tempContained.end(); ++setItr) {
            vsl_b_write(os, *setItr);
        }

        list<vgl_polygon<double>*> tempContours = tempObj->get_outline();
        list<vgl_polygon<double>*>::iterator polyItr;
        for (polyItr = tempContours.begin(); polyItr != tempContours.end(); ++polyItr) {
            vgl_polygon<double> tempPoly = **polyItr;//*(tempContours.front());
            int count = tempPoly[0].size();
            vector<pair<double, double>> coords;
            coords.clear();
            for (int countI = 0; countI < count; countI++) {
                coords.push_back(pair<double, double>(tempPoly[0][countI].x()*1, tempPoly[0][countI].y()*1));
            }
            vsl_b_write(os, count);
            for (int countI = 0; countI < count; countI++) {
                if ( (*i == 6 && countI == 7) || (*i == 7 && countI == 0)  || (*i == 4 && countI == 5) || (*i == 5 && countI == 1) || (*i == 7 && countI == 8)) {
                    //vcl_cout << "x: " << coords[countI].first << " y: " << coords[countI].second << vcl_endl;
                }
                if (countI < 111050 
                    
                    
                    ) {
    //                vsl_b_write(os, static_cast<double>(static_cast<float>(coords[countI].first)));
    //                vsl_b_write(os, static_cast<double>(static_cast<float>(coords[countI].second)));

                    vsl_b_write(os, static_cast<double>(static_cast<int>(coords[countI].first*1)));
                    vsl_b_write(os, static_cast<double>(static_cast<int>(coords[countI].second*1)));


                }
                else {
                    if (countI < 111050) {
                        vcl_cout << "x: " << coords[countI].first << " y: " << coords[countI].second << vcl_endl;
                    }
                    vsl_b_write(os, static_cast<double>(rand()%234));
                    vsl_b_write(os, static_cast<double>(rand()%180));
                }
            }

            
            //vsl_b_write(os, coords[countI].first);
            //vsl_b_write(os, static_cast<double>(rand()%180));
            
            
            
            //vsl_b_write(os, tempPoly[0][countI].x());
            //vsl_b_write(os, tempPoly[0][countI].y());
        }
    
    }

    
    vsl_b_write(os, image.ni());
    vsl_b_write(os, image.nj());

    vsl_b_write(os, labeled_image);
    vsl_b_write(os, image);
    */
  
}

//: Binary load self from stream. (not typically used)
template <class T>
void dbseg_seg_object<T>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
        int nplanes;
        vsl_b_read(is, nplanes);

        int numIDs;
        vsl_b_read(is, numIDs);
        list<int> validIDs;
        idCount = 0;
        vcl_vector<int> validIDsVector;
        vsl_b_read(is, validIDsVector);
        for (int j = 0; j < numIDs; j++) {
            int tempID=0;
            if (!is.is()) {
                vcl_cout << "break";
            }
            //vsl_b_read(is, tempID);
            tempID = validIDsVector[j];
            if (tempID > idCount) {
                idCount = tempID;
            }
            validIDs.push_back(tempID);
        }
        idCount++;
        
        vector<int> parent_list;
        vector<list<int>> children_list;
        vector<dbseg_seg_spatial_object<T>*> object_list;
        vector<list<int>> neighbor_list;

        parent_list.resize(idCount);
        children_list.resize(idCount);
        object_list.resize(idCount);
        neighbor_list.resize(idCount);


        list<int>::iterator i;

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempParent;
            vsl_b_read(is, tempParent);
            parent_list[*i] = tempParent;
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempChildCount;
            vsl_b_read(is, tempChildCount);
            for (int j = 0; j < tempChildCount; j++) {
                int tempChild;
                vsl_b_read(is, tempChild);
                children_list[*i].push_back(tempChild);
            }
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempNeighborCount;
            vsl_b_read(is, tempNeighborCount);
            for (int j = 0; j < tempNeighborCount; j++) {
                int tempNeighbor;
                vsl_b_read(is, tempNeighbor);
                neighbor_list[*i].push_back(tempNeighbor);
            }
        }

        int maxDepth;
        vsl_b_read(is, maxDepth);
        vector<list<int>> depth_list;
        depth_list.resize(maxDepth+1);

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {

            



            int depth, bottom, top, left, right;
            vector<T> dataL;
            dataL.resize(nplanes);
            vsl_b_read(is, depth);
            vsl_b_read(is, bottom);
            vsl_b_read(is, top);
            vsl_b_read(is, left);
            vsl_b_read(is, right);
            for (int k = 0; k < nplanes; k++) {
                
                vsl_b_read(is, dataL[k]);
                T testcheck = dataL[k];
            }
            set<int> contained;
            int tempNumContained;
            vsl_b_read(is, tempNumContained);
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                int tempCont;
                vsl_b_read(is, tempCont);
                contained.insert(tempCont);
            }


            list<vgl_polygon<double>*> contours;

            //vsl_b_read(is, tempPoly);
            //contours.push_back(new vgl_polygon<double>(tempPoly));
            for (int ii = 0; ii < tempNumContained+1; ii++) {
                int count;
                vsl_b_read(is, count);
                vector<pair<double, double>> tempVector;
                vgl_polygon<double> tempPoly;
                tempPoly.clear();
                tempPoly.new_sheet();
                for (int countI = 0; countI < count; countI++) {
                    double tempX = 0;
                    double tempY = 0;

                    vsl_b_read(is, tempX);
                        if (!is) {
                            vcl_cout << "not IS" << vcl_endl;
                        }

                    vsl_b_read(is, tempY);
                        if (!is) {
                            vcl_cout << "not IS" << vcl_endl;
                        }
                    //tempX /= 100;
                    //tempY /= 100;
                    tempVector.push_back(pair<double, double>(tempX, tempY));
                    tempPoly.push_back(tempX, tempY);
                }

                contours.push_back(new vgl_polygon<double>(tempPoly));
            }


            /*for (int j2 = 0; j2 < contained.size()+1; j2++) {
                vgl_polygon<double> tempPoly;
                tempPoly.clear();
                tempPoly.new_sheet();
                //vsl_b_read(is, tempPoly);
                if (!is) {
                    vcl_cout << "not IS" << vcl_endl;
                }
                int tempPolySize;
                vsl_b_read(is, tempPolySize);
                for (int j3 = 0; j3 < tempPolySize; j3++) {
                    int tempX = 0;
                    int tempY = 0;
                    if (!is) {
                        vcl_cout << "not IS" << vcl_endl;
                    }
                    vsl_b_read(is, tempX);
                    if (!is) {
                        vcl_cout << "not IS" << vcl_endl;
                    }

                    vsl_b_read(is, tempY);
                    tempPoly.push_back(tempX, tempY);
                    //error, temp Y is 9.593574113109e-311#DEN
                }
                contours.push_back(new vgl_polygon<double>(tempPoly));
            }
            */
            /*
            vgl_polygon<double> tempPoly;
            tempPoly.clear();
            tempPoly.new_sheet();
            int tempX1 = 0;
            int tempX2 = 0;
            int tempY1 = 0;
            int tempY2 = 0;    
            if (!is) {
                vcl_cout << "not IS 1" << vcl_endl;
            }
            vsl_b_read(is, tempX1);
            if (!is) {
                vcl_cout << "not IS 2" << vcl_endl;
            }
            vsl_b_read(is, tempY1);
            if (!is) {
                vcl_cout << "not IS 3" << vcl_endl;
            }
            vsl_b_read(is, tempX2);
            if (!is) {
                vcl_cout << "not IS 4" << vcl_endl;
            }
            vsl_b_read(is, tempY2);
            if (!is) {
                vcl_cout << "not IS 5" << vcl_endl;
            }
            tempPoly.push_back(tempX1, tempY1);
            tempPoly.push_back(tempX2, tempY2);
    
            contours.push_back(new vgl_polygon<double>(tempPoly));
            */

/*            
            int tempNumOuterPoints;
            vsl_b_read(is, tempNumOuterPoints);
            list<vgl_polygon<double>*> contours;
            vgl_polygon<double> tempPoly;
            tempPoly.new_sheet();
            for (int j2 = 0; j2 < tempNumOuterPoints; j2++) {
                double tempX, tempY;
                vsl_b_read(is, tempX);
                vsl_b_read(is, tempY);
                tempPoly.push_back(tempX, tempY);
            }
            contours.push_back(new vgl_polygon<double>(tempPoly));
            
            set<int> contained;
            int tempNumContained;
            vsl_b_read(is, tempNumContained);
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                int tempContained;
                vsl_b_read(is, tempContained);
                contained.insert(tempContained);
            }
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                tempPoly.clear();
                tempPoly.new_sheet();
                int tempNumInnerPoints;
                vsl_b_read(is, tempNumInnerPoints);
                for (int j3 = 0; j3 < tempNumInnerPoints; j3++) {
                    double tempX, tempY;
                    vsl_b_read(is, tempX);
                    vsl_b_read(is, tempY);
                    tempPoly.push_back(tempX, tempY);
                }
                contours.push_back(new vgl_polygon<double>(tempPoly));
                
    
            }
    */
            object_list[*i]=(new dbseg_seg_spatial_object<T>(*i, depth, bottom, top, left, right, dataL, contained, contours));
            depth_list[depth].push_back(*i);

        }

        int ni, nj;
        vsl_b_read(is, ni);
        vsl_b_read(is, nj);

        vsl_b_read(is, labeled_image);
        vsl_b_read(is, image);
        
        /*labeled_image = vil_image_view<int>(ni, nj, 1);
        image = vil_image_view<T>(ni, nj, nplanes);

        for (int x = 0; x < ni; x++) {
            for (int y = 0; y < nj; y++) {
                int tempLabel;
                vsl_b_read(is, tempLabel);
                labeled_image(x,y) = tempLabel;
            }
        }

        for (int j = 0; j < nplanes; j++) {
            for (int x = 0; x < ni; x++) {
                for (int y = 0; y < nj; y++) {
                    T tempValue;
                    vsl_b_read(is, tempValue);
                    image(x,y,j) = tempValue;
                }
            }
        }*/
        
        totalPixels = 0;
        neighborhood = new dbseg_seg_neighborhood(neighbor_list);
        tree = new dbseg_seg_tree<T>(validIDs, object_list, parent_list, children_list, depth_list);
    }
    
   else {
    vcl_cerr << "dbseg_seg_object: unknown I/O version " << ver << '\n';
   }
  
}

//: Return IO version number;
template <class T>
short dbseg_seg_object<T>::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
template <class T>
void dbseg_seg_object<T>::print_summary(vcl_ostream &os) const
{
  //os << *this;
}

//: Binary save dbseg_seg_object to stream.
template<class T>
void
vsl_b_write(vsl_b_ostream &os, const dbseg_seg_object<T>* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load dbsta_gaussian_sphere from stream.
template<class T>
void
vsl_b_read(vsl_b_istream &is, dbseg_seg_object<T>* &p)
{
  //delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new dbseg_seg_object<T>();
    p->b_read(is);
  }
  else
    p = 0;
}


#define DBSEG_SEG_OBJECT_INSTANTIATE(T) \
template class dbseg_seg_object<T>;\
template class dbseg_seg_spatial_object<T>;\
template class dbseg_seg_tree<T>;\
template void vsl_b_read(vsl_b_istream &, dbseg_seg_object<T> *&); \
template void vsl_b_write(vsl_b_ostream &, const dbseg_seg_object<T> *); \


#endif // dbseg_seg_object_txx_


