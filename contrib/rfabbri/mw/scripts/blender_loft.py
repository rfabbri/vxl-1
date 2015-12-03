#
# Generate ground truth data from ground truth blender file
#
# For instance, open the project
#
# /Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/pavillon_barcelone_v1.2-cam-002.blend
#
# Then select the desired animated camera and set as active camera
#
# Then run this script. It will play frame by frame and export the 3x4 camera
# matrix for that frame, among other things.
#
# Run with:
#
# filename = "/Users/rfabbri/cprg/vxlprg/lemsvxl/contrib/rfabbri/mw/scripts/blender_loft.py"
# exec(compile(open(filename).read(), filename, 'exec'))
import bpy
import bpy_extras
import numpy
from mathutils import Matrix

if __name__ == "__main__":
    print('hello')
    # Create a spline

    
    c0 = bpy.data.curves.new('firstCurveToLoft', 'CURVE')
    o0 = bpy.data.objects.new('firstCurveToLoft', c0)
    bpy.context.scene.objects.link(o0)
    c0.dimensions = "3D"

    p0 = []
    p0.append([0, 0, 0])
    p0.append([1, 0, 0])
    p0.append([0, 1, 0])

    npts = len(p0)

    spline = o0.data.splines.new('BEZIER')
    spline.bezier_points.add(npts - 1) # less one because one point is added when the spline is created.
    for p in range(0, npts):
        # spline.bezier_points[p].co = [p0[p][0], p0[p][1], p0[p][2]]
        spline.bezier_points[p].co = p0[p]
