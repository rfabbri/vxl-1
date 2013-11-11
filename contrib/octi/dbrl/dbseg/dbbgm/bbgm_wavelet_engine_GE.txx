#include <dbbgm/bbgm_wavelet_engine_GE.h>
#include <dbbgm/bbgm_wavelet_list.h>
#include <dbbgm/bbgm_wavelet_image_traits.h>
#include <vul/vul_timer.h>

template <class image_>
bool
dbbgm_wavelet_engine_GE<image_>::bbgmWaveletTransform(const image_* from, image_* to,
                                              const bool forwardp,    // or inverse
                                              int nlevels,
                                              const int waveletno)
{

  int dims[3];
  dims[0] = to->nj();
  dims[1] = to->ni();
  dims[2] = dims[0] * dims[1];
  image_* workWork =bbgm_wavelet_image_traits<image_>::init(dims[2],1);
  				   
  typename image_::const_iterator itrFrom = from->begin();
  typename image_::iterator itrTo = workWork->begin();
  
  for (int i = 0; i < dims[2];++itrTo,++itrFrom, i++)
      *(itrTo)=*(itrFrom);
	
  bool success= bbgmWaveletTransform(*workWork, // transform in place
                                                       dims, 2,
                                                       forwardp, nlevels,
                                                       waveletno);
 
  typename image_::iterator itrFrom2 = workWork->begin();
  itrTo = to->begin();
  for (int i = 0; i < dims[2];++itrTo,++itrFrom2, i++)
      *(itrTo)=*(itrFrom2);
  
  delete workWork;
  return success;
   
}

template <class image_>
bool
dbbgm_wavelet_engine_GE<image_>::bbgmWaveletTransform(image_& array,const int* dims, const int ndim,
                                              const bool forwardp, int nlevels,
                                              const int waveletno)

{
  	vul_timer t;
	int ntot = 1, maxn = 0;

  {for (int d = 0; d < ndim; d++) {
    int dim = dims[d];
    ntot *= dim;
    if (dim > maxn) maxn = dim;
  }}

  float* lo_filter = NULL;
  float* hi_filter = NULL;
  int ncof = 0;
  if (!bbgmFindWavelet(waveletno, lo_filter, hi_filter, ncof)) // look up wavelets
    return false;

  int** level_dims = new int*[nlevels];
  level_dims[0] =  new int[ndim+1];
  for (int d = 0; d < ndim; d++)
    level_dims[0][d] = dims[d];
  level_dims[0][ndim] = ntot;
  for (int l = 1; l < nlevels; l++) {
    level_dims[l] = new int[ndim+1];
    int n = 1;
    for (int d = 0; d < ndim; d++) {
      int nd = level_dims[l-1][d];
      if (nd >= 4)
        nd /= 2;
      level_dims[l][d] = nd;
      n *= nd;
    }
    level_dims[l][ndim] = n;
  }
  if (!forwardp) {                              // reverse order of the
    int * swap;                                 // dimensions of sub_array
    for (int l = 0; l < nlevels/2; l++) {
      swap = level_dims[l];
      level_dims[l] = level_dims[nlevels-1-l];
      level_dims[nlevels-1-l] = swap;
    }
  }
  image_* sub_array =bbgm_wavelet_image_traits<image_>::init(ntot,1);           // sub-block of low frequency
  image_* buffer=bbgm_wavelet_image_traits<image_>::init(maxn,1);              // working buffers for
  image_* wksp=bbgm_wavelet_image_traits<image_>::init(maxn,1);                // 1d wavelet transformation
  {for (int l = 0; l < nlevels; l++) {
    vcl_cout<<"Started level "<<l+1<<vcl_endl;
	int* sub_dims = level_dims[l];
    int n = sub_dims[ndim];

	image_iterator itrTo = sub_array->begin();
    image_iterator itrFrom = array.begin();
    bbgmCopyNdRecursive(array, ntot, dims,
                    *sub_array, n, sub_dims,     // consecutive elements.
                    ndim,true,0,0);
	bbgmWaveletTransformStep(*sub_array, sub_dims, ndim,
                         forwardp,
                         lo_filter, hi_filter, ncof,
                         *buffer, *wksp);

	 itrTo = array.begin();
     itrFrom = sub_array->begin();

    bbgmCopyNdRecursive(*sub_array ,n, sub_dims,
                    array, ntot, dims,
                    ndim,true,0,0);
	vcl_cout<<"Done level "<<l+1<<vcl_endl;
  }}
  delete sub_array;                          // free workspace>  
  for (int l = 0; l < nlevels; l++)
    delete[] level_dims[l];
  delete[] level_dims;
  delete buffer;
  delete wksp;
  vcl_cout << " Transformed image in " << double(t.real())/1000 << " seconds\n"
             << vcl_flush;
  return true;
}



