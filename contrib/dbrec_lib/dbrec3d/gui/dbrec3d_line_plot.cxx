//:
// \file
// \author Isabel Restrepo
// \date 30-Nov-2010

#include "dbrec3d_line_plot.h"
#include <qcolor.h>
#include <qpainter.h>
#include <qwt_plot_picker.h>
/*********************************dbrec3d_line_plot******************************/
dbrec3d_line_plot::dbrec3d_line_plot(const double* datablock, unsigned size)
{
  /**** Arrange data *****/
  size_= size;

  //x values are assumed to be a simple increasing index
  xvals_ = new double[size];
  yvals_ = new double[size];
  
  for(unsigned i=0; i<size_;i++)
    xvals_[i] = i; 
     
  yvals_ = datablock;
  
  
  /**** Arrange plot *****/
  setCanvasBackground(QColor(Qt::white));
  setAxisTitle(xBottom, "X");
  setAxisTitle(yLeft, "Y");
  
  
  /**** Attach curves *****/
  
  //define the style
  main_curve_ = new QwtPlotCurve;
  main_curve_->setPen(QColor(Qt::blue));
  main_curve_->setStyle(QwtPlotCurve::Lines);
  main_curve_->setRenderHint(QwtPlotItem::RenderAntialiased);
  
  //attach the data
  main_curve_->setRawData(xvals_, yvals_, size_);
  main_curve_->attach(this);
  
  // Insert marker at x=0
  curr_x_ = 0;
  curr_marker_ = new QwtPlotMarker();
  curr_marker_->setLabel(QString::number(yvals_[curr_x_]));
  curr_marker_->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
  curr_marker_->setLineStyle(QwtPlotMarker::Cross);
  curr_marker_->setXValue((double)curr_x_);
  curr_marker_->setYValue(yvals_[curr_x_]);
  curr_marker_->attach(this);
  
  
  replot();
  
}

void dbrec3d_line_plot::set_active_datapoint(unsigned index)
{
  curr_x_ = index;
  curr_marker_->setLabel(QString::number(yvals_[curr_x_]));
  curr_marker_->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
  curr_marker_->setLineStyle(QwtPlotMarker::Cross);
  curr_marker_->setXValue((double)curr_x_);
  curr_marker_->setYValue(yvals_[curr_x_]);
  //curr_marker_->attach(this);
  replot();
}

/*********************************dbrec3d_choose_plot******************************/

dbrec3d_choose_plot::dbrec3d_choose_plot()
{
  /**** Arrange plot *****/
  setCanvasBackground(QColor(Qt::white));
  setAxisTitle(xBottom, "X");
  setAxisTitle(yLeft, "Y");
  
    
  //define the style
  main_curve_ = new QwtPlotCurve;
  main_curve_->setPen(QColor(Qt::blue));
  main_curve_->setStyle(QwtPlotCurve::Lines);
  main_curve_->setRenderHint(QwtPlotItem::RenderAntialiased);
  
  QwtPlotPicker* d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::PointSelection | QwtPicker::DragSelection, 
                               QwtPlotPicker::CrossRubberBand, QwtPicker::ActiveOnly, 
                               this->canvas());
  d_picker->setRubberBandPen(QColor(Qt::green));
  d_picker->setRubberBand(QwtPicker::CrossRubberBand);
  d_picker->setTrackerPen(QColor(Qt::white));
  
  
  //connect signals/slots
  connect(this, SIGNAL(request_next_plot()), this, SLOT(draw_next_plot()));
  
}

void dbrec3d_choose_plot::register_plot(QString const &plot_name, QwtArray<double> const &x_data, QwtArray<double> const &y_data)
{
  QPair<QwtArray<double>, QwtArray<double> >	data_pair =  qMakePair(x_data, y_data);
  plots_.insert(plot_name, data_pair);  
}

void dbrec3d_choose_plot::init()
{
  plots_it_ = plots_.constBegin();
  
  //attach the first plot
  if(plots_it_ != plots_.constEnd())
  {
    this->setTitle(plots_it_.key());
    main_curve_->setRawData(&plots_it_.value().first[0], &plots_it_.value().second[0],plots_it_.value().first.size() );
    main_curve_->attach(this);
  }
}

#if 0 
void dbrec3d_choose_plot::set_active_datapoint(unsigned index)
{
  curr_x_ = index;
  curr_marker_->setLabel(QString::number(yvals_[curr_x_]));
  curr_marker_->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
  curr_marker_->setLineStyle(QwtPlotMarker::Cross);
  curr_marker_->setXValue((double)curr_x_);
  curr_marker_->setYValue(yvals_[curr_x_]);
  //curr_marker_->attach(this);
  replot();
}
#endif

void dbrec3d_choose_plot::draw_next_plot()
{
  ++plots_it_;
  
  if(plots_it_==plots_.constEnd())
    plots_it_ = plots_.constBegin();
  
  this->setTitle(plots_it_.key());
  main_curve_->setRawData(&plots_it_.value().first[0], &plots_it_.value().second[0],plots_it_.value().first.size() );

  replot();
}

void dbrec3d_choose_plot::keyPressEvent(QKeyEvent *event)
{
  //if undefined, ignore
  if (event->key() == 0)
	{
		event->ignore();
		return;
	}
  
 
  if(event->key() == Qt::Key_Right)
  {
    emit request_next_plot();//update_volume_data();
  }
//  if(event->key() == Qt::Key_Left)
//  {
//    emit request_prev_volume();//update_volume_data();
//  }
}