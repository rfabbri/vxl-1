#pragma once
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_mixture_traits.h>
#define combinatorial 0 
//#define DEBUG
template <class dist_>
bsta_mixture<dist_> bsta_mixture<dist_>::operator + (const bsta_mixture<dist_>& rhs) const
  {
    
	bsta_mixture<dist_> mix;
	mix.compactify=this->compactify;
	int n= this->num_components();
	int m=rhs.num_components();
	
	if (m==0)
		return *this;
	if (n==0){
		
		return rhs;
	}
	for (int i=0;i<m;i++)
		for (int j=0;j<n;j++)
		{
			T wi=rhs.weight(i);
			T wj=this->weight(j);
		   	mix.insert(this->distribution(j)+rhs.distribution(i),wi*wj);
		}
#ifdef _DEBUG	
	T sum=0;
	for (unsigned int i=0;i<mix.num_components();sum+=mix.weight(i),i++);
	
		//vcl_cout<<"sum (duplicate removal )of the means is not 1 but "<<sum<<"\n"<<mix<<vcl_endl;
#endif
	    mix.removeDuplicates();
		switch(this->compactify)
		{
		case Heap:
			mix.compactifyHeapWeight();
			break;
		case Conservative:
			mix.compactifyConservativeHeapWeight();
			break;
		case Prune:
			mix.compactifyCrude();
			break;
		case Dominant:
			mix.compactifyHeapWeight(true);
			break;
		default:
			mix.compactifyConservativeHeapWeight();
				
		}
		mix.normalize_weights();  //need to perform this to prevent numerical instabilities.
		mix.sort();
	 

	return mix;
  }

template <class dist_>
void bsta_mixture<dist_>::operator += (const bsta_mixture<dist_>& rhs)
  {
 	 *this=*this+rhs;
  }

template <class dist_>
void bsta_mixture<dist_>::operator -= (const bsta_mixture<dist_>& rhs)
  {
	 *this=*this-rhs;
  }


template <class dist_>
bsta_mixture<dist_> bsta_mixture<dist_>::operator - (const bsta_mixture<dist_>& rhs) const
  {
    return *this+rhs*(T(-1));
  }

template <class dist_>
bsta_mixture<dist_> bsta_mixture<dist_>::operator * (T scale) const
  {
	  bsta_mixture<dist_> mix;
	  mix.compactify=this->compactify;
	  int n= this->num_components();
		for (int j=0;j<n;j++)
		{
			mix.insert(this->distribution(j)*scale,this->weight(j));
		}
	    return mix;
  }

template <class dist_>
void bsta_mixture<dist_>::operator *= (T scale)
  {
	 (*this=*this*scale);
  }
  
 template <class dist_>
 bsta_mixture<dist_> bsta_mixture<dist_>::operator / (T scale) const
  {
	
	T mult=T(1)/scale;
	return *this*mult;
   
  }