template <class image_>
void
dbbgm_wavelet_engine_GE<image_>::bbgmWaveletTransformStep(image_& array, const int* dims, const int ndim,
                                           const bool forwardp,
                                           const float* lo_filter,
                                           const float* hi_filter,
                                           const int ncof,
                                           image_ buffer, image_ wksp)
{
  int ntot = dims[ndim];
  int nprev = 1;
  for (int d = ndim-1; d >= 0; d--) {   // imension varies most, first
    int n = dims[d];
    int nnew = n * nprev;
    if (n >= 4) {

		for (int i2 = 0; i2 < ntot; i2 += nnew){		//this code processes two dimensions at a time starting with rows
														//i1 is the index that iterates over the 'faster' dimension
														//i2 is the index that iterates	over the 'slower' dimension
														//each dimension step size is increased by an increment 
			for (int i1 = 0; i1 < nprev; i1++) {		//nprev is the increment of step size the 'faster dimension
				 int i3 = i1 + i2;     					//nnew is the increment of the step size of'slower' dimension
			
				for (int k = 0; k<n; k++) {	  
			   		 buffer(k,0)=array(i3,0);	// copy data to convolve				
					 i3 += nprev;
				}
				bbgmWaveletTransformStep(buffer, n, forwardp,
					lo_filter, hi_filter, ncof,
					wksp);
				
				i3 = i1 + i2;                         // copy back results
				for (int k = 0; k<n;k++) {
					 array(i3,0)=buffer(k,0);
					 i3 += nprev;
				}
#ifdef VERBOSE
				if (nprev!=1)
				vcl_cout<<"wrote wavelet domain column "<<(i1)<<vcl_endl;
#endif
			
			}
#ifdef VERBOSE			
		vcl_cout<<"wrote wavelet domain line "<<(i2/nnew)<<vcl_endl;
#endif
		}
	}
    nprev = nnew;
  }
}

static bool bbgmFindWavelet(const int waveletno,float*& lo_filter, float*& hi_filter, int& ncof)
{
  ncof = waveletno;
  switch (waveletno)
  {
   case 2:
    lo_filter = haar2;
    break;
   case 4:
    lo_filter = daubechies4;
    break;
   case 6:
    lo_filter = daubechies6;
    break;
   case 8:
    lo_filter = daubechies8;
    break;
   case 10:
    lo_filter = daubechies10;
    break;
   case 12:
    lo_filter = daubechies12;
    break;
   case 20:
    lo_filter = daubechies20;
    break;
   case 9:                                       // wavelets from epic
    lo_filter = coifman9;
    break;
   case 11:
    lo_filter = coifman11;
    break;
   case 15:
    lo_filter = coifman15;
    break;
   default:
    ncof = 0;
    lo_filter = hi_filter = NULL;
    vcl_cerr << "Unknown wavelet: " << waveletno << vcl_endl;
    return false;
  }
  // find hi-filter wavelet, dual of the lo-filter wavelet
  if (lo_filter)
  {
    hi_filter = dual_wavelet;
    if ((waveletno%2) == 0) {
      int sign = -1;                            // reverse and
      for (int k = 0; k < ncof; k++) {          // flip sign on odd coefts
        hi_filter[ncof-1-k] = sign * lo_filter[k];
        sign = - sign;
      }
    } else {                                    // odd filter is symmetric
      int sign = 1;
      int ctr = ncof/2;                         // flip sign on odd coefts
      for (int k = 0; k <= ncof/2; k++) {       // starting from center
        hi_filter[ctr+k] = sign * lo_filter[ctr-k];
        hi_filter[ctr-k] = sign * lo_filter[ctr+k];
        sign = - sign;
      }
      vcl_cerr << "Scale factor need to be fixed up too!!!\n";
    }
    // find area of lo_filter and hi_filter
    float lo_area = 0;
    float hi_area = 0;
    for (int k = 0; k < ncof; k++) {
      lo_area += lo_filter[k];
      hi_area += hi_filter[k];
    }
    lo_filter[ncof] = lo_area;
    hi_filter[ncof] = hi_area;
  }
#ifdef DEBUG
  vcl_cout << "lo-filter wavelet " << waveletno << ':'; // print wavelets
  for (int i = 0; i < ncof; i++)
    vcl_cout << ' ' << lo_filter[i];
  vcl_cout << "\nhi-filter wavelet " << waveletno << ':';
  for (int i = 0; i < ncof; i++)
    vcl_cout << ' ' << hi_filter[i];
  vcl_cout << vcl_endl;
#endif
  return true;
}


