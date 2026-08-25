from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import random as rd
app = FastAPI()

origins = [
    "*"  # allow all origins (any frontend can fetch)
    # You can restrict to specific origins like:
    # "http://localhost:5500",
    # "http://192.168.1.42:3000"
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,       # which origins are allowed
    allow_credentials=True,
    allow_methods=["*"],         # allow all HTTP methods
    allow_headers=["*"],         # allow all headers
)

@app.get("/")
def read_root():
    return {
    "current": round(rd.randint(90, 106)/0.99, 2),
    "currentMax": round(rd.randint(90, 106)/0.99, 2),
    "voltage": round(rd.randint(10, 15)/0.99, 2),
    "voltageMax": round(rd.randint(13, 17)/0.99, 2)
}
