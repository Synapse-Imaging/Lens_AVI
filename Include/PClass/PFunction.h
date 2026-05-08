#ifndef _PFUNCTION_H_
#define _PFUNCTION_H_

#define IS_SETFLAG(state, flag) ((state & flag) == flag)

#define P_PI 3.1415926535
#define CONV_RADIAN(x) x * P_PI / 180
#define CONV_DEGREE(x) x * 180 / P_PI

extern "C"
{
 _declspec(dllimport) void PConv0360Angle(double *dpAngle);
 _declspec(dllimport) void PConvM180P180Angle(double *dpAngle);
 _declspec(dllimport) double PGetAngle(double dCX, double dCY, double dX, double dY, bool bReverse);
 _declspec(dllimport) void PGetRotatePoint(double dAngle, double dCX, double dCY, double *dpX, double *dpY);

 _declspec(dllimport) int PGetProgressPos();
 _declspec(dllimport) void PSetProgressPos(int iPos);
}


struct DPOINT
{
  double x;
  double y;

  void Reset()
  {
	  x = -1;
	  y = -1;
  };

  BOOL operator==(const DPOINT& dOthers) const
  {
    return (this->x == dOthers.x) && (this->y == dOthers.y);
  };

  DPOINT& operator+(const DPOINT& dOthers) const
  {
    DPOINT dResult;
    dResult.x = this->x + dOthers.x;
    dResult.y = this->y + dOthers.y;
    return dResult;
  };

  DPOINT& operator-(const DPOINT& dOthers) const
  {
    DPOINT dResult;
    dResult.x = this->x - dOthers.x;
    dResult.y = this->y - dOthers.y;
    return dResult;
  };
};

//struct DRECT
//{
//  double left;
//  double top;
//  double right;
//  double bottom;
//};

#endif;
