#include "vehicle_model.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgl/vgl_homg_point_2d.h>

//helper function to transform the model
//only translation and scaling
void transform_component(vnl_matrix<double>const & m,vsol_rectangle_2d & box)
    {
    vsol_point_2d_sptr v1 = box.p0();
    vsol_point_2d_sptr v2 = box.p1();
    vsol_point_2d_sptr v3 = box.p2();
    vsol_point_2d_sptr v4 = box.p3();

    double x,y;

    x = v1->x();
    y = v1->y();

    v1->set_x(m.get(0,0)*x+m.get(0,2));
    v1->set_y(m.get(1,1)*y+m.get(1,2));

    x = v2->x();
    y = v2->y();

    v2->set_x(m.get(0,0)*x+m.get(0,2));
    v2->set_y(m.get(1,1)*y+m.get(1,2));

    x = v3->x();
    y = v3->y();

    v3->set_x(m.get(0,0)*x+m.get(0,2));
    v3->set_y(m.get(1,1)*y+m.get(1,2));

    x = v4->x();
    y = v4->y();

    v4->set_x(m.get(0,0)*x+m.get(0,2));
    v4->set_y(m.get(1,1)*y+m.get(1,2));
    }

vehicle_model::vehicle_model(vsol_rectangle_2d  engine,vsol_rectangle_2d  body,vsol_rectangle_2d  rear):
    engine_(engine),body_(body),rear_(rear)
        {

        }

    void vehicle_model::transform_model(vnl_matrix<double>const& m1,vnl_matrix<double>const& m2,vnl_matrix<double>const& m3)
        {
        transform_component(m1,engine_);
        transform_component(m2,body_);
        transform_component(m3,rear_);


        }
    void vehicle_model::set_model(vsol_rectangle_2d  engine,vsol_rectangle_2d  body,vsol_rectangle_2d  rear)
        {
        engine_.p0()->set_x(engine.p0()->x());
        engine_.p0()->set_y(engine.p0()->y());
        engine_.p1()->set_x(engine.p1()->x());
        engine_.p1()->set_y(engine.p1()->y());
        engine_.p2()->set_x(engine.p2()->x());
        engine_.p2()->set_y(engine.p2()->y());
        engine_.p3()->set_x(engine.p3()->x());
        engine_.p3()->set_y(engine.p3()->y());

        body_.p0()->set_x(body.p0()->x());
        body_.p0()->set_y(body.p0()->y());
        body_.p1()->set_x(body.p1()->x());
        body_.p1()->set_y(body.p1()->y());
        body_.p2()->set_x(body.p2()->x());
        body_.p2()->set_y(body.p2()->y());
        body_.p3()->set_x(body.p3()->x());
        body_.p3()->set_y(body.p3()->y());

        rear_.p0()->set_x(rear.p0()->x());
        rear_.p0()->set_y(rear.p0()->y());
        rear_.p1()->set_x(rear.p1()->x());
        rear_.p1()->set_y(rear.p1()->y());
        rear_.p2()->set_x(rear.p2()->x());
        rear_.p2()->set_y(rear.p2()->y());
        rear_.p3()->set_x(rear.p3()->x());
        rear_.p3()->set_y(rear.p3()->y());
        }


    vcl_ostream& vehicle_model::print(vcl_ostream& out) const
        {
        out << engine_.p0()->x() <<" " <<engine_.p0()->y() << " " << vcl_endl;
        out << engine_.p1()->x() <<" " <<engine_.p1()->y() << " " << vcl_endl;
        out << engine_.p2()->x() <<" " <<engine_.p2()->y() << " " << vcl_endl;
        out << engine_.p3()->x() <<" " <<engine_.p3()->y() << " " << vcl_endl;

        out << body_.p0()->x() <<" " <<body_.p0()->y() << " " << vcl_endl;
        out << body_.p1()->x() <<" " <<body_.p1()->y() << " " << vcl_endl;
        out << body_.p2()->x() <<" " <<body_.p2()->y() << " " << vcl_endl;
        out << body_.p3()->x() <<" " <<body_.p3()->y() << " " << vcl_endl;

        out << rear_.p0()->x() <<" " <<rear_.p0()->y() << " " << vcl_endl;
        out << rear_.p1()->x() <<" " <<rear_.p1()->y() << " " << vcl_endl;
        out << rear_.p2()->x() <<" " <<rear_.p2()->y() << " " << vcl_endl;
        out << rear_.p3()->x() <<" " <<rear_.p3()->y() << " " << vcl_endl;

        return out;
        }

      void vehicle_model::vrml_print(vcl_ostream& out) 
        {
        out << engine_.p0()->x() <<" " <<engine_.p0()->y() << " " << "1" << vcl_endl;
        out << engine_.p1()->x() <<" " <<engine_.p1()->y() << " " <<  "1" << vcl_endl;
        out << engine_.p2()->x() <<" " <<engine_.p2()->y() << " " << "1" <<vcl_endl;
        out << engine_.p3()->x() <<" " <<engine_.p3()->y() << " " << "1" <<vcl_endl;

        out << body_.p0()->x() <<" " <<body_.p0()->y() << " " << "1" <<vcl_endl;
        out << body_.p1()->x() <<" " <<body_.p1()->y() << " " << "1" <<vcl_endl;
        out << body_.p2()->x() <<" " <<body_.p2()->y() << " " << "1" <<vcl_endl;
        out << body_.p3()->x() <<" " <<body_.p3()->y() << " " << "1" <<vcl_endl;

        out << rear_.p0()->x() <<" " <<rear_.p0()->y() << " " << "1" <<vcl_endl;
        out << rear_.p1()->x() <<" " <<rear_.p1()->y() << " " << "1" <<vcl_endl;
        out << rear_.p2()->x() <<" " <<rear_.p2()->y() << " " << "1" <<vcl_endl;
        out << rear_.p3()->x() <<" " <<rear_.p3()->y() << " " << "1" <<vcl_endl;

        }

    vcl_istream& vehicle_model::read(vcl_istream& in) const
        {
        int j,k;
        double x,y;
        vcl_vector<vsol_rectangle_2d> rec_vec;
        //reading the three rectangles..the engine,box and the rear
        for (j =0;j<3;j++)
            {  
            vsol_point_2d p0,p1,p2,p3;
            //reading the four co-ordinates of the rectangle
            for (k = 0;k<4;k++)
                {
                in >> x >> y;
                if (k==0)
                    {
                    p3.set_x(x);
                    p3.set_y(y);
                    }
                if (k==1)
                    {
                    p2.set_x(x);
                    p2.set_y(y);
                    }
                if (k==2)
                    {
                    p1.set_x(x);
                    p1.set_y(y);
                    }
                if (k==3)
                    {
                    p0.set_x(x);
                    p0.set_y(y);
                    }

                }
            if (j == 0)
                {
                engine_.p0()->set_x(p0.x());
                engine_.p0()->set_y(p0.y());
                engine_.p1()->set_x(p1.x());
                engine_.p1()->set_y(p1.y());
                engine_.p2()->set_x(p2.x());
                engine_.p2()->set_y(p2.y());
                engine_.p3()->set_x(p3.x());
                engine_.p3()->set_y(p3.y());
                }
            if (j == 1)
                {
                body_.p0()->set_x(p0.x());
                body_.p0()->set_y(p0.y());
                body_.p1()->set_x(p1.x());
                body_.p1()->set_y(p1.y());
                body_.p2()->set_x(p2.x());
                body_.p2()->set_y(p2.y());
                body_.p3()->set_x(p3.x());
                body_.p3()->set_y(p3.y());
                }
            if (j == 2)
                {
                rear_.p0()->set_x(p0.x());
                rear_.p0()->set_y(p0.y());
                rear_.p1()->set_x(p1.x());
                rear_.p1()->set_y(p1.y());
                rear_.p2()->set_x(p2.x());
                rear_.p2()->set_y(p2.y());
                rear_.p3()->set_x(p3.x());
                rear_.p3()->set_y(p3.y());
                }
            }

        return in;
        }


    vcl_ostream& operator<<(vcl_ostream& out,vehicle_model const& M)
        {
        return M.print(out);
        }

    vcl_istream& operator>>(vcl_istream& in,vehicle_model &M)
        {
        return M.read(in);
        }





