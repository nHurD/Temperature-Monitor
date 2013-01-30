import cgi

import cherrypy

import sqlite3

from lib.model.TemperatureData import TemperatureData

__all__ = ['TemperatureMonitor']

class TemperatureMonitor ( object ):

    def json_output ( self, val ): 
        return{ 'row_id' : val.row_id, 'sensor' : val.sensor_id, 'date' : '%s' % val.time_data, \
            'temperature' : val.temperature }

    def fetch_data ( self, query, params=None ):
        conn = sqlite3.connect ( '/Users/jrsteele/Projects/BeagleBone/temperature_data.db' )
        curs = conn.cursor ( )
        if params == None:
            curs.execute ( query )
        else:
            curs.execute ( query, params )
        result = curs.fetchall ( )
        curs.close ( )
        conn.close ( )

        return result

    def fetch_sensors ( self ):
        sensors = self.fetch_data ( "SELECT distinct sensor_id from TemperatureData order by sensor_id asc" )
        return sensors


    @cherrypy.expose
    @cherrypy.tools.render ( template="index.mako" )
    def index ( self ):
        sensors = self.fetch_sensors ( )
        data = { 'name' : "%s" % sensors[0] }
        return data

    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def TemperatureData ( self ):
        farenheit = lambda x:  ( (1.8 * x) + 32 )
        json_format = lambda val: { 'time_data' : val[1], 'temperature': round ( farenheit ( val[2] ), 2 ) }
        data = []
        query = "Select row_id, time_data, temperature from TemperatureData where (row_id - ( select row_id from TemperatureData order by row_id limit 1)) % 1000 = 0"

        for row in self.fetch_data ( query ):
            data.append ( json_format ( row ) )

        result = { 'count' : len ( data ), 'data' : data }
        return result



    
    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data ( self, limit = 0, offset = 0 ):
        query = "select row_id, time_data, sensor_id, temperature from TemperatureData order by time_data"
        query += " limit {0} offset {1}".format ( limit, offset ) if limit > 0 else ""

        json_format = lambda val: { 'row_id' : val[0], 'time_data' : val[1], 'sensor_id' : val[2], 'temperature': val[3] }
        data = []
        for row in self.fetch_data ( query ):
            data.append ( json_format ( row ) )

        result = { 'length': len ( data ), 'data' : data }

        return result

    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data_by_span ( self, start, end ):
        query = "select row_id, time_data, sensor_id, temperature from TemperatureData where time_data between ? and ? order by time_data"
        json_format = lambda val: { 'row_id' : val[0], 'time_data' : val[1], 'sensor_id' : val[2], 'temperature': val[3] }
        data = []
        for row in self.fetch_data ( query, (start, end) ):
            data.append ( json_format ( row ) )

        result = { 'length' : len ( data ), 'data' : data }
        return result



