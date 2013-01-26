import cgi

import cherrypy

from lib.model.TemperatureData import TemperatureData

__all__ = ['TemperatureMonitor']

class TemperatureMonitor ( object ):

    @cherrypy.expose
    @cherrypy.tools.render(template="index.mako")
    def index (self ):
        pass

    
    @cherrypy.expose
    @cherrypy.tools.json_out ( )
    def gather_data ( self ):
        db = cherrypy.request.db
        data = []
        for temp in db.query ( TemperatureData ):
            data.append( { 'row_id': temp.row_id, 'sensor': temp.sensor_id, 'date': '%s' % temp.time_data, 'temperature': temp.temperature } )
        
        result = { 'count' : 10, 'data': data }

        return result
