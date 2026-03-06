# SkiOx Dashboard

A real-time health monitoring dashboard built with Django that displays SpO2 (oxygen saturation) and heart rate data.

## Features

- Real-time SpO2 and heart rate monitoring
- REST API for health data
- Live dashboard with auto-refresh
- Historical data tracking

## Installation

1. Clone the repository:
```bash
git clone https://github.com/YOUR_USERNAME/SkiOx_Dashboard.git
cd SkiOx_Dashboard
```

2. Create a virtual environment:
```bash
python -m venv venv
venv\Scripts\activate  # Windows
source venv/bin/activate  # Linux/Mac
```

3. Install dependencies:
```bash
pip install -r requirements.txt
```

4. Run migrations:
```bash
python manage.py migrate
```

5. Start the server:
```bash
python manage.py runserver
```

6. Open your browser and go to:
```
http://127.0.0.1:8000/
```

## API Endpoints

- `GET /api/health-data/` - Get all health records
- `POST /api/health-data/` - Add new health data
- `GET /api/latest/` - Get the latest health reading

## Tech Stack

- Python
- Django
- Django REST Framework
- SQLite
- HTML/CSS/JavaScript

## License

MIT
