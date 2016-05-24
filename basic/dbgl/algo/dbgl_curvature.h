// This is dbgl_curvature.h
#ifndef dbgl_curvature_h_
#define dbgl_curvature_h_
//:
//\file
//\brief Curvature estimation of digital curves
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 02/05/07 14:52:58 EST
//

#include <vcl_vector.h>

//: A simple discrete curvature smoothing technique. Simply move each sample point according to a
// roughly normal direction (bisector at each sample point) and strength.
//
// \param[in,out] c: digital curve (sequence of points). Endpoints remain fixed.
// \param[in] psi: how much to evolve at each step. If in interval [0,1], then it is guaranteed that
// the smoothed curve will not cross itself.
// \param[in] nsteps : number of smoothing iterations
//
// \remarks The algorithm may generate cusps. For example, if the input shape is the cursive "l"
// letter, with one self-intersection, then after some steps a cusp is formed, before desintangling
// the curve. Prof. Ben Kimia called this method "discrete curve shortening".
//


// Curvature estimation by D. Kroons's approach
//
// Original code is Copyright (c) 2011, Dirk-Jan Kroon
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the distribution
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
void dbgl_compute_curvature(
    const std::vector< vgl_point_2d<double> > &vertices, 
    vnl_vector<double> *k
    ) {

  std::cout << "stub" << std::endl;
}



#endif // dbgl_curvature_h