template <class dist_>
 bool bsta_mixture<dist_>::removeDuplicates()
 {
	
	unsigned int delete_count=0;
	for (unsigned int i=0;i<components_.size();i++)
		 for(unsigned int j=i+1;j<components_.size()-1;j++)
			 if ((this->weight(i)!=0)&&(this->weight(j)!=0))	//avoid cehcking already deleted components
				if (bsta_mixture_traits<dist_,T>::areTheSame(this->distribution(i),this->distribution(j)))
			{
				this->set_weight(i,this->weight(i)+this->weight(j));
				//remove the smallest weighted component
				this->set_weight(j,0);//mark the distribution for deletion by giving it a zero weight
				delete_count++;
		   	}
	if (delete_count)
	{
		//delete the marked mixtures
		this->sort();
		for (unsigned int i=0;i<delete_count;i++,this->remove_last()); 
		 T sum=0;
#ifdef DEBUG		
		 for (unsigned int i=0;i<this->num_components();sum+=this->weight(i),i++);
		if (abs(sum-1)>0.001)
		vcl_cout<<"sum of the means is not 1 but "<<sum<<"\n"<<*this<<vcl_endl;	
#endif
		return true;
	}
	return false;
	
 }
 template <class dist_>
  bool bsta_mixture<dist_>::compactifyLowestWeight()
 {
	 if (components_.size()>this->max_components)
	 {
		 this->sort();
		 unsigned int n=components_.size();
		 unsigned int merge_no=n-this->max_components; //how many components we need to merge
		 unsigned int mIndex=this->max_components-1; //the index of the component that will merge with the rest
		 dist_ tempDist=this->distribution(mIndex);
		 T new_weight=this->weight(mIndex);
		 for (unsigned int i=mIndex+1;i<n;i++)
		 {
			 //keep merging the extra components into the component at @mIndex 
			 //until we have at most @max_components components
			 tempDist=bsta_mixture_traits<dist_,T>::merge(tempDist,new_weight,
				                                        this->distribution(i),this->weight(i)) ;
			 new_weight+=this->weight(i); //update the weight during the merger
		 }
		 //now prune the extra components and actually insert their merger into the mixture
		 for (unsigned int i=0;i<merge_no+1;i++,this->remove_last());  
		 this->insert(tempDist,new_weight);
		 this->sort();
		 return true;			
	 }
		return false;
 }

  template <class dist_>
  bool bsta_mixture<dist_>::compactifyHeapWeight(bool reversal)
 {
	 if (components_.size()>this->max_components)
	 {
		 
		 //vcl_sort(components_.begin(), components_.end(), sort_adaptor<sort_entropy>(sort_entropy()));
		 //float sum=1-weight(components_.size()-1);
		 while (components_.size()>max_components)
		 {
		 
		 bool tiny_weight=false;
		 float eps=1e-5f;
		 vcl_vector<component*>::iterator it1;
		 
		 if (!reversal)
			it1=min_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		 else
			 it1=max_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		
		 component *c1=*it1;
		 T w1=c1->weight;
		 dist_ tmp_dist1=c1->distribution;
		 delete c1; components_.erase(it1);

		 if(w1<eps)
		 {
			tiny_weight=true;
			continue;
		 } 
		 vcl_vector<component*>::iterator it2;

		 if (!reversal)
			it2=min_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		 else
			 it2=max_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		 component *c2=*it2;
		 T w2=c2->weight;
		 dist_ tmp_dist2=c2->distribution;
			
		 c2->distribution=bsta_mixture_traits<dist_,T>::merge(tmp_dist1,w1,tmp_dist2,w2);
		 c2->weight=w1+w2;   //update the weight during the merger
			 //keep merging the extra components into the component at @mIndex 
			 //until we have at most @max_components components
		}
		 /*float sum=0;
		 typedef typename vcl_vector<component*>::const_iterator comp_itr;
		 for (comp_itr i=components_.begin();i!=components_.end();i++)
		 sum+= (*i)->weight;  */
		
		return true;			
	 }
	return false;
 }

  template <class dist_>
  bool bsta_mixture<dist_>::compactifyConservativeHeapWeight()
 {
	 if (components_.size()>this->max_components)
	 {
		 
		 vcl_sort(components_.begin(), components_.end(), sort_adaptor<sort_entropy>(sort_entropy()));
		 //float sum=1-weight(components_.size()-1);
		 while (components_.size()>max_components)
		 {
		 
		 bool tiny_weight=false;
		 float eps=1e-5f;
		 vcl_vector<component*>::iterator it1=min_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		 component *c1=*it1;
		 T w1=c1->weight;
		 dist_ tmp_dist1=c1->distribution;
		 delete c1; components_.erase(it1);

		 if(w1<eps)
		 {
			tiny_weight=true;
			continue;
		 } 
		 

		 vcl_vector<component*>::iterator it2=min_element(components_.begin(),components_.end(),sort_adaptor<sort_entropy>(sort_entropy()));
		 component *c2=*it2;
		 T w2=c2->weight;
		 dist_ tmp_dist2=c2->distribution;
			
		 c2->distribution=bsta_mixture_traits<dist_,T>::merge(tmp_dist1,w1,tmp_dist2,w2);
		 c2->weight=w2;   //update the weight during the merger
			 //keep merging the extra components into the component at @mIndex 
			 //until we have at most @max_components components
		}
		 /*float sum=0;
		 typedef typename vcl_vector<component*>::const_iterator comp_itr;
		 for (comp_itr i=components_.begin();i!=components_.end();i++)
		 sum+= (*i)->weight;  */
		
		return true;			
	 }
	return false;
 }