template <class image_>
void dbbgm_wavelet_engine_GE<image_>::bbgmCopyNdRecursive(const image_& from,const int from_size, const int* from_dims,image_& to,const int to_size, const int* to_dims,const int ndim,const bool fullp,int offsetTo,int offsetFrom)
{
    if (ndim == 1) {                              // end of recursion
    int size = vcl_min(from_size, to_size);
    
   
	for (int i = 0; i < size;i++)              // copy 1d array for
      to(i+offsetTo,0)=from(i+offsetFrom,0);              // common indices only
		
  } else {
    int from_n = from_dims[0], to_n = to_dims[0];
    int from_nsize = from_size / from_n;
    int to_nsize = to_size / to_n;
    int n = vcl_min(from_n, to_n);
  	for (int i = 0; i < n; i++) {               // copy n common subarrays
      bbgmCopyNdRecursive(from, from_nsize, from_dims+1,
                      to,to_nsize, to_dims+1,
                      ndim-1, fullp,offsetTo,offsetFrom);
     
	 
	  if (fullp) {
         offsetTo+=to_nsize;
		 offsetFrom+=from_nsize;
		} else {
		int block_size = vcl_max(from_nsize, to_nsize);
		
		offsetTo+=block_size;
		offsetFrom+=block_size; 
													         // inc pointer of arrays
                    // to next block
      }
    }
  }
}



template <class image_>
void
	dbbgm_wavelet_engine_GE<image_>::bbgmWaveletTransformStep(image_& array, const int n,
	const bool forwardp,
	const float* lo_filter,
	const float* hi_filter ,					    
	const int ncof,
	image_ wksp)
{

	                 // clear workspace*/

	wksp=bbgm_wavelet_image_traits<image_>::zeroVal(wksp);	
	int nmid = n / 2;                               // round off towards 0
	int nmod = nmid * 2;

	// even and <= n.
	if (forwardp) {                               // forward transform
		for (int i=0, ii=0; i < nmod; i +=2, ++ii)  // every pair
			for (int k = 0; k < ncof; k++) {          // convolution with filters
				int j = (i + k) % nmod;                 // wrap around
			       // when n is power of 2

				wksp(ii,0)=wksp(ii,0)+		  // lo-filter results
					array(j,0)*lo_filter[k];

				wksp(ii+nmid,0)=wksp(ii+nmid,0)+		   // hi-filter results
					array(j,0)*hi_filter[k];

			}
			float scale = vcl_max(lo_filter[ncof], hi_filter[ncof]);

			image_iterator wkspIt=wksp.begin();
			for (int j = 0; j < nmod;++wkspIt, j++)             // normalize results.
			    	*wkspIt =*wkspIt/scale;
	} else {                                         // inverse transform
		for (int i=0, ii=0; i < nmod; i+=2, ++ii) { // every pair
			for (int k = 0; k < ncof; k++) {          // multiplication with inverse
				int j = (i + k) % nmod; 
													// matrix, or its transpose
			  wksp(j,0)=wksp(j,0)+array(ii,0)*lo_filter[k]+
					               array(ii+nmid,0)*hi_filter[k];
								// when n is power of 2
			}
		}
		float scale = vcl_max(lo_filter[ncof], hi_filter[ncof]);
		image_iterator wkspIt=wksp.begin();

		for (int j = 0; j < nmod; j++,++wkspIt)              // unnormalize results.
			*wkspIt =*wkspIt* scale;
	}

	image_iterator wkspIt=wksp.begin();
	image_iterator arrayIt=array.begin();
	for (int j = 0; j < nmod; j++,++wkspIt,++arrayIt)     // copy only nmod results back to array
		*arrayIt = *wkspIt;                         
}				 

