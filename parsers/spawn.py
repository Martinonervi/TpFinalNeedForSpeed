import json
import yaml

INPUT = "spawn2.tmj"
OUTPUT = "spawn2.yaml"


RECT_LAYER = "spawn2" 


with open(INPUT, "r", encoding="utf-8") as f:
    mapa = json.load(f)


def get_layer(name):
    return next(
        l for l in mapa["layers"]
        if l["name"] == name and l["type"] == "objectgroup"
    )


rect_layer = get_layer(RECT_LAYER)
rects = []

objetos = sorted(rect_layer["objects"], key=lambda o: o["y"])

for obj in objetos:
    if obj.get("shape", "rectangle") != "rectangle":
        continue

    # centro
    cx = obj["x"] + obj["width"] / 2.0
    cy = obj["y"] + obj["height"] / 2.0

    # ancho y alto
    w = obj["width"]
    h = obj["height"]

    angle_deg = obj.get("rotation", 0.0)

    rects.append({
        "x": float(cx),
        "y": float(cy),
        "w": float(w),
        "h": float(h),
        "angle": float(angle_deg),
    })

data = {
    "rectangles": rects,
}

with open(OUTPUT, "w", encoding="utf-8") as f:
    yaml.dump(data, f, sort_keys=False)

print(f"Generado {OUTPUT} con {len(rects)} rectángulos.") 