template <class dist_>
 bool bsta_mixture<dist_>::compactify2Paired()
 {
	if (components_.size()>this->max_components)
	{
		components_=this->recursiveMerge(components_);
 		return true;
	}
	return false;
 }

 template <class dist_>
 bool bsta_mixture<dist_>::compactifyCrude(){
	   if (this->num_components()>this->max_components)
		{   
			this->sort();
			int delete_no=this->num_components()-this->max_components;
			for (int i=0;i<delete_no;i++,this->remove_last());
			this->normalize_weights();
			return true;
		}
	   return false;
   }
 template <class dist_>
 bool bsta_mixture<dist_>::compactifyNPaired()
 {
	 
	 //typedef component bsta_mixture::component;
	 unsigned int n=components_.size();
	 if (n<max_components)
		 return false;
	 unsigned int num_partitions=n/this->max_components;
	 if (num_partitions<2)
		 return this->compactifyHeapWeight();
	 unsigned int extra_comps=n%max_components;
	 unsigned int n2=max_components*num_partitions;
	 
	 vcl_vector<component*> tmpVec;
	 //vcl_sort(components_.begin(), components_.end(), sort_adaptor<sort_entropy>(sort_entropy()));
	 //vcl_sort(components_.begin(), components_.end(), sort_weight() );
	 for (unsigned int partInd=0;partInd<n2;partInd+=max_components)
	 {
		dist_ tempDist=components_[partInd]->distribution;
		T tempWeight=components_[partInd]->weight;
		for (unsigned int i=1;i<max_components;i++)
		{
			T w=components_[partInd+i]->weight;
			tempDist=bsta_mixture_traits<dist_,T>::merge(tempDist,tempWeight,
										  components_[partInd+i]->distribution,w) ;
			tempWeight+=w;
		}
		tmpVec.push_back(new component(tempDist,tempWeight));
	 }

	 if(extra_comps)
	 {
		 dist_ tempDist=tmpVec[n2-1]->distribution;
		 T tempWeight=tmpVec[n2-1]->weight;
		 for(unsigned int i=0;i<extra_comps;i++)
			{
	 		 T w=components_[n2+i]->weight;
			 tempDist=bsta_mixture_traits<dist_,T>::merge(tempDist,tempWeight,
										  components_[n2+i]->distribution,w);
			 tempWeight+=w;
	 
			}
	 tmpVec.push_back(new component(tempDist,tempWeight));
	 }
	 //erase the old vector;
	 for (unsigned i=0;i<n;delete components_[i],i++);
	 components_.clear();
	 //assign the new vector
	 components_=tmpVec;
	 vcl_sort(components_.begin(), components_.end(), sort_weight());
	 return true;
 }

 template <class dist_>
 vcl_vector<typename bsta_mixture<dist_>::component*> bsta_mixture<dist_>::recursiveMerge(vcl_vector<typename bsta_mixture<dist_>::component* > vec)
 {
	if (vec.size()<=this->max_components)
	{
		return vec;
	}
	vcl_vector<component*> tmpVec;
	unsigned int n=vec.size();
	vcl_sort(vec.begin(), vec.end(), sort_weight() );
	unsigned int start=0;
	if (vec.size()%2!=0)		 //transfer the largest weighted mixture if the number of elements is odd
	{
		T w1=vec.at(n-1)->weight;
		T w2=vec.at(n-2)->weight;
		
		dist_ myDist =bsta_mixture_traits<dist_,T>::merge(vec[n-1]->distribution,w1,
										  vec[n-2]->distribution,w2) ;
		delete vec[n-1];delete vec[n-2];vec.pop_back();vec.pop_back();
		vec.push_back(new component(myDist,w1+w2));
		n=vec.size();
	}
#if combinatorial 
	unsigned int finish=n/2;
	unsigned int incr=1;
#else
	unsigned int finish=n-1;
	unsigned int incr=2;
#endif
	for (unsigned int i=start;i<finish;i+=incr)
	{
#if combinatorial		
		T w1=vec.at(i)->weight;
		T w2=vec.at(n-i-1)->weight;
		dist_ myDist =bsta_mixture_traits<dist_,T>::merge(vec.at(i)->distribution,w1,
										  vec.at(n-i-1)->distribution,w2) ;
#else
		T w1=vec.at(i)->weight;
		T w2=vec.at(i+1)->weight;
		dist_ myDist =bsta_mixture_traits<dist_,T>::merge(vec.at(i)->distribution,w1,
										  vec.at(i+1)->distribution,w2) ;
#endif
		tmpVec.push_back(new component(myDist,w1+w2));
	}
	for (unsigned i=0;i<vec.size();delete vec[i],i++); //cleanup so we'll avoid memory leaks. 
	vec.clear();
	return recursiveMerge(tmpVec);
 
 }

 

#define BSTA_MIXTURE_INSTANTIATE(dist_) \
template class bsta_mixture<dist_>
 