import bpy
import os

# get room data
room_lines = {}
room_bounds = {}
world_portals = []

for obj in bpy.context.scene.objects:
    collection = obj.users_collection[0]

    if obj.type == 'CURVE' and collection.name.startswith("room"):
        if not obj.data.splines: continue

        roomId = int(collection.name.split(" ")[1])
        if roomId not in room_lines: room_lines[roomId] = []

        for spline in obj.data.splines:
            if len(spline.bezier_points) != 2: continue
            line = []
            for point in spline.bezier_points:
                coords = obj.matrix_world @ point.co
                line.append(coords)
            room_lines[roomId].append(line)

    if obj.type == 'MESH' and collection.name.startswith("bound"):
        roomId = int(collection.name.split(" ")[1])
        if roomId not in room_bounds: room_bounds[roomId] = []
        
        normal = (obj.matrix_world.to_3x3() @ obj.data.polygons[0].normal).normalized()
        vert_on_plane = obj.matrix_world @ obj.data.vertices[0].co
        dist = normal.dot(vert_on_plane)

        bound = [normal.x, normal.y, normal.z, dist]
        room_bounds[roomId].append(bound)

    if obj.type == 'MESH' and obj.name.startswith("portal"):
        roomFrom = int(obj.name.split(" ")[1])
        roomTo = int(obj.name.split(" ")[2])
        verts = [obj.matrix_world @ obj.data.vertices[i].co for i in obj.data.polygons[0].vertices]
        world_portals.append({"from": roomFrom, "to": roomTo, "verts": verts})


# save data into a header file

filepath = bpy.data.filepath
directory = os.path.dirname(filepath)

mapData = "#ifndef __MAP_DATA_H__\n"
mapData += "#define __MAP_DATA_H__\n\n"
mapData += "#include \"../src/world.h\"\n\n"

for roomId in room_lines:
    mapData += f"line_t g_world_room_{roomId}_lines[{len(room_lines[roomId])}] = {{\n"

    for line in room_lines[roomId]:
        mapData += "    {{"
        mapData += "{:.3f},{:.3f},{:.3f}".format(line[0][0], line[0][2], line[0][1])
        mapData += "},{"
        mapData += "{:.3f},{:.3f},{:.3f}".format(line[1][0], line[1][2], line[1][1])
        mapData += "}},\n"

    mapData += "};\n\n"

for roomId in room_bounds:
    mapData += f"vector_t g_world_room_{roomId}_bounds[{len(room_bounds[roomId])}] = {{\n"

    for bound in room_bounds[roomId]:
        mapData += "    {"
        mapData += "{:.3f},{:.3f},{:.3f},{:.3f}".format(bound[0], bound[2], bound[1], bound[3])
        mapData += "},\n"

    mapData += "};\n\n"

for portal in world_portals:
    mapData += f"vector_t g_world_portal_{portal['from']}_{portal['to']}_lines[{len(portal['verts'])}] = {{\n"

    for vert in portal['verts']:
        mapData += "    {"
        mapData += "{:.3f},{:.3f},{:.3f}".format(vert[0], vert[2], vert[1])
        mapData += "},\n"

    mapData += "};\n\n"

mapData += f"struct WorldRoomData g_world_rooms[{len(room_lines)}] = {{\n"

for roomId in room_lines:
    mapData += "    {"
    mapData += f"{len(room_lines[roomId])}, "
    mapData += f"g_world_room_{roomId}_lines, "
    mapData += f"{len(room_bounds[roomId])}, "
    mapData += f"g_world_room_{roomId}_bounds"
    mapData += "},\n"

mapData += "};\n\n"

mapData += f"struct WorldPortalData g_world_portals[{len(world_portals)}] = {{\n"

for portal in world_portals:
    mapData += "    {"
    mapData += f"{portal['from']}, "
    mapData += f"{portal['to']}, "
    mapData += f"{len(portal['verts'])}, "
    mapData += f"g_world_portal_{portal['from']}_{portal['to']}_lines"
    mapData += "},\n"

mapData += "};\n\n"

mapData += f"struct WorldData g_world = {{{len(room_lines)}, g_world_rooms, {len(world_portals)}, g_world_portals}};\n\n"

mapData += "#endif\n"

file = open(directory + "\\map.h", 'w')
file.write(mapData)
file.close()