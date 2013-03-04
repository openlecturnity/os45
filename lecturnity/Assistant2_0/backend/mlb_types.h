/*********************************************************************

 program : mlb_types.h
 authors : Gabriela Maass
 date    : 18.02.2000
 desc    : 

 **********************************************************************/



#ifndef __ASSISTANT_TYPES__
#define __ASSISTANT_TYPES__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif
 
typedef unsigned int        uint32;


namespace ASSISTANT {
  //using namespace std;
  
  class Action {
  public:
      enum type {
	      BEGIN = 0,
         CONTINUE = 1,
	      END = 2,
	      SHOWLINE = 3,
         FINISH = 4
      };
  }; 

  class Point {
  public:
    double x;
    double y;
    int nr;

    Point () {}
    Point(double _x, double _y) {x = _x; y = _y; nr=-1;}
    void Set(double _x, double _y) {x = _x; y = _y; nr=-1;}
  };
  
}

#endif
