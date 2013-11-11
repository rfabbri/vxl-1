// This is dbrec3d_line_plot.h
#ifndef dbrec3d_line_plot_h
#define dbrec3d_line_plot_h

//:
// \file
// \brief A widget to add a line plot given input data 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  30-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QKeyEvent>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>

class dbrec3d_line_plot: public QwtPlot
{
  Q_OBJECT
public:
  //: Constructor from a vnl_vector. The domain is assumed to be a uncreasing index
  dbrec3d_line_plot(const double* data_block, unsigned size);
  //: Size properties
  virtual QSize sizeHint() const { return QSize(400,400); }
public slots:
  
  //: Draw a mark on the current datapoint (e.g current principal component in being visualized on another widget)
  void set_active_datapoint(unsigned index);
  
  
protected:
  //: Basic plot container
  QwtPlotCurve *main_curve_;
  //: A marker to current datapoint
  QwtPlotMarker *curr_marker_;
  unsigned curr_x_;
  
  //:Data
  const double* yvals_;
  double* xvals_;
  unsigned size_;
  
};

//: A widget that allows you to register line plots and allows user to choose, which curve to plot
class dbrec3d_choose_plot: public QwtPlot
{
  Q_OBJECT
public:
  //: Constructor from a vnl_vector. The domain is assumed to be a uncreasing index
  dbrec3d_choose_plot();
  
  void register_plot(QString const &plot_name, QwtArray<double> const &x_data, QwtArray<double> const &y_data);
  
  void init();
  
  public slots:
  
  //: Draw the next plot
  void draw_next_plot();
  
  virtual QSize sizeHint() const { return QSize(640,480); }

signals:
  void request_next_plot();
 
protected:
  //: Basic plot container
  QwtPlotCurve *main_curve_;

  //:Data
  QMap<QString, QPair<QwtArray<double>, QwtArray<double> > > plots_;
  QMap<QString, QPair<QwtArray<double>, QwtArray<double> > >::const_iterator plots_it_;
  unsigned curr_plot_;

  //: Handle key events
  virtual void keyPressEvent(QKeyEvent *e);
};

#endif
