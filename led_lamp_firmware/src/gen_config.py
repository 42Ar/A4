import json

b = json.load(open("config.json"))
for i in range(1, 7):
    b["wifi"]["ip"] = f"192.168.1.{20 + i}"
    b["name"] = f"Lamp {i}"
    b["device_id"] = f"main-lamp-{i}"
    json.dump(b, open(f"config-{i}.json", "w"), indent=4)
b["wifi"]["ip"] = f"192.168.1.40"
b["name"] = f"lemon-light"
b["device_id"] = f"lemon-light"
json.dump(b, open(f"config-lemon-light.json", "w"), indent=4)
