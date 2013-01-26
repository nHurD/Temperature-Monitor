import cgi

import cherrypy

from lib.model.TemperatureData import TemperatureData

__all__ = ['TemperatureMonitor']

class TemperatureMonitor ( object ):

    @cherrypy.expose
    @cherrypy.tools.render(template="index.mako")
    def index (self ):
        pass

