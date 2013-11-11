#include "dbdet_region.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>


void dbdet_region::compute_V()
{

    //vcl_map<int,int>::iterator iter_option=ids_ws.begin();
    //vcl_map<int,float>::iterator iter_weights=ids_weights.begin();
    //vcl_map<int,dbdet_temporal_model*>::iterator iter_model=ids_model.begin();

    //vcl_map<int,dbdet_temporal_model*>::iterator iter_model_i;
    //vcl_map<int,dbdet_temporal_model*>::iterator iter_model_j;
    //vcl_map<int,dbdet_temporal_model*>::iterator iter_model_k;

    //vcl_map<int,float>::iterator iter_weights_i=ids_weights.begin();
    //vcl_map<int,float>::iterator iter_weights_j;
    //vcl_map<int,float>::iterator iter_weights_k;
    //vcl_map<int,int>::iterator iter_option_i=ids_ws.begin();
    //vcl_map<int,int>::iterator iter_option_j;

    //int tableofcoeffs[9][2]={{0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2}};


    //vnl_matrix<float> D(3,3,0.0);
    //vnl_matrix<float> dd(9,9,0.0);
    //vnl_vector<float> tau;
    //vnl_vector<float> Dtau(3,0);
    //
    //vnl_vector<float> tau_i(3);
    //vnl_vector<float> dtau_j(3);
    //vnl_vector<float> tau_k(3);
    //float dw_j;
    //for(iter_model_i=ids_model.begin();iter_model_i!=ids_model.end();iter_model_i++,iter_weights_i++,iter_option_i++)
    //{                            
    //    if(dbdet_temporal_normal_velocity_model * m_i=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model_i->second))
    //    {
    //        if(iter_option_i->second==1)
    //            tau_i=m_i->tau1;
    //        else if(iter_option_i->second==2)
    //            tau_i=m_i->tau2;

    //        Dtau+=iter_weights_i->second*tau_i;

    //        iter_weights_k=ids_weights.begin();
    //        vcl_map<int,int>::iterator iter_option_k=ids_ws.begin();

    //        for(iter_model_k=ids_model.begin();iter_model_k!=ids_model.end();iter_model_k++,iter_weights_k++,iter_option_k++)
    //        {
    //            if(dbdet_temporal_normal_velocity_model * m_k=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model_k->second))
    //            {
    //                if(iter_option_k->second==1)
    //                    tau_k=m_k->tau1;
    //                else if(iter_option_k->second==2)
    //                    tau_k=m_k->tau2;
    //                iter_weights_j=ids_weights.begin();
    //                vcl_map<int,int>::iterator iter_option_j=ids_ws.begin();

    //                for(iter_model_j=ids_model.begin();iter_model_j!=ids_model.end();iter_model_j++,iter_weights_j++,iter_option_j++)
    //                {
    //                    if(dbdet_temporal_normal_velocity_model * m_j=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model_j->second))
    //                    {

    //                        dtau_j=m_j->dtau;
    //                        dw_j=m_j->dw;

    //                        for(unsigned p=0;p<9;p++)
    //                            for(unsigned q=0;q<9;q++)
    //                            {
    //                                int d11=tableofcoeffs[p][0];
    //                                int d12=tableofcoeffs[p][1];
    //                                int d21=tableofcoeffs[q][0];
    //                                int d22=tableofcoeffs[q][1];

    //                                dd(p,q) += iter_weights_i->second*iter_weights_j->second*
    //                                    iter_weights_k->second/*iter_weights_j->second*/*(
    //                                    tau_i(d11)*tau_k(d21)*dw_j*dtau_j(d12)*dtau_j(d22)+
    //                                    tau_i(d11)*tau_k(d22)*dw_j*dtau_j(d12)*dtau_j(d21)+
    //                                    tau_i(d12)*tau_k(d22)*dw_j*dtau_j(d11)*dtau_j(d21)+
    //                                    tau_i(d12)*tau_k(d21)*dw_j*dtau_j(d11)*dtau_j(d22)); 
    //                            }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    //iter_weights_i=ids_weights.begin();
    //vnl_vector<float> dtau_i(3);
    //vnl_vector<float> dtau_k(3);

    //for(iter_model_i=ids_model.begin();iter_model_i!=ids_model.end();iter_model_i++,iter_weights_i++)
    //{                            
    //    if(dbdet_temporal_normal_velocity_model * m_i=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model_i->second))
    //    {
    //        dtau_i=m_i->dtau;
    //        float dw_i=m_i->dw;
    //        iter_weights_k=ids_weights.begin();

    //        for(iter_model_k=ids_model.begin();iter_model_k!=ids_model.end();iter_model_k++,iter_weights_k++)
    //        {

    //            if(dbdet_temporal_normal_velocity_model * m_k=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model_k->second))
    //            {
    //                dtau_k=m_k->dtau;   
    //                float dw_k=m_k->dw;
    //                for(unsigned p=0;p<9;p++)
    //                    for(unsigned q=0;q<9;q++)
    //                    {
    //                        int d11=tableofcoeffs[p][0];
    //                        int d12=tableofcoeffs[p][1];
    //                        int d21=tableofcoeffs[q][0];
    //                        int d22=tableofcoeffs[q][1];

    //                        dd(p,q) += 3/*iter_weights_i->second*/*iter_weights_i->second*
    //                                   /*iter_weights_k->second*/iter_weights_k->second*
    //                                   dw_i*dw_k*dtau_i(d11)*dtau_i(d12)*dtau_k(d21)*dtau_k(d22);

    //                    }
    //            }

    //        }
    //    }
    //}
    //
    ////for(;iter_model!=ids_model.end();iter_model++,iter_option++,iter_weights++)
    ////{
    ////for(;iter_model!=ids_model.end();iter_model++,iter_option++,iter_weights++)
    ////{
    ////    if(dbdet_temporal_normal_velocity_model * m
    ////        =dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model->second))
    ////    {
    ////        if(iter_option->second==1)
    ////            tau=m->tau1;
    ////        else if (iter_option->second==2)
    ////            tau=m->tau2;

    ////        //Dtau+= /*iter_weights->second**/iter_weights->second*outer_product(tau,tau);
    ////        //D+=iter_weights->second*outer_product(tau,tau);
    ////        Dtau+=iter_weights->second*tau;
    ////        double dw=m->dw;
    ////        vnl_vector<float> dtau=m->dtau;
    ////        for(unsigned i=0;i<9;i++)
    ////            for(unsigned j=0;j<9;j++)
    ////            {
    ////                int d11=tableofcoeffs[i][0];
    ////                int d12=tableofcoeffs[i][1];
    ////                int d21=tableofcoeffs[j][0];
    ////                int d22=tableofcoeffs[j][1];

    ////                dd(i,j) +=  //iter_weights->second*iter_weights->second*
    ////                            //iter_weights->second*
    ////                            iter_weights->second*
    ////                            (
    ////                            tau(d11)*tau(d21)*dw*dtau(d12)*dtau(d22)+
    ////                            tau(d11)*tau(d22)*dw*dtau(d12)*dtau(d21)+
    ////                            tau(d12)*tau(d22)*dw*dtau(d11)*dtau(d21)+
    ////                            tau(d12)*tau(d21)*dw*dtau(d11)*dtau(d22)+
    ////                            3*dw*dw*dtau(d11)*dtau(d12)*dtau(d21)*dtau(d22));



    ////                vcl_map<int,dbdet_temporal_model*>::iterator iter_model1=ids_model.begin();

    ////                for(;iter_model1!=ids_model.end();iter_model1++)
    ////                    if(iter_model1->first!=iter_model->first)
    ////                        if(dbdet_temporal_normal_velocity_model * m1=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model1->second))
    ////                            dd(i,j)+=/*iter_weights->second*iter_weights->second*ids_weights[iter_model1->first]**/ids_weights[iter_model1->first]*
    ////                                     dw*dtau(d11)*dtau(d12)*m1->dw*m1->dtau(d21)*m1->dtau(d22);
    ////            }
    ////    }
    ////}

    //vnl_matrix<float> D1=outer_product<float>(Dtau,Dtau);
    //vnl_vector<float> lambdas(3);
    //vnl_matrix<float> Vs(3,3);
    //vcl_cout<<"\n Dd is "<<"\n";
    //vcl_cout<<dd<<"\n";
    //if(!vnl_symmetric_eigensystem_compute(D,Vs,lambdas))
    //    return ;
    //vcl_cout<<   lambdas;
    //vcl_cout<< Vs;

    //if(!vnl_symmetric_eigensystem_compute(D1,Vs,lambdas))
    //    return ;
    //vcl_cout<<"Actual one";
    //vcl_cout<<lambdas;
    //vcl_cout<< Vs;

    //v1=Vs.get_column(0);
    //v2=Vs.get_column(1);

    //l1=lambdas(0);
    //l2=lambdas(1);
    //l3=lambdas(2);

    //vnl_matrix<float> Delta1(3,3,0);
    //Delta1(2,2)=-1/lambdas(2);

    //vnl_matrix<float> Delta2(3,3,0);
    //Delta2(2,2)=-1/lambdas(2);

    //vnl_matrix<float> U1(3,9,0.0);
    //U1(0,0)=Vs(0,0);    U1(0,3)=Vs(1,0);     U1(0,6)=Vs(2,0);   
    //U1(1,1)=Vs(0,0);    U1(1,4)=Vs(1,0);     U1(1,7)=Vs(2,0);
    //U1(2,2)=Vs(0,0);    U1(2,5)=Vs(1,0);     U1(2,8)=Vs(2,0);

    //vnl_matrix<float> U2(3,9,0.0);
    //U2(0,0)=Vs(0,1);    U2(0,3)=Vs(1,1);     U2(0,6)=Vs(2,1);   
    //U2(1,1)=Vs(0,1);    U2(1,4)=Vs(1,1);     U2(1,7)=Vs(2,1);
    //U2(2,2)=Vs(0,1);    U2(2,5)=Vs(1,1);     U2(2,8)=Vs(2,1);

    //v1v1=Vs*Delta1*Vs.transpose()*U1*dd*U1.transpose()*Vs*Delta1*Vs.transpose();
    //v1v2=Vs*Delta1*Vs.transpose()*U1*dd*U2.transpose()*Vs*Delta2*Vs.transpose();
    //v2v2=Vs*Delta2*Vs.transpose()*U2*dd*U2.transpose()*Vs*Delta2*Vs.transpose();

    //vcl_cout<<"v1v1\n"<<v1v1;
    //vcl_cout<<"v2v2\n"<<v2v2;
    //vcl_cout<<"v1v2\n"<<v1v2;

}

