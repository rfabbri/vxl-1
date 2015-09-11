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
#filename = "/Users/rfabbri/cprg/vxlprg/lemsvxl/contrib/rfabbri/mw/scripts/blender_cameras.py"
#exec(compile(open(filename).read(), filename, 'exec'))

import bpy
import bpy_extras
import numpy
from mathutils import Matrix

#------------------------------------------------------------------------
# 3x4 P matrix to Blender camera


# Build intrinsic camera parameters from Blender camera data
#
# See notes on this in 
# blender.stackexchange.com/questions/15102/what-is-blenders-camera-projection-matrix-model
#
# Minor notes:
#   See also libmv/src/ui/tvr/tvr_document.h for the inverse
#   Some relevant blender branches: blender:multiview, libmv_prediction
def get_calibration_matrix_K_from_blender(camd):
    f_in_mm = camd.lens
    scene = bpy.context.scene
    resolution_x_in_px = scene.render.resolution_x
    resolution_y_in_px = scene.render.resolution_y
    scale = scene.render.resolution_percentage / 100
    sensor_width_in_mm = camd.sensor_width
    sensor_height_in_mm = camd.sensor_height
    aspect_ratio = scene.render.pixel_aspect_x / scene.render.pixel_aspect_y
    # the sensor width is fixed, the sensor height is effectively changed with the
    # aspect ratio
    s_u = resolution_x_in_px * scale / sensor_width_in_mm
    s_v = resolution_y_in_px * scale * aspect_ratio / sensor_height_in_mm

    # Parameters of intrinsic calibration matrix K
    alpha_u = f_in_mm * s_u
    alpha_v = f_in_mm * s_v
    u_0 = resolution_x_in_px * scale / 2
    v_0 = resolution_y_in_px * scale / 2
    skew = 0 # only use rectangular pixels

    K = Matrix(
        ((alpha_u, skew,    u_0),
        (    0  , alpha_v, v_0),
        (    0  , 0,        1 )))
    return K

# Returns camera rotation and translation matrices from Blender.
# 
# There are 3 coordinate systems involved:
#    * The World coordinates: "world"
#       - right-handed
#    * The Blender camera coordinates: "bcam"
#       - x is horizontal
#       - y is up
#       - right-handed: negative z look-at direction
#    * The desired computer vision camera coordinates: "cv"
#       - x is horizontal
#       - y is down (to align to the actual pixel coordinates used in digital images)
#       - right-handed: positive z look-at direction
def get_3x4_RT_matrix_from_blender(cam):
    # bcam stands for blender camera
    R_bcam2cv = Matrix(
        ((1, 0,  0),
         (0, -1, 0),
         (0, 0, -1)))

    # Transpose since the rotation is object rotation, and we want coordinate
    # rotation
    # R_world2bcam = cam.rotation_euler.to_matrix().transposed()
    location, rotation = cam.matrix_world.decompose()[0:2]
    R_world2bcam = rotation.to_matrix().transposed()

    # Convert camera location to translation vector used in coordinate changes
    T_world2bcam = -1*R_world2bcam * location

    # Build the coordinate transform matrix from world to computer vision camera
    R_world2cv = R_bcam2cv*R_world2bcam
    T_world2cv = R_bcam2cv*T_world2bcam

    # put into 3x4 matrix
    RT = Matrix((
        R_world2cv[0][:] + (T_world2cv[0],),
        R_world2cv[1][:] + (T_world2cv[1],),
        R_world2cv[2][:] + (T_world2cv[2],)
         ))
    return RT

# You can use it this way but it won't work for certain animations
# unless you bake them. I baked my path-constrained camera 
# by object -> animation bake then selecting all options but the last
def get_3x4_RT_matrix_from_blender_without_matrix_world(cam):
    # bcam stands for blender camera
    R_bcam2cv = Matrix(
        ((1, 0,  0),
         (0, -1, 0),
         (0, 0, -1)))

    # Transpose since the rotation is object rotation, and we want coordinate
    # rotation
    R_world2bcam = cam.rotation_euler.to_matrix().transposed()
