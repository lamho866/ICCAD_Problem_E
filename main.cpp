#include <bits/stdc++.h>
using namespace std;

void readLine(char *s){
    char tap[20];
    double x1, x2, y1, y2;
    sscanf(s + 5,"%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2);
    printf("line: %lf,%lf,%lf,%lf\n", x1, y1, x2, y2);
}

void readarc(char *s){
    char tap[20];
    double x1, x2, y1, y2, rx, ry;
    sscanf(s + 4,"%lf,%lf,%lf,%lf,%lf,%lf,%s", &x1, &y1, &x2, &y2, &rx, &ry, tap);
    printf("arc: %lf,%lf,%lf,%lf,%lf,%lf, %s\n", x1, y1, x2, y2, rx, ry, tap);
}



class Line
{
public:
    bool isLine, isCW; //CW or CCW 
    double x1, y1, x2, y2, rx, ry;
    double slope;
    char tap[5];

    Line(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx = 0.0, double _ry = 0.0, bool _isCW = false)
     : isLine(_isLine), x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW)
     {
         //if the line is horizontal slope is -1
         if(x1 == x2) slope = -1.0;
         else slope = (y1 - y2) / (x1 - x2);

     }


    double dist(){
        return sqrt((x1 - x2)*(x1 - x2) + (y1 -y2)*(y1 - y2));
    }
};

bool isTurnLeft(Line a, Line b){
    double OA_x = a.x2 - a.x1, OA_y = a.y2 - a.y1;
    double OB_x = b.x2 - a.x1, OB_y = b.y2 - a.y1;
    return (OA_x * OB_y - OB_x * OA_y) >= 0;
}

void connectAB(Line &a, Line &b){
    double a1, b1, c1, a2, b2, c2;
    double D, Dx, Dy;
    a1 = a.y1 - a.y2, b1 = -a.x1 + a.x2;
    a2 = b.y1 - b.y2, b2 = -b.x1 + b.x2;
    c1 = a1 * a.x1 + b1 * a.y1;
    c2 = a2 * b.x1 + b2 * b.y1;

    D = a1 * b2 - a2 * b1;
    Dx = c1 * b2 - c2 * b1;
    Dy = a1 * c2 - a2 * c1;

    a.x2 = b.x1 = Dx / D;
    a.y2 = b.y1 = Dy / D;
}


class Polygom{
public:
    vector<Line> shape;
    void addLine(char *s){
        char tap[20];
        double x1, x2, y1, y2, rx, ry;
        if(s[0] == 'l'){
            //isLine
            sscanf(s + 5,"%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2);
            shape.push_back(Line(true, x1, y1, x2, y2));
            printf("line: %lf,%lf,%lf,%lf\n", x1, y1, x2, y2);
            
        }
        if(s[0] == 'a'){
            //isCycle
            sscanf(s + 4,"%lf,%lf,%lf,%lf,%lf,%lf,%s", &x1, &y1, &x2, &y2, &rx, &ry, tap);
            shape.push_back(Line(false, x1, y1, x2, y2, rx, ry, strcmp(tap, "CW") == 0));
            printf("arc: %lf,%lf,%lf,%lf,%lf,%lf, ", x1, y1, x2, y2, rx, ry);
            if(strcmp(tap, "CW") == 0) printf("CW\n");
            else printf("CWW\n");
        }
    }
    Polygom(){}
};

void addOutLine(Polygom &outline, Line &a, Line &b, double assemblygap){
    double addX, addY;
    if(isTurnLeft(a, b)){
        //CW
        addX = (a.y2 - a.y1) / a.dist() * assemblygap;
        addY = -(a.x2 - a.x1) / a.dist() * assemblygap;
    }else{
        //CWW
        addX = -(a.y2 - a.y1) / a.dist() * assemblygap;
        addY = (a.x2 - a.x1) / a.dist() * assemblygap;
    }
    outline.shape.push_back(Line(true, a.x1 + addX, a.y1 + addY, a.x2 + addX, a.y2 + addY));
}

void printPolyShape(Polygom &poly){
    printf("x = [");
    for(int i = 0; i < poly.shape.size(); ++i)
        printf("%lf, %lf, ", poly.shape[i].x1, poly.shape[i].x2);
    printf("]\ny = [") ;
    for(int i = 0; i  < poly.shape.size(); ++i)
        printf("%lf, %lf, ", poly.shape[i].y1, poly.shape[i].y2);
    printf("]\n");
}

void drawThePolygonOffsetting(Polygom &assembly, double const assemblygap){
    Polygom outline;

    for(int i = 0; i < assembly.shape.size() - 1; ++i)
        addOutLine(outline, assembly.shape[i], assembly.shape[i + 1], assemblygap);
    addOutLine(outline, assembly.shape[assembly.shape.size() - 1], assembly.shape[0], assemblygap);

    //connect the line
    for(int i = 0; i < outline.shape.size() - 1; ++i)
        connectAB(outline.shape[i], outline.shape[i + 1]);
    connectAB(outline.shape[outline.shape.size() - 1], outline.shape[0]);

    printPolyShape(outline);
}

int main(){
    Polygom assembly;
    vector<Polygom> cropperList;
    int cropSize = 0;

    char str[256];
    double assemblygap, croppergap, silkscreenlen;
    cin >> str;
    sscanf(str + 12, "%lf", &assemblygap);
    cin >> str;
    sscanf(str , "%lf", &croppergap);
    cin >> str;
    sscanf(str, "%lf", &silkscreenlen);
    cin >> str;
    //read assembly
    printf("assembly\n");
    while(1){
        cin >> str;
        if(str[0] == 'c') break;
        assembly.addLine(str); 
    }
    
    //read cropperList
    printf("copper\n");
    cropperList.push_back(Polygom());
    while(cin >> str){
        if(str[0] == 'c'){
            printf("copper\n");
            cropperList.push_back(Polygom());
            cropSize++;
            continue;
        }
        cropperList[cropSize].addLine(str);
    }

    printPolyShape(assembly);
    printf("assemblygap : %lf\n", assemblygap);
    drawThePolygonOffsetting(assembly, assemblygap);
}