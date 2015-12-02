/********************************************************************************************
 * File: Silhouette.cpp
 * ----------------------
 * v.1 2015/11/15
 * Programm gets white-black image (GIF, PNG, JPEG, BMP),
 * and discovers what object on it
 * are actually could be human bodies silhouettes.
 * Shows quantity of humans wich have
 * been found.
 ********************************************************************************************/

#include <iostream>
#include <stdlib.h>
#include "myPQueue.h"
#include "myVector.h"
#include "gbufferedimage.h"
#include "filelib.h"
#include "simpio.h"

using namespace std;

/* Declarations
 * -----------------------------------------------------------------------------------------*/

/* Type: Pt
 * ----------
 * Combines coordinates of simple image pixel.
 * Pt - reduction of "point".*/
struct Pt{
    int x;
    int y;
    bool operator==(const Pt & n1){
             return (x == n1.x) && (y == n1.y);
    }
};

/* Global variables
 * -----------------
 * unionColor - integer of color for discovered
 * points union on the image */
int UNION_COLOR;
/* weakObject - minimal valid size in pixels for
 * discovered points union on the image  */
int MINIMAL_SIZE;
/* notHumanObjectColor - integer of color for
 * objects which aren't defined as human silhouette   */
int NOT_HUMAN_OBJECT_COLOR;
/* img - main program image  */
GBufferedImage* img;
/* Main image dimenisions. Are set global to
 * reduce programm calls */
int imgHeight;
int imgWidth;

/* Global constatnts
 * -------------------
 * Typical colors values in programm */
int const WHITE = 0xffffff;
int const GREEN = 0x00ff00;
int const BLUE = 0x0000ff;
int const BLACK = 0x000000;
int const PERIMETR_COLOR = WHITE;
/* FILTER_COLOR_LIMIT - some koef to make start image biColor */
int const FILTER_COLOR_LIMIT = 4000000;
/* SHRINK_KOEF - some koef to shrink humans bodies in effective way  */
int const SHRINK_KOEF = 30;//Useful range is 20 - 40;


/* -----------------------------------------------------------------------------------------//
 * Implementation section
 * -----------------------------------------------------------------------------------------*/

/* Function: fileInput()
 * ----------------------------
 * Provides user file input.
 * Defines if this file exist in project directory */
string fileInput(string promptText){
    /* Until user dosen't make valid input - ask him */
    string result = "";
    while(true){
        result = getLine(promptText);
        if(fileExists(result)){
            break;
        }else{
            cout << "Unfortunately your input is failed" << endl;
        }
    }
    return result;
}

/* Function: adjustMainWindow()
 * ----------------------------
 * Makes main program preparations - loads image, sets
 * main global variables */
void adjustMainWindow(GWindow& gw, string inputFile){
    img = new GBufferedImage(800,800, 0);
    img->load(inputFile);
    imgHeight = img->getHeight();
    imgWidth = img->getWidth();
    img->resize(imgWidth, imgHeight);
    gw.setSize(imgWidth, imgHeight);
    gw.add(img, 0, 0);

    MINIMAL_SIZE = ((imgWidth * imgHeight)/1000);//Size for garbage objects
    NOT_HUMAN_OBJECT_COLOR = BLUE;               //To show objects discovered as not human
}

/* Function: filterImage()
 * ------------------------
 * Makes param image biColor. */
void filterImage(GBufferedImage* img){
    /* -----------------------------------------------------------*/
    cout << "       - FILTER IMAGE INTO BLACK-WHITE PIXELS" << endl;
    /* -----------------------------------------------------------*/
    int height = img->getHeight();
    int width = img->getWidth();
    for(int row = 0; row < (height); row++){
        for(int col = 0; col < (width); col++){
            int color = img->getRGB(col, row);
            if((color < FILTER_COLOR_LIMIT)){
            /* Each dark cell set as black */
                img->setRGB(col, row, 0x0);
            }else{
            /* Other cells set as white */
                img->setRGB(col, row, WHITE);
            }
        }
    }
}

/* Function: detectUnion()
 * -----------------------
 * Returns vector of pixels, which are not white
 * on the main program image, and
 * which create single union object with
 * input param point. The function checks each pixel
 * around param pixel. If pixel is not white -
 * function adds it to  to result vector.
 *
 * @param row, col  pixel where first not white pixel is obtained  */