#     location, rotation = cam.matrix_world.decompose()[0:2]
#     R_world2bcam = rotation.to_matrix().transposed()

    # Convert camera location to translation vector used in coordinate changes
#     T_world2bcam = -1*R_world2bcam * location
    T_world2bcam = -1*R_world2bcam * cam.location

    # Build the coordinate transform matrix from world to computer vision camera
    R_world2cv = R_bcam2cv*R_world2bcam
    T_world2cv = R_bcam2cv*T_world2bcam

    # put into 3x4 matrix
    RT = Matrix((
        R_world2cv[0][:] + (T_world2cv[0],),
        R_world2cv[1][:] + (T_world2cv[1],),
        R_world2cv[2][:] + (T_world2cv[2],)
         ))
    return RT

def get_3x4_P_matrix_from_blender(cam):
    K = get_calibration_matrix_K_from_blender(cam.data)
    RT = get_3x4_RT_matrix_from_blender(cam)
    return K*RT, K, RT


#------------------------------------------------------------------------
# From http://blender.stackexchange.com/questions/882/how-to-find-image-coordinates-of-the-rendered-vertex?lq=1
def project_by_object_utils(cam, point):
    scene = bpy.context.scene
    # obj = bpy.context.object
    # co = bpy.context.scene.cursor_location
    co_2d = bpy_extras.object_utils.world_to_camera_view(scene, cam, point)
    # print("2D Coords:", co_2d)

    # If you want pixel coords
    render_scale = scene.render.resolution_percentage / 100
    render_size = (
            int(scene.render.resolution_x * render_scale),
            int(scene.render.resolution_y * render_scale),
            )
    return Vector((co_2d.x * render_size[0], render_size[1] - co_2d.y * render_size[1]))
#     print("Pixel Coords:", (
#           round(co_2d.x * render_size[0]),
#           round(co_2d.y * render_size[1]),
#           ))


#------------------------------------------------------------------------

s = "2- time: sunset"

def view_plane(camd, winx, winy, xasp, yasp):    
    #/* fields rendering */
    ycor = yasp / xasp
    use_fields = False
    if (use_fields):
      ycor *= 2

    def BKE_camera_sensor_size(p_sensor_fit, sensor_x, sensor_y):
        #/* sensor size used to fit to. for auto, sensor_x is both x and y. */
        if (p_sensor_fit == 'VERTICAL'):
            return sensor_y;

        return sensor_x;

    if (camd.type == 'ORTHO'):
      #/* orthographic camera */
      #/* scale == 1.0 means exact 1 to 1 mapping */
      pixsize = camd.ortho_scale
    else:
      #/* perspective camera */
      sensor_size = BKE_camera_sensor_size(camd.sensor_fit, camd.sensor_width, camd.sensor_height)
      pixsize = (sensor_size * camd.clip_start) / camd.lens

    #/* determine sensor fit */
    def BKE_camera_sensor_fit(p_sensor_fit, sizex, sizey):
        if (p_sensor_fit == 'AUTO'):
            if (sizex >= sizey):
                return 'HORIZONTAL'
            else:
                return 'VERTICAL'

        return p_sensor_fit

    sensor_fit = BKE_camera_sensor_fit(camd.sensor_fit, xasp * winx, yasp * winy)

    if (sensor_fit == 'HORIZONTAL'):
      viewfac = winx
    else:
      viewfac = ycor * winy

    pixsize /= viewfac

    #/* extra zoom factor */
    pixsize *= 1 #params->zoom

    #/* compute view plane:
    # * fully centered, zbuffer fills in jittered between -.5 and +.5 */
    xmin = -0.5 * winx
    ymin = -0.5 * ycor * winy
    xmax =  0.5 * winx
    ymax =  0.5 * ycor * winy

    #/* lens shift and offset */
    dx = camd.shift_x * viewfac # + winx * params->offsetx
    dy = camd.shift_y * viewfac # + winy * params->offsety

    xmin += dx
    ymin += dy
    xmax += dx
    ymax += dy

    #/* fields offset */
    #if (params->field_second):
    #    if (params->field_odd):
    #        ymin -= 0.5 * ycor
    #        ymax -= 0.5 * ycor
    #    else:
    #        ymin += 0.5 * ycor
    #        ymax += 0.5 * ycor

    #/* the window matrix is used for clipping, and not changed during OSA steps */
    #/* using an offset of +0.5 here would give clip errors on edges */
    xmin *= pixsize
    xmax *= pixsize
    ymin *= pixsize
    ymax *= pixsize

    return xmin, xmax, ymin, ymax


