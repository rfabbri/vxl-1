#ifndef POWELLS_ALGORITHM_H
#define POWELLS_ALGORITHM_H
#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vcl_vector.h>


namespace  Powell_NS
  {
    template<int NumDim>
            class DirectionMatrix
              {
                private:
                    vcl_vector<vcl_vector<double> > _matrix;
                    void _allocateMatrix()
                      {
                        assert(_matrix.size()==NumDim);

                        for (int i=0;i<NumDim;i++)
                          {
                            for (int j=0;j<NumDim;j++)
                                _matrix[i].push_back(0);
                           }
                       }

                public:
                    typedef vcl_vector<double>  DirectionVectorType;

                    DirectionMatrix():_matrix(NumDim)
                      {
                        _allocateMatrix();
                        reset();
                       }

                    void getDirectionVector(int dir, vcl_vector<double> &dir_vec) const
                      {
                        assert(dir<NumDim);
                        assert(dir>=0);
                        assert(dir_vec.size()==NumDim);

                        for (int i=0;i<NumDim;i++)
                          {
                            dir_vec[i]=_matrix[dir][i];
                           }
                       }


                    void setDirectionVector(int dir_to_change, int dir_copy_source)
                      {
                        assert(dir_to_change<NumDim);
                        assert(dir_to_change>=0);

                        assert(dir_copy_source<NumDim);
                        assert(dir_copy_source>=0);

                        for (int i=0;i<NumDim;i++)
                          {
                            _matrix[dir_to_change][i]=_matrix[dir_copy_source][i];
                           }
                       }


                    void setDirectionVector(int dir, const vcl_vector<double> &dir_vec)
                      {
                        assert(dir<NumDim);
                        assert(dir>=0);
                        assert(dir_vec.size()==NumDim);

                        for (int i=0;i<NumDim;i++)
                          {
                            _matrix[dir][i]=dir_vec[i];
                           }
                       }


                    void reset()
                      {

                        for (int i=0;i<NumDim;i++)
                          {
                            for (int j=0;j<NumDim;j++)
                                _matrix[i][j]=0;
                       
                            _matrix[i][i]=1;
                           }
                       }





                    vcl_string printMatrix()
                      {
                        vcl_ostringstream ostrm;

                        ostrm<<vcl_endl;
                        for (int i=0;i<NumDim;i++)
                          {
                            ostrm<<"[ ";
                            for (int j=0;j<NumDim;j++)
                              {
                                ostrm<<setprecision(5)<<setw(7)<<_matrix[i][j]<<"  ";
                               }
                            ostrm<<"] ";
                            ostrm<<vcl_endl;
                           }
                        ostrm<<vcl_endl;
                        return ostrm.str();
                       }

                    vcl_string printVector(int i)
                      {
                        assert(i<NumDim);
                        assert(i>=0);

                        vcl_ostringstream ostrm;

                        ostrm<<"[ ";
                        for (int j=0;j<NumDim;j++)
                          {
                            ostrm<<setprecision(5)<<setw(7)<<_matrix[i][j]<<"  ";
                           }
                        ostrm<<"] ";
                        ostrm<<vcl_endl;
                        return ostrm.str();
                       }
               };


    /***
     * The following functions have been adapted from : "Numerical Recipes in C".
     * So you know whom to blame when you come across one character variable names.
     * */

    /*
     * Takes a N-Dim function and creates a 1-D function out of it.
     * */


    /**
     * Make class LineOptimizer  a friend of this class and make the constructor private.
     * */
    template<typename PointType, typename DirectionVectorType, typename FunctionType>
            class SingleDimFunction
              {
                private:
                    PointType             *_currentPt;
                    DirectionVectorType   *_currentDir;
                    FunctionType          *_energyFunction;
                    int                    _evalCount;



                public:
                    SingleDimFunction():_currentPt(0), _currentDir(0), _energyFunction(0),_evalCount(0)
                      {
                       };

                    ~SingleDimFunction(){vcl_cout<<" SingleDimFunction()   : Evaluations = "<<_evalCount<<vcl_endl;};

                    void setParams(PointType  *pt, DirectionVectorType *dir_vec, FunctionType* func)
                      {
                        _currentPt       =  pt;
                        _currentDir      =  dir_vec;
                        _energyFunction  =  func;
                       }

                    double operator()(double x)
                      {
                        assert(_currentPt!=0);
                        assert(_currentDir!=0);
                        assert(_energyFunction!=0);

                        _evalCount++;
                        //PointType temp(*(_currentPt)+x*(*_currentDir));
                        PointType temp(*(_currentPt));

                        for (unsigned int i=0;i<_currentDir->size();i++)
                            temp[i] += x*((*_currentDir)[i]);

                        return ((*_energyFunction)(temp));
                       }
               };


#define SHIFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

    class BrentsMethod
      {
        private:
            const double _itMax;
            const double _cgold;
            const double _zeps;
            int  _callCount;
            int  _itCount;
            //ITMAX=10
        public:
            BrentsMethod(double tiny_val, int brent_it_max):_itMax(brent_it_max), _cgold(0.3819660), _zeps(tiny_val),_callCount(0),_itCount(0)
              {
               };

            ~BrentsMethod(){vcl_cout<<" BrentsMethod()        : Calls = "<<_callCount<<" Iterations = "<<_itCount<<vcl_endl;};
            template<typename SingleDimFunctionType>
                    double operator()(double ax, double bx, double cx, SingleDimFunctionType &single_dim_energy_func, double tol, double *xmin, double current_val)
                      {
                        _callCount++;
                        /*
                         * Given a function f, and given a bracketing triplet of abscissas ax, bx,
                         * cx (such that bx is between ax and cx, andf(bx) is less than both f(ax)
                         * and f(cx)), this routine isolates the minimum to a fractional precision 
                         * of about tol using Brent's method. The abscissa of the minimum is 
                         * returned as xmin, and the minimum function value is returned as brent, 
                         * the returned function value.
                         * */
                        int iter;
                        double a,b,d=0,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
                        double e=0.0; /*This will be the distance moved on
                                        the step before last.*/

                        a=(ax < cx ? ax : cx); /*a and b must be in ascending order,
                                                 but input abscissas need not be.*/ 
                        b=(ax > cx ? ax : cx);
                        x=w=v=bx; /*Initializations...*/

                        //fw=fv=fx=(single_dim_energy_func)(x, BRENTS_METHOD);
                        fw=fv=fx=current_val;

                        for (iter=1;iter<=_itMax;iter++)
                          { 
                            _itCount++;
                            /*Main program loop.*/
                            xm=0.5*(a+b);
                            tol2=2.0*(tol1=tol*vcl_fabs(x)+_zeps);
                            if (vcl_fabs(x-xm) <= (tol2-0.5*(b-a))) 
                              { 
                                /*Test for done here.*/
                                *xmin=x;
                                return fx;
                               }

                            //vcl_cout<<" It ="<<_itCount<<" x="<<x<<" tol2-0.5*(b-a) ="<<tol2-0.5*(b-a)<<" vcl_fabs(x-xm) ="<<vcl_fabs(x-xm)<<vcl_endl;
                            if (vcl_fabs(e) > tol1) 
                              { 
                                /* Construct a trial parabolic fit. */
                                r=(x-w)*(fx-fv);
                                q=(x-v)*(fx-fw);
                                p=(x-v)*q-(x-w)*r;
                                q=2.0*(q-r);
                                if (q > 0.0) p = -p;
                                q=vcl_fabs(q);
                                etemp=e;
                                e=d;
                                if (vcl_fabs(p) >= vcl_fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
                                    d=_cgold*(e=(x >= xm ? a-x : b-x));
                                /* The above conditions determine the acceptability of the parabolic 
                                   fit. Here we take the golden section step into the larger of the two 
                                   segments.*/
                                else 
                                  {
                                    d=p/q; /*Take the parabolic step.*/
                                    u=x+d;
                                    if (u-a < tol2 || b-u < tol2)
                                        d=PAsign(tol1,xm-x);
                                   }
                               } 
                            else 
                              {
                                d=_cgold*(e=(x >= xm ? a-x : b-x));
                               }
                            u=(vcl_fabs(d) >= tol1 ? x+d : x+PAsign(tol1,d));
                            /* This is the one function evaluation per iteration.*/
                            fu=(single_dim_energy_func)(u);
                            if (fu <= fx) 
                              {
                                /* Now decide what to do with our function evaluation. */
                                if (u >= x) a=x; else b=x;
                                SHIFT(v,w,x,u); /*Housekeeping follows:*/
                                SHIFT(fv,fw,fx,fu);
                               } 
                            else
                              {
                                if (u < x) a=u; else b=u;
                                if (fu <= fw || w == x) 
                                  {
                                    v=w;
                                    w=u;
                                    fv=fw;
                                    fw=fu;
                                   } 
                                else 
                                    if (fu <= fv || v == x || v == w) 
                                      {
                                        v=u;
                                        fv=fu;
                                       }
                               } 
                            /* Done with housekeeping. Back for another iteration.*/
                           }
                        //nrerror("Too many iterations in brent");
                        *xmin=x; /* Never get here.*/
                        return fx;
                       }




            double PAsign(const double a, const double b) const
              {
                if (b>=0)
                    return vcl_fabs(a);
                else
                    return -vcl_fabs(a);
               }



       };



    /**
     * Minimizes a given function along a given line.
     * */
    template<typename BrentsMethodType, typename SingleDimFunctionType>
            class LineOptimizer
              {
                private:
                    const double          _tolerance;
                    SingleDimFunctionType _singleDimFunc;
                    BrentsMethodType      _brentsMethod;
                    int                   _callCount;

                public:
                    LineOptimizer(double tol, double tiny_val, int brent_it_max):_tolerance(tol),_brentsMethod(tiny_val, brent_it_max),_callCount(0)
                      {
                       }

                    ~LineOptimizer(){vcl_cout<<" LineOptimizer()       : Calls = "<<_callCount<<vcl_endl;};

                    template<typename PointType, typename DirectionVectorType, typename FunctionType>
                            void operator()(PointType &current_pt, DirectionVectorType &dir_vec, FunctionType& energy_func, double &current_value)
                              {
                                _callCount++;
                                double ax, xx,bx,xmin;
                                ax=0.0; /*Initial guess for brackets.*/
                                xx=1.0;
                                _singleDimFunc.setParams(&current_pt, &dir_vec, &energy_func);


                                ax=0.5;
                                xx=0;
                                bx=-0.5;

                                //vcl_cout<<" Min Bracketer : Dir = ["<<dir_vec[0]<<", "<<dir_vec[1]<<") "<<vcl_endl;

                                current_value=_brentsMethod(ax,xx,bx,_singleDimFunc,_tolerance,&xmin, current_value);

                                for (unsigned int i=0;i<dir_vec.size();i++) 
                                  { 
                                    /*Construct the vcl_vector results to return.*/
                                    dir_vec[i]    *= xmin;
                                    current_pt[i] += dir_vec[i];
                                   }
                                return;
                               };


               };


    
    /**
     * Things Changed :Wed May 28 11:27:58 EDT 2003
     * Number of iterations. (Powell & Brent (was 10 before)
     * */


    template<typename LineOptimizerType>
            class PowellOptimizer
              {
                private:
                    const double _fractionalTolerance;
                    const double _tiny;
                    const double _itMax;

                    LineOptimizerType _lineOptimizer;
                    int _itCount;
                    int _callCount;




                public:
                    PowellOptimizer(double frac_tol, double tiny_val, int powell_it_max, int brent_it_max):_fractionalTolerance(frac_tol),_tiny(tiny_val), _itMax(powell_it_max),_lineOptimizer(frac_tol, tiny_val, brent_it_max),_itCount(0),_callCount(0)
                      {
                       }

                    ~PowellOptimizer(){vcl_cout<<" PowellOptimizer()     : Calls = "<<_callCount<<" Iterations = "<<_itCount<<vcl_endl;};
                    template<typename PointType, typename DirectionMatrixType, typename FunctionType>
                            void operator()(PointType &current_pt, DirectionMatrixType &dir_matrix, FunctionType& energy_func, double &current_value)
                              {
                                _callCount++;
                                int dimensions = current_pt.getNumDimensions();
                                typename DirectionMatrixType::DirectionVectorType  dir_vec(dimensions);

                                /*
                                 * Save the initial point.
                                 * */ 

                                PointType intial_point(current_pt);
                                PointType point_temp2(current_pt);

                                double func_value_temp1(0);
                                double func_value_temp2(0);
                                int  largest_decrease_index(0);
                                double largest_decrease(0);

                                for (int iteration_num=0;iteration_num<_itMax;++iteration_num) 
                                  {
                                    _itCount++;
                                    func_value_temp1=current_value;
                                    largest_decrease=0;
                                    largest_decrease_index=0;
                                    for (int dir=0;dir<dimensions;dir++) 
                                      { 
                                        /*
                                         * In each iteration, loop over all directions in the set.
                                         * */

                                        dir_matrix.getDirectionVector(dir, dir_vec);
                                        func_value_temp2=current_value;
                                        _lineOptimizer(current_pt, dir_vec, energy_func, current_value); /*minimize along it,*/

                                        if ((func_value_temp2-current_value)> largest_decrease) 
                                          {
                                            /*
                                             * And record it if it is the largest decrease so far. 
                                             * */

                                            largest_decrease        = func_value_temp2-current_value;
                                            largest_decrease_index  = dir;
                                           }
                                       }


                                    if (2.0*(func_value_temp1-current_value) <= _fractionalTolerance*(vcl_fabs(func_value_temp1)+vcl_fabs(current_value))+_tiny) 
                                      {
                                        /*
                                         * Termination criterion.
                                         * */
                                        return;
                                       }

                                    /*
                                     * Construct the extrapolated point and the
                                     * average direction moved. Save the old starting point.
                                     * */
                                    for (int i=0;i<dimensions;i++)
                                      {
                                        point_temp2[i]   =  2.0*current_pt[i]-intial_point[i];
                                       }
                                    current_pt.getDifferenceVector(dir_vec, intial_point);
                                    intial_point  =  current_pt;

                                    func_value_temp2=energy_func(point_temp2); /*Function value at extrapolated point.*/
                                    if (func_value_temp2 < func_value_temp1)
                                      {
                                        if ((2.0*(func_value_temp1-2.0*(current_value)+func_value_temp2)*vcl_sqrt(func_value_temp1-(current_value)-largest_decrease)-largest_decrease*vcl_sqrt(func_value_temp1-func_value_temp2))<0)
                                          {
                                            /*
                                             * Move to the minimum of the new direc-tion, and save the new direction. 
                                             * */ 
                                            _lineOptimizer(current_pt, dir_vec, energy_func, current_value); 
                                            dir_matrix.setDirectionVector(largest_decrease_index, (dimensions-1));
                                            dir_matrix.setDirectionVector((dimensions-1), dir_vec);
                                           }
                                       }
                                    /*Back for another iteration.*/
                                   } 
                               };

               };



#undef SHIFT
   }




#endif   /*POWELLS_ALGORITHM_H*/