MyVector<Pt> detectUnion(int row, int col){
    MyVector<Pt> result;
    /* The first cell in result vector is param point */
    Pt pt;
    pt.x = col;
    pt.y = row;
    MyPQueue<Pt> pointsQueue;//MyPQueue is priority queue class
    MyVector<Pt> queueList;
    pointsQueue.enqueue(pt,0);//"0" priority is assigned to every input object in queue
    queueList.add(pt);
    /* Finding of around cells process */
    while(!pointsQueue.isEmpty()){
        pt = pointsQueue.dequeueMin();
        queueList.removeValue(pt);
        if(!result.isContains(pt)){
            result.add(pt);          
            img->setRGB(pt.x, pt.y, UNION_COLOR);//Every pixel from queue is already validated
            /* Main around cells checking */
            int x = pt.x;
            int y = pt.y;
            if((x > 0) && (x < (imgWidth - 1))){
                if((y > 0) && (y < (imgHeight - 1))){
                    for(int i = x-1; i <= x+1; i++){
                        for(int u = y-1; u <= y+1; u++){
                            int cellColor = img->getRGB(i, u);
                            if((cellColor != UNION_COLOR) && (cellColor != WHITE)){
                            /* This pixel is new part for current union */
                                Pt objPoint;
                                objPoint.x = i;
                                objPoint.y = u;
                                if(!result.isContains(objPoint)){
                                    if(!queueList.isContains(objPoint)){
                                        pointsQueue.enqueue(objPoint, 0);
                                        queueList.add(objPoint);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

/* Function: detectImageUnions()
 * ------------------------------
 * Makes single iteration through program image, and
 * finds not white unions on it. Adds detected unions to
 * param vector.
 *
 * @param unionsTable   Image objects storage vector   */
void detectImageUnions (MyVector<MyVector<Pt>>& unionsTable){
    /* ------------------------------------------------*/
    cout << "       - ASSIGN PIXELS TO OBJECTS" << endl;
    /* ------------------------------------------------*/
    /* Main iteration through image */
    for(int row = 0; row < (imgHeight); row++){
        for(int col = 0; col < (imgWidth); col++){
            int cellColor = img->getRGB(col, row);
            if((cellColor != UNION_COLOR) && (cellColor != WHITE)){
                 /* If the first pixel of possible object is detected  - check all her
                  * around pixels  */
                 MyVector<Pt> imgObject = detectUnion(row, col);
                 if(imgObject.size() > MINIMAL_SIZE){
                     unionsTable.add(imgObject);
                 }else{
                     /* Erase the smallest objects */
                     for(int u = 0; u < imgObject.size(); u++){
                         Pt i = imgObject[u];
                         img->setRGB(i.x, i.y, WHITE);
                     }
                 }
            }
        }
    }
    /* -------------------------------------------------*/
    cout << "       - UNIONS BEFORE HUMAN-PROPORTIONS CHECKING = "
         << unionsTable.size() << endl;
    /* -------------------------------------------------*/
}

/* Function: findMaxes()
 * -----------------------
 * Finds edges of the param object, and store their values into
 * param references.
 *
 * @param min, max   at this references obtained edges values will be load
 * @param objectVec  object to process  */
void findMaxes(int& min_X, int & max_X, int& min_Y, int& max_Y, MyVector<Pt>& objectVec){
    /* Initiates params values by some point values */
    for(int u = 0; u < objectVec.size(); u++){
        Pt i = objectVec[u];
        min_X = i.x;
        min_X = i.x;
        min_Y = i.y;
        max_Y = i.y;
        break;
    }
    /* Finds max and min edges values through the object */
    for(int u = 0; u < objectVec.size(); u++){
        Pt i = objectVec[u];
        if(i.x < min_X){
            min_X = i.x;
        }
        if(i.x > max_X){
            max_X = i.x;
        }
        if(i.y < min_Y){
            min_Y = i.y;
        }
        if(i.y > max_Y){
            max_Y = i.y;
        }
    }
}

/* Function: getMiddleWidth()
 * ---------------------------
 * Returns width of the param object on the level of his middle_Y
 * coordinate ("waistline")
 *
 * @param middle_Y   average middle Y coordinate of the param object
 * @param objectVec  object to process  */
int getMiddleWidth(int middle_Y, MyVector<Pt> objectVec){
    int result = 0;
    int middleMin_X = 0;
    int middleMax_X = 0;
    /* Initiates max values by first obtained point values */
    for(int u = 0; u < objectVec.size(); u++){
        Pt i = objectVec[u];
        if(i.y == middle_Y){
            middleMin_X = i.x;
            middleMax_X = i.x;
            break;
        }
    }
    /* Finds edges on object the middle Y level */
    for(int u = 0; u < objectVec.size(); u++){
        Pt i = objectVec[u];
        if(i.y == middle_Y){
            if(i.x < middleMin_X){
                middleMin_X = i.x;
            }
            if(i.x > middleMax_X){
                middleMax_X = i.x;
            }
        }
    }
    result = middleMax_X - middleMin_X;
    return result;
}

/* Function: repaintPerimetrColor()
 * --------------------------------
 * Repaint object perimeter pixels in PERIMETR_COLOR
 * for 1 time due to simple condition: if there are other
 * colors cells around current cell - it's perimeter cell.
 *
 * @param objectVec  object to process  */
void repaintPerimetrColor(MyVector<Pt>& objectVec){
    MyVector<Pt> perimeter;
    for(int u = 0; u < objectVec.size(); u++){
        Pt point = objectVec[u];
        int x = point.x;
        int y = point.y;

        int pointColor = img->getRGB(x, y);
        if((x > 0) && (x < (img->getWidth() - 1))){
            if((y > 0) && (y < (img->getHeight() - 1))){
                for(int i = x-1; i <= x+1; i++){
                    for(int u = y-1; u <= y+1; u++){                       
                        if(img->getRGB(i, u) != pointColor){
                            if(!perimeter.isContains(point)){
                                    perimeter.add(point);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    /* Repaint all perimetr cells */
    for(int u = 0; u < perimeter.size(); u++){
        Pt i = perimeter[u];

        img->setRGB(i.x, i.y, PERIMETR_COLOR);
        objectVec.removeValue(i);
    }
}

/* Function: shrinkAllUnions()
 * --------------------------
 * For every object from input param vector chooses allowable
 * level of object  shrinking and calls repaintPerimetrColor()
 * to repaint object perimetr into another color - white
 * for example.
 *
 * @param unionsTable  vector of objects to process  */
void shrinkAllUnions(MyVector<MyVector<Pt>>& unionsTable){
    /* -------------------------------------------------*/
    cout << "       - SHRINK OBJECTS PERIMETERS" << endl;
    /* -------------------------------------------------*/
    for(int i = 0; i < unionsTable.size(); i++){
        MyVector<Pt> objectVec = unionsTable[i];
        int min_X = 0;
        int max_X = 0;
        int min_Y = 0;
        int max_Y = 0;
        findMaxes(min_X, max_X, min_Y, max_Y, objectVec);
        int objectHeight = max_Y - min_Y;
        /* Calculate count of shrinks for this object due to some human body height  */
        int perimetrShrinks = (objectHeight/SHRINK_KOEF) - 1;//absolutely empirical formula
        for(int u = 0; u < perimetrShrinks; u++){
            repaintPerimetrColor(objectVec);
        }
    }
}

/* Function: silhouettesCounting()
 * --------------------------
 * Returns quantity of objects, which could be
 * human silhouettes on the image. Belive that object is human
 * if it's waistline isn't bigger then half of it's height.
 * Too "low" objects are considered as garbage and erased.
 * Supose that all humans are pictured in the same scale.
 *
 * @param unionsTable  vector of objects to process  */
int silhouettesCounting(MyVector<MyVector<Pt>> unionsTable){
    int result = 0;//Total quantity of humans on the image
    int maxImageHeight = 0;//The highest object height
    for(int i = 0; i <unionsTable.size(); i++){
        MyVector<Pt> object = unionsTable[i];
        int min_X, max_X, min_Y, max_Y;
        min_X = max_X = min_Y = max_Y = 0;
        findMaxes(min_X, max_X, min_Y, max_Y, object);
        int objectHeight = max_Y - min_Y;//Current object height
        if(maxImageHeight < objectHeight){
            maxImageHeight = objectHeight;//Update highest object height
        }
        int middle_Y = min_Y + (objectHeight/2);//Middle of the "human body"
        /* Find out length of waistline of the "human body" */
        int middleWidth = getMiddleWidth(middle_Y, object);
        /* Main "human" proportion evaluation:
         * - if waistline isn't bigger then half of it's height - belive this is human
         * - supose, that minimal human silhouette wouldn't be 5 times lower then
         *   highest object, else - it is erased as garbage */
        if(((objectHeight / 2)  > middleWidth) && (objectHeight > (maxImageHeight/5))){
                result++;//Apreciate current object as "human body silhouette"
        }else{//If object haven't matched propotions conditions - it's erased from image
            for(int u = 0; u < object.size(); u++){
                Pt i = object[u];
                /* Repaint no human objects into garbageColor */
                img->setRGB(i.x, i.y, NOT_HUMAN_OBJECT_COLOR);
            }
        }
    }
    /* -------------------------------------------------*/
    cout << "       - HUMANS QUANTITY DUE TO PROPORTIONS = "
         << result << endl;
    /* -------------------------------------------------*/
    return result;
}


int main(){  
    /* --------------------------------------------------------------------------*/
    //string inputFile = "free-vector.jpg";//Enter your image file here
    /* --------------------------------------------------------------------------*/
    string inputFile = fileInput("Enter your image file here: ");

    GWindow gw;
    adjustMainWindow(gw, inputFile);//Main window and program settings

    cout << "PROCESSING..." << endl;
    cout << "==========================================================" << endl;
    filterImage(img);

    UNION_COLOR = GREEN;//To show union detection processing
    MyVector<MyVector<Pt>> unionsTable1;
    detectImageUnions(unionsTable1);
    int assign1 = silhouettesCounting(unionsTable1);

    shrinkAllUnions(unionsTable1);

    UNION_COLOR = BLACK;//To return black-white picture from function
    MyVector<MyVector<Pt>> unionsTable2;
    detectImageUnions(unionsTable2);
    int assign2 = silhouettesCounting(unionsTable2);

    int averageQty = (assign1 + assign2) / 2;

    cout << "==========================================================" << endl;
    cout << "PROGRAM IS FINISHED. AVERAGE HUMANS QUANTITY IS: " << averageQty << endl;

    return 0;
}
