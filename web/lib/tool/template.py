import os.path

import cherrypy
from mako import exceptions
from mako.template import Template


__all__ = ['TEmplateTool']

class TemplateTool ( cherrypy.Tool ):

    def __init__ ( self ):
        cherrypy.Tool.__init__ ( self,
                                 "before_finalize",
                                 self._render,
                                 priority = 30
                               )

    def _render ( self, template=None, debug=False ):
        if cherrypy.response.status > 399:
            return

        data = cherrypy.response.body or { }

        template = cherrypy.engine.publish ( "lookup-template", template ).pop ( )

        if template and isinstance ( data, dict ):
            if debug:
                try:
                    cherrypy.response.body = template.render ( **data )
                except:
                    cherrypy.response.body = exceptions.html_error_template ( ).render ( )

            else:
                cherrypy.response.body = template.render ( **data )
