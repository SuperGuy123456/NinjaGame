import xml.etree.ElementTree as ET

CHUNK_SIZE = 10   # tiles per chunk (10x10)

COLLISION_GROUPS = {
    "passable": [-1, 6],
    "rigid":    [0,1,2,3,4,5,7,8,9,10,11,12,13,14],
    "danger":   [],
    "grass":    [15],
}

CATEGORY_ID = {
    "passable": 0,
    "rigid":    1,
    "danger":   2,
    "grass":    3
}

DEFAULT_COLLISION = CATEGORY_ID["passable"]

tile_to_collision = {}
for category, ids in COLLISION_GROUPS.items():
    for tid in ids:
        tile_to_collision[tid] = CATEGORY_ID[category]


def parse_tmx(path):
    tree = ET.parse(path)
    root = tree.getroot()

    map_w = int(root.get("width"))
    map_h = int(root.get("height"))

    layer = root.find("layer")
    csv_data = layer.find("data").text.strip()

    flat = [int(x) for x in csv_data.split(",")]
    grid = [flat[i:i+map_w] for i in range(0, len(flat), map_w)]

    chunks_x = map_w // CHUNK_SIZE
    chunks_y = map_h // CHUNK_SIZE

    all_chunks = []

    for cy in range(chunks_y):
        for cx in range(chunks_x):

            chunk = []
            col_chunk = []

            for row in grid[cy*CHUNK_SIZE:(cy+1)*CHUNK_SIZE]:
                tex_row = []
                col_row = []

                for tid in row[cx*CHUNK_SIZE:(cx+1)*CHUNK_SIZE]:
                    engine_tid = tid - 1

                    if engine_tid == 15:  # grass tile
                        tex_row.append(-1)  # do NOT draw
                        col_row.append(CATEGORY_ID["grass"])
                    else:
                        tex_row.append(engine_tid)
                        col_row.append(tile_to_collision.get(engine_tid, DEFAULT_COLLISION))

                chunk.append(tex_row)
                col_chunk.append(col_row)

            # <-- FIX: append AFTER finishing the whole chunk
            all_chunks.append((chunk, col_chunk))

    return all_chunks


def write_chunks(chunks, out_path):
    with open(out_path, "w") as f:
        for tex, col in chunks:
            for row in tex:
                f.write(",".join(map(str, row)) + ",\n")
            for row in col:
                f.write(",".join(map(str, row)) + ",\n")
            f.write("\n")


if __name__ == "__main__":
    chunks = parse_tmx("Level1.tmx")
    write_chunks(chunks, "Level1_parsed.txt")
    print("Chunking complete.")
