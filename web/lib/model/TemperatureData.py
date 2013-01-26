from sqlalchemy import Column
from sqlalchemy.types import Integer, String, Float, DateTime

from lib.model import Base

# Table informaiton
#    row_id INTEGER PRIMARY KEY AUTOINCREMENT
#    time_data   DATETIME
#    sensor_id   TEXT
#    temperature REAL

__all__ = ['TemperatureDAta']

class TemperatureData ( Base ):
    __tablename__ =  "TemperatureData"

    row_id      = Column ( Integer, primary_key=True )
    time_data   = Column ( DateTime )
    sensor_id   = Column ( String )
    temperature = Column ( Float )

    def __init__ ( self, row_id, time_data, sensor_id, temperature ):
        self.row_id         = row_id
        self.time_data      = time_data
        self.sensor_id      = sensor_id
        self.temperature    = temperature

   
