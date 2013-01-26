import sys
import logging
from logging import handlers
import os, os.path

import cherrypy
from cherrypy import _cplogging
from cherrypy.lib import httputil

class Server ( object ):
    def __init__ ( self ):
        self.base_dir = os.path.normpath ( os.path.abspath ( options.basedir ) )

        self.conf_path = os.path.join ( self.base_dir, "conf" )

        log_dir = os.path.join ( self.base_dir, "logs" )

        if not os.path.exists ( log_dir ):
            os.mkdir ( log_dir )

        cherrypy.config.update ( self.config_path, "server.cfg" )

        engine = cherrypy.engine

        sys.path.insert ( 0, self.base_dir )


        from lib.tool.template import TemplateTool
        cherrypy.tools.render = TemplateTool ( )

        from lib.tool.db import DBTool
        cherrypy.tools.db = DBTool ( )

        from app.TemperatureMonitor import TemperatureMonitor
        webapp = TemperatureMonitor ( )

        app = cherrypy.tree.mount ( webapp, "/", os.path.join ( self.conf_path, "app.cfg" ) )
        self.make

        