def projection_matrix(camd):
    r = bpy.context.scene.render
    left, right, bottom, top = view_plane(camd, r.resolution_x, r.resolution_y, 1, 1)

    farClip, nearClip = camd.clip_end, camd.clip_start

    Xdelta = right - left
    Ydelta = top - bottom
    Zdelta = farClip - nearClip

    mat = [[0]*4 for i in range(4)]

    mat[0][0] = nearClip * 2 / Xdelta
    mat[1][1] = nearClip * 2 / Ydelta
    mat[2][0] = (right + left) / Xdelta #/* note: negate Z  */
    mat[2][1] = (top + bottom) / Ydelta
    mat[2][2] = -(farClip + nearClip) / Zdelta
    mat[2][3] = -1
    mat[3][2] = (-2 * nearClip * farClip) / Zdelta

    return mat
#    return sum([c for c in mat], [])

def next_frame():
#    bpy.data.scenes[s].frame_set(bpy.data.scenes[s].frame_current+1)
     bpy.context.scene.frame_set(bpy.context.scene.frame_current + 1)

def set_frame(i):
#     bpy.data.scenes[s].frame_set(i)
     bpy.context.scene.frame_set(i)

def get_cam():
    return get_3x4_P_matrix_from_blender(bpy.context.object)[0]
#    return get_3x4_P_matrix_from_blender(bpy.data.objects['Camera.004'])[0]

# ----------------------------------------------------------------------------
def test():

# For simple tests:
    cam = bpy.data.objects['Camera.001']

# For the sunset set
#     cam = bpy.data.objects['Camera.004']
    P, K, RT = get_3x4_P_matrix_from_blender(cam)
    print("K")
    print(K)
    print("RT")
    print(RT)
    print("P")
    print(P)

    print("==== Tests ====")
    e1 = Vector((1, 0,    0, 1))
    e2 = Vector((0, 1,    0, 1))
    e3 = Vector((0, 0,    1, 1))
    O  = Vector((0, 0,    0, 1))

    p1 = P * e1
    p1 /= p1[2]
    print("Projected e1")
    print(p1)
    print("proj by object_utils")
    print(project_by_object_utils(cam, Vector(e1[0:3])))

    p2 = P * e2
    p2 /= p2[2]
    print("Projected e2")
    print(p2)
    print("proj by object_utils")
    print(project_by_object_utils(cam, Vector(e2[0:3])))

    p3 = P * e3
    p3 /= p3[2]
    print("Projected e3")
    print(p3)
    print("proj by object_utils")
    print(project_by_object_utils(cam, Vector(e3[0:3])))

    pO = P * O
    pO /= pO[2]
    print("Projected world origin")
    print(pO)
    print("proj by object_utils")
    print(project_by_object_utils(cam, Vector(O[0:3])))


    # save the 3x4 P matrix into a plain text file
    nP = numpy.matrix(P)
    numpy.savetxt("/tmp/P3x4.txt", nP)  # to select precision, use e.g. fmt='%.2f'
    
# ----------------------------------------------------------------------------
#def test2():
    # get vertices of selected object
    # get object ransforms
    # project
    # plot

if __name__ == "__main__":
    set_frame(1)
    test()
    
#    pm = get_cam()

#    for i in range(1,101):
#        # Extrinsic transform matrix
#        pm = get_cam()
#        nP = numpy.matrix(pm)
#        fname = "/tmp/%03d.projmatrix" %  i
#        print("writing " + fname)
#        numpy.savetxt(fname, nP)  # to select precision, use e.g. fmt='%.2f'
#        # Advance animation frame
#        test()
#        next_frame()
