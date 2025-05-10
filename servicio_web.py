from fastapi import FastAPI
from datetime import datetime
import uvicorn

app = FastAPI()

@app.get("/datetime")
async def get_datetime():
    # Obtener la fecha y hora actual en el formato requerido
    current_datetime = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
    return {"datetime": current_datetime}

if __name__ == "__main__":
    h="127.0.0.0"
    p=8000
    uvicorn.run(app, host=h, port=p)

