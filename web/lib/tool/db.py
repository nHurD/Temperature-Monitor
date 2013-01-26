import cherrypy

__all__ = ['DBTool']

class DBTool ( cherrypy.Tool ):

    def __init__ ( self ):
        cherrypy.Tool.__init__ ( self,
                                 "on_start_resource",
                                 self.bind_session,
                                 priority = 20
                               )

    def _setup ( self ):
        cherrypy.Tool._setup ( self )
        cherrypy.requests.hooks.attach ( "on_end_resource",
                                         self.commit_transaction,
                                         priority = 80
                                       )

    def bind_session ( self ):
        session = cherrypy.engie.publish ( "bind-session" ).pop ( )
        cherrypy.request.db = session

    def commit_transaction ( self ):
        if not hasattr ( cherrypy.request, "db" ):
            return

        cherrypy.request.db = None
        cherrypy.engie.publish ( "commit-session" )