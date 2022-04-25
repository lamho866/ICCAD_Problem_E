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
    char tap[5];

    Line(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx = 0.0, double _ry = 0.0, bool _isCW = false)
     : isLine(_isLine), x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW)
     {}
     
};

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


int main(){
    Polygom assembly;
    vector<Polygom> cropperList;
    int cropSize = 0;

    char str[256];
    double assemblygap, croppergap, silkscreenlen;
    cin >> str;
    sscanf(str , "%lf", &assemblygap);
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
}