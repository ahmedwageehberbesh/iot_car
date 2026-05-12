from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Literal
import sqlite3
import logging
from datetime import datetime
import uvicorn

# ==============================
# Logging
# ==============================
logging.basicConfig(level=logging.INFO)

app = FastAPI(title="Smart Car API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

DB_NAME = "smart_car.db"

# ==============================
# Models
# ==============================
class SensorData(BaseModel):
    distance: float
    direction: Literal["LEFT", "RIGHT", "FORWARD", "BACK"]

class SensorResponse(BaseModel):
    id: int
    distance: float
    direction: str
    speed: float
    timestamp: str

# ==============================
# Database Init
# ==============================
def init_db():
    with sqlite3.connect(DB_NAME) as conn:
        cursor = conn.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS sensor_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                distance REAL NOT NULL,
                direction TEXT NOT NULL,
                speed REAL DEFAULT 0.0,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        """)
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_sensor_id ON sensor_data(id DESC)")

init_db()

# ==============================
# API: Receive Data + Speed Calc
# ==============================
@app.post("/api/update")
async def receive_data(data: SensorData):
    try:
        with sqlite3.connect(DB_NAME) as conn:
            cursor = conn.cursor()

            # 🔥 جلب آخر قراءتين (مش واحدة)
            cursor.execute("""
                SELECT distance, timestamp 
                FROM sensor_data 
                ORDER BY id DESC 
                LIMIT 2
            """)
            rows = cursor.fetchall()

            current_speed = 0.0

            # ==============================
            # حساب السرعة الحقيقي
            # ==============================
            if len(rows) == 2:
                last_dist, last_time = rows[0]
                prev_dist, prev_time = rows[1]

                # تنظيف timestamp من microseconds
                last_time = datetime.strptime(last_time.split('.')[0], "%Y-%m-%d %H:%M:%S")
                prev_time = datetime.strptime(prev_time.split('.')[0], "%Y-%m-%d %H:%M:%S")

                time_diff = (last_time - prev_time).total_seconds()
                dist_diff = abs(last_dist - prev_dist)

                if time_diff > 0:
                    current_speed = round(dist_diff / time_diff, 2)

            # ==============================
            # حفظ البيانات الجديدة
            # ==============================
            cursor.execute("""
                INSERT INTO sensor_data (distance, direction, speed)
                VALUES (?, ?, ?)
            """, (data.distance, data.direction, current_speed))

            conn.commit()

        logging.info(f"Saved -> Dist: {data.distance} | Speed: {current_speed}")
        return {"status": "success", "speed": current_speed}

    except Exception as e:
        logging.error(f"Error: {e}")
        raise HTTPException(status_code=500, detail=str(e))

# ==============================
# API: Get All Data
# ==============================
@app.get("/api/all")
async def get_all_data():
    with sqlite3.connect(DB_NAME) as conn:
        cursor = conn.cursor()
        cursor.execute("""
            SELECT id, distance, direction, speed, timestamp 
            FROM sensor_data 
            ORDER BY id DESC
        """)
        rows = cursor.fetchall()

        return [
            {
                "id": r[0],
                "distance": r[1],
                "direction": r[2],
                "speed": r[3],
                "timestamp": r[4]
            }
            for r in rows
        ]

# ==============================
# API: Get Latest Data
# ==============================
@app.get("/api/latest", response_model=SensorResponse)
async def get_latest_data():
    with sqlite3.connect(DB_NAME) as conn:
        cursor = conn.cursor()
        cursor.execute("""
            SELECT id, distance, direction, speed, timestamp 
            FROM sensor_data 
            ORDER BY id DESC 
            LIMIT 1
        """)
        r = cursor.fetchone()

        if r:
            return {
                "id": r[0],
                "distance": r[1],
                "direction": r[2],
                "speed": r[3],
                "timestamp": r[4]
            }

        raise HTTPException(status_code=404, detail="No data found")

# ==============================
# Run Server
# ==============================
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)