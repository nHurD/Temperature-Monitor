import cgi

import cherrypy

from lib.model.TemperatureData import TemperatureData

__all__ = ['TemperatureMonitor']

class TemperatureMonitor ( object ):

    def json_output ( self, val ): 
        return{ 'row_id' : val.row_id, 'sensor' : val.sensor_id, 'date' : '%s' % val.time_data, \
            'temperature' : val.temperature }


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
    def gather_data_by_span ( self, start, end ):
        data = []
        for temp in TemperatureData.date_span ( cherrypy.request.db, start, end ):
            data.append ( self.json_output ( temp ) )


        result = { 'count' : len ( data ), 'data' : data }
        return result


