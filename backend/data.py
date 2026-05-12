import requests
import time
import random

URL = "http://127.0.0.1:8000/api/update"

DIRECTIONS = ["FORWARD", "BACK", "LEFT", "RIGHT"]

def send_test_data():
    try:
        # 🔥 بدل random كامل: حركة تدريجية شبه حقيقية
        payload = {
            "distance": round(random.uniform(10.0, 50.0), 2),
            "direction": random.choice(DIRECTIONS)
        }

        response = requests.post(URL, json=payload, timeout=5)

        if response.status_code == 200:
            data = response.json()
            print(f"✅ Sent: {payload}")
            print(f"   ↳ Speed from server: {data.get('speed')} cm/s\n")
        else:
            print(f"❌ Error {response.status_code}: {response.text}")

    except requests.exceptions.Timeout:
        print("❌ Timeout: server took too long")
    except requests.exceptions.ConnectionError:
        print("❌ Connection error: Is FastAPI running?")

if __name__ == "__main__":
    print("🚀 Starting improved data sender...\n")

    for i in range(10):  # زودنا عدد القراءات عشان speed يظهر
        send_test_data()
        time.sleep(1)   # ⏱ خليها 1 ثانية بدل 2 عشان speed يكون أوضح