void dbdet_region::range_for_phi()
{
    //: to compute range for free paramter using +ve depth constraint
    //double va=0,vb=0;
    //vcl_map<int,int>::iterator iter_option=ids_ws.begin();
    //vcl_map<int,float>::iterator iter_weights=ids_weights.begin();
    //vcl_map<int,dbdet_temporal_model*>::iterator iter_model=ids_model.begin();


    //for(;iter_model!=ids_model.end();iter_model++,iter_option++,iter_weights++)
    //{
    //    if(dbdet_temporal_normal_velocity_model * m
    //        =dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model->second))
    //    {
    //        vnl_vector<float> dtau=m->dtau;

    //        if(vcl_fabs(m->b0)>0.2)
    //        {
    //            va+=iter_weights->second*dot_product<float>(dtau,v1)/m->b0;
    //            vb+=iter_weights->second*dot_product<float>(dtau,v2)/m->b0;
    //        }
    //    }
    //}

    //double psi=vcl_atan2(vb,va);

    //phi_min=vnl_math::pi_over_2+psi;
    //phi_max=3*vnl_math::pi_over_2+psi;



}

bool dbdet_region::compute_dist()
{
    vcl_map<int,dbdet_spherical_histogram_sptr >::iterator iter;
    Vdist=new dbdet_spherical_histogram();
    //Vdist.fill(0.0);
    float sum=0;
    for(iter=Vs.begin();iter!=Vs.end();iter++)
    {
        Vdist->operator +=(*(iter->second.ptr()));
        //for(unsigned i=0;i<iter->second.rows();i++)
        //    for(unsigned j=0;j<iter->second.cols();j++)
        //    {
        //            Vdist(i,j)+=iter->second(i,j);
        //            sum+=iter->second(i,j);
        //        

        //    }
    }
    //for(unsigned i=0;i<Vdist.rows();i++)
    //    for(unsigned j=0;j<Vdist.cols();j++)
    //    {
    //        if(Vdist(i,j))
    //            Vdist(i,j)/=sum;

    //    }

    return true;
}


