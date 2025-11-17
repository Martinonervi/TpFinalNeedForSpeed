import json
import yaml

INPUT = "mapa_tiled.tmj"
OUTPUT = "map_buildings.yaml"

CITY_NAME = "Liberty City"  # cambialo si querés

with open(INPUT, "r", encoding="utf-8") as f:
    mapa = json.load(f)

layer = next(
    l for l in mapa["layers"]
    if l["name"] == "Colisiones" and l["type"] == "objectgroup"
)

buildings = []

for obj in layer["objects"]:
    if obj.get("shape", "rectangle") != "rectangle":
        continue

    x_px = obj["x"] + obj["width"] / 2.0
    y_px = obj["y"] + obj["height"] / 2.0
    w_px = obj["width"]
    h_px = obj["height"]

    angle_deg = obj.get("rotation", 0.0)

    buildings.append({
        "x": float(x_px),
        "y": float(y_px),
        "w": float(w_px),
        "h": float(h_px),
        # si tu juego espera radianes, después lo convertís al leer el YAML
        "angle": float(angle_deg),
    })

data = {
    "city": CITY_NAME,
    "buildings": buildings,
}

with open(OUTPUT, "w", encoding="utf-8") as f:
    yaml.dump(data, f, sort_keys=False)

print(f"Generado {OUTPUT} con {len(buildings)} edificios.")
