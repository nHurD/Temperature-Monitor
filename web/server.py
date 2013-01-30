import sys
import logging
from logging import handlers
import os, os.path

import cherrypy
from cherrypy import _cplogging
from cherrypy.lib import httputil

class Server ( object ):
    def __init__ ( self, options ):
        self.base_dir = os.path.normpath ( os.path.abspath ( options.basedir ) )

        self.config_path = os.path.join ( self.base_dir, "conf" )

        log_dir = os.path.join ( self.base_dir, "logs" )

        if not os.path.exists ( log_dir ):
            os.mkdir ( log_dir )
        
        server_config = { 'global': {
                                        'server.socket_host': "0.0.0.0",
                                        'server.socket_port': 8090,
                                        'server.thread_pool': 10,
                                        'engine.autoreload_on': False,
                                        'log.error_file': "./logs/error.log",
                                        'log.access_file': "./logs/access.log"
                            }
        }

        cherrypy.config.update ( server_config )
        
        engine = cherrypy.engine

        sys.path.insert ( 0, self.base_dir )


        from lib.tool.template import TemplateTool
        cherrypy.tools.render = TemplateTool ( )

        from app.TemperatureMonitor import TemperatureMonitor
        webapp = TemperatureMonitor ( )
                
        app_config = { '/': {
                                'tools.staticdir.root' : os.path.normpath(os.path.abspath(os.path.curdir)),
                                'tools.staticfile.root' : os.path.normpath(os.path.abspath(os.path.curdir)),
                                'tools.encode.on' : False,
                                'tools.gzip.on' : True,
                                'tools.gzip.mime_types' : ['text/html', 'text/plain', 'application/json', 'text/javascript', 'application/javascript'],
                                'tools.sessions.on' : True,
                                'tools.sessions.storage_type' : "file",
                                'tools.sessions.storage_path' : os.path.join("/tmp/", "temperature_sessions")

                            },
                    '/static': {
                                    'tools.etags.on' : True,
                                    'tools.staticdir.on' : True,
                                    'tools.staticdir.dir' : "public"
                            }
        }

        app = cherrypy.tree.mount ( webapp, "/", app_config )
                
        
        from lib.plugin.template import MakoPlugin
        engine.mako = MakoPlugin ( engine,
                                      os.path.join ( self.base_dir, "template" ),
                                      os.path.join (self.base_dir, "cache" )
                                     )
        engine.mako.subscribe ( )



    def run ( self ):
        engine = cherrypy.engine
                
        if hasattr ( engine, "signal_handler" ):
            engine.signal_handler.subscribe ( )
                
        if hasattr ( engine, "console_control_handler" ):
            engine.console_control_handler.subscribe ( )
                
        
        engine.start()
                
            
        engine.block()
                    
    def on_error(self, status, message, traceback, version):
        code = "404" if status.startswith ( "404" ) else "error"
        template = cherrypy.engine.publish ( "lookup-template", "%s.mako" % code ).pop ( )
        return template.render ( )


if __name__ == '__main__':
    from optparse import OptionParser
    
    def parse_commandline ( ):
        curdir = os.path.normpath ( os.path.abspath ( os.path.curdir ) )
        
        parser = OptionParser ( )
        parser.add_option ( "-b",
                            "--base-dir",
                           dest="basedir",
                           help="Base directory in which the server "\
                           "is launched (default: %s)" % curdir )
        parser.set_defaults ( basedir=curdir )
        ( options, args ) = parser.parse_args ( )
        
        return options
    
    Server ( parse_commandline ( ) ).run ( )
