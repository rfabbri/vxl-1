# Loft curves given by files into surface
#
# INPUT
#    p0.txt, p1.txt
#
# OUTPUT
#    loftsurf.ply
#
# USAGE
#
#   Commandline
#      blender -b -nosound -P blender_loft.py
#       
#   Inside blender console
#
#      filename = "PWD/blender_loft.py"
#      exec(compile(open(filename).read(), filename, 'exec'))
#
# AUTHOR
#   Ricardo Fabbri <rfabbri@gmail.com>, 3 Dec 2015
#
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
    spline.bezier_points.add(npts - 1) 
    # ^-- less one because one point is added when the spline is created.
    for p in range(0, npts):
        spline.bezier_points[p].co = p0[p]
        # spline.bezier_points[p].co = [p0[p][0], p0[p][1], p0[p][2]]
    return c0, o0, spline 

def read_files():
    p0_fname = 'p0.txt'
    p1_fname = 'p1.txt'
    p0 = numpy.loadtxt(p0_fname)
    p0.tolist()
    p1 = numpy.loadtxt(p1_fname)
    p1.tolist()
    return p0, p1

# to be called inside console after testing
def cleanup():
    bpy.ops.object.mode_set(mode='OBJECT', toggle=True)
    bpy.ops.object.delete(use_global=False)

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

def test1():
    # cleanup()
    p0, p1 = read_files()
    loft(p0, p1)
    bpy.ops.export_mesh.ply(filepath='loftsurf.ply')

def loft(p0, p1):
    c0, o0, s0 = new_curve_from_points(p0,'first')
    c1, o1, s1 = new_curve_from_points(p1,'second')

    # join curves into a group
    bpy.data.objects[o0.name].select = True
    bpy.data.objects[o1.name].select = True
    bpy.context.scene.objects.active = bpy.data.objects[o1.name]
    bpy.ops.object.join('INVOKE_REGION_WIN')

    # create a mesh to store the final surface
    me = bpy.data.meshes.new("outputLoft")
    ob = bpy.data.objects.new("outputLoft", me)
    scn = bpy.context.scene
    scn.objects.link(ob)
    scn.objects.active = ob
    ob.select = True

    # curves + object should be selected
    # call lofting
    bpy.ops.object.mode_set('INVOKE_REGION_WIN', mode='EDIT', toggle=True)
    bpy.ops.mesh.reveal()
    bpy.ops.gpencil.surfsk_add_surface('INVOKE_DEFAULT')

if __name__ == "__main__":
    test1()
    bpy.ops.wm.quit_blender()

