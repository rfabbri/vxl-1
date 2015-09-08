# Test the function using the active object (which must be a camera)
# and the 3D cursor as the location to find.
# Note that the pixel coordinates, the y is pointing up instead of down.
# So to get matrix pixel coordinates just do 
#  y = height - y
#
#
#
# From: http://blender.stackexchange.com/questions/882/how-to-find-image-coordinates-of-the-rendered-vertex
import bpy
import bpy_extras

scene = bpy.context.scene
obj = bpy.context.object
co = bpy.context.scene.cursor_location

co_2d = bpy_extras.object_utils.world_to_camera_view(scene, obj, co)
print("2D Coords:", co_2d)

# If you want pixel coords
render_scale = scene.render.resolution_percentage / 100
render_size = (
        int(scene.render.resolution_x * render_scale),
        int(scene.render.resolution_y * render_scale),
        )
print("Pixel Coords:", (
      round(co_2d.x * render_size[0]),
      round(co_2d.y * render_size[1]),
      ))
