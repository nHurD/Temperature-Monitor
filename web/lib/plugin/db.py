import cherrypy
from cherrypy.process import wspbus, plugins
from sqlalchemy import create_engine
from sqlalchemy.orm import scoped_session, sessionmaker

from lib.model import Base

__all__ = ['DBEnginePlugin']

class DBEnginePlugin ( plugins.SimplePlugin ):
    def __init__ ( self, bus ):
        plugins.SimplePlugin.__init__ ( self , bus )
        self.sa_engine = None
        self.session = scoped_session ( sessionmaker ( autoflush = True, autocommit = False ) )

    def start ( self ):
        self.bus.log ( "Starting Database Access" )
        self.sa_engine = create_engine ( "sqlite:////Users/jrsteele/Projects/BeagleBone/temperature_data.db", echo=False )
        self.sa_engine.flush = self.no_write
        self.bus.subscribe ( "bind-session", self.bind )


    def stop ( self ):
        self.bus.log ( "Stopping Database Access" )
        self.bus.unsubscribe ( "bind-session", self.bind )
        if self.sa_engine:
            self.sa_engine.dispose ( )
            self.sa_engine = None

    def bind ( self ):
        self.session.configure ( bind=self.sa_engine )
        return self.session

    def no_write (self, *args, **kwargs ):
        return
