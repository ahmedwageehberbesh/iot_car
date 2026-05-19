# GEI IoT Smart Car Project

This project is a smart car system that combines:

- Arduino-based car control and obstacle avoidance
- ESP32 Bluetooth gamepad control (manual and auto mode switch)
- FastAPI backend for telemetry storage and APIs
- Web dashboard for live monitoring

The repository includes firmware sketches, backend APIs, and a browser dashboard.

## Project Structure

- `arduino_car/arduino_car.ino`: Main Arduino car firmware (motor control, ultrasonic sensing, auto/manual mode, serial telemetry output)
- `Esp_Bt_Car/Esp_Bt_Car.ino`: ESP32 firmware using Dabble GamePad to send control commands over UART to Arduino
- `sketch_may1a/sketch_may1a.ino`: Standalone auto-mode Arduino test sketch with serial decision logs
- `backend/rt_from_car.py`: FastAPI server that receives/stores telemetry and serves API endpoints
- `backend/data.py`: Python sender script that simulates car telemetry for backend testing
- `backend/index.html`: Live dashboard (Arabic UI) that visualizes distance/speed from backend

## System Overview

1. ESP32 reads phone gamepad input via Dabble Bluetooth.
2. ESP32 sends commands (`F`, `B`, `L`, `R`, `S`, `A`, `M`) over UART to Arduino.
3. Arduino drives motors and reads ultrasonic distance.
4. Arduino sends periodic telemetry as `distance,direction` over serial.
5. Backend accepts telemetry at `/api/update`, computes speed, and stores readings in SQLite.
6. Dashboard reads `/api/all` every second and updates cards/charts.

## Modes and Commands

- Manual mode (`M`): direct movement commands from gamepad
- Auto mode (`A`): Arduino obstacle-avoidance logic is active

Movement commands used by firmware:

- `F`: forward
- `B`: reverse
- `L`: turn left
- `R`: turn right
- `S`: stop

## Backend API

Base URL (default): `http://127.0.0.1:8000`

- `POST /api/update`
	- Body:
		```json
		{
			"distance": 24.5,
			"direction": "FORWARD"
		}
		```
	- Direction must be one of: `LEFT`, `RIGHT`, `FORWARD`, `BACK`
	- Response includes computed speed

- `GET /api/latest`
	- Returns latest saved row

- `GET /api/all`
	- Returns all rows sorted by newest first

SQLite DB file is created automatically as `backend/smart_car.db` when the server starts.

## How Speed Is Calculated

On each update, the backend reads the two most recent saved distances and timestamps, then computes:

`speed = |distance_1 - distance_2| / time_diff_seconds`

The computed speed is saved with the new reading.

## Run the Project

## 1) Backend Setup

From project root:

```powershell
cd backend
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install fastapi uvicorn requests
```

Start the API server:

```powershell
python rt_from_car.py
```

Server listens on `0.0.0.0:8000`.

## 2) Optional: Send Test Data (without hardware)

In another terminal (with the backend venv activated):

```powershell
cd backend
python data.py
```

This sends sample distance/direction values to `/api/update`.

## 3) Open Dashboard

Open `backend/index.html` in your browser.

The dashboard fetches telemetry from `http://127.0.0.1:8000/api/all` every second.

## 4) Upload Firmware

- Upload `arduino_car/arduino_car.ino` to Arduino Uno (or compatible board)
- Upload `Esp_Bt_Car/Esp_Bt_Car.ino` to ESP32
- Pair phone app (Dabble) with ESP32 Bluetooth name: `MyBluetoothCar`

## Hardware Notes

Main hardware expected by the sketches:

- Arduino + L298N motor driver
- HC-SR04 ultrasonic sensor
- Servo motor for scanning left/right
- ESP32 for Bluetooth gamepad bridge

Current key pin mapping in `arduino_car.ino`:

- Motor: `IN1=2`, `IN2=3`, `IN3=4`, `IN4=5`
- Ultrasonic: `trig=11`, `echo=12`
- Servo: `10`

ESP32 UART pins in `Esp_Bt_Car.ino`:

- `RXD2=16`
- `TXD2=17`

## Troubleshooting

- `Connection error: Is FastAPI running?`
	- Start backend server first (`python rt_from_car.py`)

- CORS/API access issues
	- Backend currently allows all origins (`allow_origins=["*"]`)

- Dashboard not updating
	- Verify API is running at `127.0.0.1:8000`
	- Open browser dev tools and check failed network requests

- Invalid direction value on `POST /api/update`
	- Use only: `LEFT`, `RIGHT`, `FORWARD`, `BACK`

## Future Improvements

- Add endpoint authentication
- Save additional telemetry (battery, obstacle side, motor PWM)
- Add historical filtering and CSV export on dashboard
- Connect real ESP32 Wi-Fi telemetry upload path (partly drafted in comments)