template <class image_>
bool dbbgm_wavelet_engine_GE<image_>::decompressLocation(unsigned int x,unsigned int y,contained_type& result,
	const int waveletNo,int level,const image_& waveletDomain)
{
  float* lo_filter = NULL;
  float* hi_filter = NULL;
  int ncof = 0;
  if (!bbgmFindWavelet(waveletNo, lo_filter, hi_filter, ncof)) // look up wavelets
  {
	  result=waveletDomain(x,y);
	  return false;
  }
  unsigned int LL_h=waveletDomain.nj()/int(pow(2.0,level));
  unsigned int LL_w=waveletDomain.ni()/int(pow(2.0,level));
										   
  int extra_contributors=(ncof-1)/2;
  int size=2*(2*extra_contributors+1);
  int h_size=size/2;
  
  image_* copy_buffer =bbgm_wavelet_image_traits<image_>::init(size,size);
  image_* out_buffer =bbgm_wavelet_image_traits<image_>::init(size,size);
  image_* copy_buffer_1D =bbgm_wavelet_image_traits<image_>::init(size,1);
  image_* out_buffer_1D =bbgm_wavelet_image_traits<image_>::init(size,1);
  
  unsigned int ** coords= new unsigned int*[level+1];
  
  for ( int i=0;i<=level;i++)
	coords[i]= new unsigned int[2];
  this->determine_coordinates(x,y,coords,level);   //find the coordinates at each level

  unsigned int x_top=coords[level][0];
  unsigned int y_top=coords[level][1]; 
  
 
  for (int i=0;i<h_size;i++)					 
  {	  
	  int x_lo=int(x_top)-i>=0 ? x_top-i : x_top-i+LL_w; //wrap around if a contributor is outside the canvass
	  int x_hi=x_lo+LL_w;
	  for (int j=0;j<h_size;j++) 	  //create the small 2d buffer that we will transform recursively.
	{ 
		int y_lo= (int(y_top)-j>=0) ? y_top-j : y_top-j+LL_h ;
	    int y_hi=y_lo+LL_h;
			
	    (*copy_buffer)(h_size-i-1,h_size-j-1)=waveletDomain(x_lo,y_lo);	    //actual grayscale value or LL component
		(*copy_buffer)(h_size-i-1,size-1-j)=waveletDomain(x_lo,y_hi);   //LH component
		(*copy_buffer)(size-i-1,h_size-j-1)=waveletDomain(x_hi,y_lo);   // HL component
		(*copy_buffer)(size-i-1 ,size-1-j)=waveletDomain(x_hi,y_hi); //HH component
		// vcl_cout<<i<<" , "<<j<<" low part is "<<(*copy_buffer)(i,j)<<" and high part is "<<(*copy_buffer)(i+sz_x,j+sz_y)<<vcl_endl;
	}

  }
		
    
  recursiveWaveletLookup(*copy_buffer,*out_buffer,*copy_buffer_1D,*out_buffer_1D,lo_filter,hi_filter,waveletDomain,level-1,coords,ncof);
  
  result=(*copy_buffer)(size-1,size-1);
  
  delete copy_buffer;
  delete out_buffer;
  delete copy_buffer_1D;
  delete out_buffer_1D;
  for (int i=0;i<=level;i++)
	  delete coords[i];
  delete [] coords;
  return true;
}
 
