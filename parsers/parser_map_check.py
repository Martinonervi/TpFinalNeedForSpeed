import json
import yaml

INPUT = "checkpoints2_tiled.tmj"
OUTPUT = "checkpoint_ruta_2.yaml"

CITY_NAME = "Liberty City"

CHECKPOINTS_LAYER = "checkPoint2CLibertyCity"
with open(INPUT, "r", encoding="utf-8") as f:
    mapa = json.load(f)


def get_layer(name):
    return next(
        l for l in mapa["layers"]
        if l["name"] == name and l["type"] == "objectgroup"
    )


checkpoints_layer = get_layer(CHECKPOINTS_LAYER)
checkpoints = []

objetos = sorted(checkpoints_layer["objects"], key=lambda o: o["y"])

for idx, obj in enumerate(objetos, start=1):   # id = 1,2,3,...
    if obj.get("shape", "rectangle") != "rectangle":
        continue

    cx = obj["x"] + obj["width"] / 2.0
    cy = obj["y"] + obj["height"] / 2.0
    w = obj["width"]
    h = obj["height"]

    angle_deg = obj.get("rotation", 0.0)

    props_raw = {p["name"]: p["value"] for p in obj.get("properties", [])}
    kind = props_raw.get("kind", "Normal")

    cp_id = len(checkpoints) + 1

    checkpoints.append({
        "id": cp_id,
        "kind": kind,
        "x": float(cx),
        "y": float(cy),
        "w": float(w),
        "h": float(h),
        "angle": float(angle_deg),
    })

if checkpoints:
    checkpoints[0]["kind"] = "Start"
    if len(checkpoints) > 1:
        checkpoints[-1]["kind"] = "Finish"

data = {
    "city": CITY_NAME,
    "route": {
        "checkpoints": checkpoints
    }
}

with open(OUTPUT, "w", encoding="utf-8") as f:
    yaml.dump(data, f, sort_keys=False)

print(f"Generado {OUTPUT} con {len(checkpoints)} checkpoints.")
