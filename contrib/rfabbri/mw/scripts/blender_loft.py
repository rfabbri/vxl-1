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

# Create a spline/bezier from a list of points
def new_curve_from_points(p0,name_prefix):
    npts = len(p0)
    c0 = bpy.data.curves.new(name_prefix + 'CurveToLoft', 'CURVE')
    o0 = bpy.data.objects.new(name_prefix + 'CurveToLoft', c0)
    bpy.context.scene.objects.link(o0)
    c0.dimensions = "3D"

    spline = o0.data.splines.new('BEZIER')
    spline.bezier_points.add(npts - 1) # less one because one point is added when the spline is created.
    for p in range(0, npts):
        # spline.bezier_points[p].co = [p0[p][0], p0[p][1], p0[p][2]]
        spline.bezier_points[p].co = p0[p]
    return c0, o0, spline 

def test():
    print('hello')
    p0 = []
    p0.append([1, 0, 0])
    p0.append([0, 0, 0])
    p0.append([0, 1, 0])

    p1 = []
    p1.append([1, 1, 0])
    p1.append([0, 1, 0])
    p1.append([0, 0, 1])
    loft(p0,p1)

def read_files():
    p0_fname = '/home/rfabbri/tmp/p0.txt'
    p1_fname = '/home/rfabbri/tmp/p1.txt'
    p0 = numpy.loadtxt(p0_fname)
    p0.tolist()
    p1 = numpy.loadtxt(p1_fname)
    p1.tolist()
    return p0, p1

def test1():
    p0, p1 = read_files()
    loft(p0, p1)
    bpy.ops.export_mesh.ply(filepath='/home/rfabbri/tmp/loftsurf.ply')

def cleanup():
    bpy.ops.object.mode_set(mode='OBJECT', toggle=True)
#     bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)

def loft(p0, p1):
    c0, o0, s0 = new_curve_from_points(p0,'first')
    c1, o1, s1 = new_curve_from_points(p1,'second')

    # join curves into a group
    bpy.data.objects[o0.name].select = True
    bpy.data.objects[o1.name].select = True
    bpy.context.scene.objects.active = bpy.data.objects[o1.name]
    bpy.ops.object.join('INVOKE_REGION_WIN') # not sure why invoke_region_win

    # create a mesh to store the final surface
    me = bpy.data.meshes.new("outputLoft")
    ob = bpy.data.objects.new("outputLoft", me)
    scn = bpy.context.scene
    scn.objects.link(ob)
    scn.objects.active = ob
    ob.select = True

    # curves + object should be selected
    # call lofting
    bpy.ops.object.mode_set(mode='EDIT', toggle=True)

    bpy.ops.mesh.reveal()
#     bpy.ops.mesh.select_all(action='SELECT')

    # execute any editmode tool
    bpy.ops.gpencil.surfsk_add_surface()

if __name__ == "__main__":
    test1()