template <class image_>
bool dbbgm_wavelet_engine_GE<image_>::recursiveWaveletLookup(image_& copy_buffer,image_& out_buffer,image_& copy_buffer_1D,image_& out_buffer_1D,
		float* lo_filter,float* hi_filter,const image_& waveletDomain,const int level,unsigned int** coords,int ncof)
{
    //image_* out_buffer =bbgm_wavelet_image_traits<image_>::init(w,h);

   	transformMinimal2DBuffer(copy_buffer,out_buffer,copy_buffer_1D,out_buffer_1D,0,lo_filter,hi_filter,ncof);	  // filter rows
    
	int ncontrib=(ncof-1)/2;
	int size=2*(2*ncontrib+1);
	int h_size=size/2;

	
	for ( int i=0;i<size;i++)	  //copy back
		for ( int j=0;j<size;j++) 
		{
	 		copy_buffer(i,j)=out_buffer(i,j);
	    }
		

   transformMinimal2DBuffer(copy_buffer,out_buffer,copy_buffer_1D,out_buffer_1D,1,lo_filter,hi_filter,ncof);	 //filter columns
   
   unsigned int x=coords[level][0];
   unsigned int y=coords[level][1];
   
   unsigned int startx= x % 2 ? size-1 :size-2; //even coordinates are second to last in the buffer while odd coordinates are on the
   unsigned int starty= y % 2 ? size-1 :size-2; //very last position in the buffer

   
     
   if (level==0)			//end of recursion
   {
	  copy_buffer(size-1,size-1)=out_buffer(startx,starty);
	  return true;		 	   
   }
  unsigned  int LL_h=waveletDomain.nj()/int(pow(2.0,level));
  unsigned  int LL_w=waveletDomain.ni()/int(pow(2.0,level));
 
  for (int i=startx,ii=0;ii<=2*ncontrib;i--,ii++)	 //TO-DO
  {
	  int x_lo=(int(x)-int(ii))>=0 ? x-ii : x-ii+LL_w;
	  int x_hi=x_lo+LL_w;
	  for(int j=starty,jj=0;jj<=2*ncontrib;j--,jj++)
	  {
		  int y_lo=(int(y)-int(jj))>=0 ? y-jj : y-jj+LL_h;
		  int y_hi=y_lo+LL_h;
		  copy_buffer(h_size-ii-1,h_size-jj-1)=out_buffer(i,j);
		  copy_buffer(h_size-ii-1,size-jj-1)=waveletDomain(x_lo,y_hi);
		  copy_buffer(size-ii-1,h_size-jj-1)=waveletDomain(x_hi,y_lo);
		  copy_buffer(size-ii-1,size-jj-1)=waveletDomain(x_hi,y_hi);
 	  }
  }

 return recursiveWaveletLookup(copy_buffer,out_buffer,copy_buffer_1D,out_buffer_1D,lo_filter,hi_filter,waveletDomain,level-1,coords,ncof);

}
template <class image_>
void
	dbbgm_wavelet_engine_GE<image_>::transformMinimal2DBuffer(const image_& in,image_& out,image_& in_1D,image_& out_1D,
	int dimension, float* lo_filter, float* hi_filter,int ncof)
{
	
	//image_* work_buffer =bbgm_wavelet_image_traits<image_>::init(size_one,1);	

	int ncontrib=(ncof-1)/2;
	int size=2*(2*ncontrib+1);
	
	in_1D =bbgm_wavelet_image_traits<image_>::zeroVal(in_1D);
	for (int i=0;i<size;i+=1)
	{
		int count=0;
	   	for (int j=0;j<size;j++,count++)
			if (dimension==0)
			{
				in_1D(count,0)=in(j,i);
			}else{
				in_1D(count,0)=in(i,j);
			}
	   transformMinimalLinearBuffer(in_1D,out_1D,lo_filter,hi_filter,ncof);
	   count=0;
		for (int j=0;j<size;j++,count++)
			if (dimension==0)
			{
				out(j,i)=out_1D(count,0);
			}else{
				out(i,j)=out_1D(count,0);
			}		
	}

}
template <class image_>
void
dbbgm_wavelet_engine_GE<image_>::determine_coordinates(unsigned int x, unsigned int y,unsigned int** coords,unsigned int level)
{
	coords[0][0]=x;
    coords[0][1]=y;
	  
	for (unsigned int k=1;k<=level;k++)
	  {
     	coords[k][0]=coords[k-1][0]/2;
		coords[k][1]=coords[k-1][1]/2;
	  }

}
template <class image_>
void
	dbbgm_wavelet_engine_GE<image_>::transformMinimalLinearBuffer(const image_& buffer,image_& wksp,float* lo_filter,float* hi_filter,int ncof)

{
	
	wksp =bbgm_wavelet_image_traits<image_>::zeroVal(wksp);

	int ncontrib=(ncof-1)/2;
	int size=2*(2*ncontrib+1);
	int h_size=size/2;
	
	for (int k=0;k<=(ncontrib+1);k+=2) //iterates over the required points for above levels
	{
		for (int i=0;i<=ncontrib;i++)			 //iterates over contributors to the point in question in the next level above
		{
		 	int j=2*i;

			if (j+1<ncof)
			wksp(size-1-k,0)=wksp(size-1-k,0)+
				buffer(h_size-1-i-k/2,0)*lo_filter[j+1]+buffer(size-1-i-k/2,0)*hi_filter[j+1];

			wksp(size-2-k,0)=wksp(size-2-k,0)+
					buffer(h_size-1-i-k/2,0)*lo_filter[j]+buffer(size-1-i-k/2,0)*hi_filter[j];
			
		}
	}
   float scale = vcl_max(lo_filter[ncof], hi_filter[ncof]);
   for (int i=0;i<size;wksp(i,0)=wksp(i,0)*scale,i++); //copy back scaled results
}
