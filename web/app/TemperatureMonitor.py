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





    @cherrypy.expose
    @cherrypy.tools.render ( template="index.mako" )
    def index (self ):
        pass

    
    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data ( self, offset=0, limit=None ):
        data = []
        for temp in TemperatureData.all ( cherrypy.request.db, offset, limit ):
            data.append ( self.json_output ( temp ) )
        
        result = { 'count' : len ( data ), 'data': data }

        return result

    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data1 ( self, limit = 0, offset = 0 ):
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
    def gather_data_by_span1 ( self, start, end ):
        query = "select row_id, time_data, sensor_id, temperature from TemperatureData where time_data between ? and ? order by time_data"
        json_format = lambda val: { 'row_id' : val[0], 'time_data' : val[1], 'sensor_id' : val[2], 'temperature': val[3] }
        data = []
        for row in self.fetch_data ( query, (start, end) ):
            data.append ( json_format ( row ) )

        result = { 'length' : len ( data ), 'data' : data }
        return result



    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data_by_span ( self, start, end ):
        data = []
        for temp in TemperatureData.date_span ( cherrypy.request.db, start, end ):
            data.append ( self.json_output ( temp ) )


        result = { 'count' : len ( data ), 'data' : data }
        return result


