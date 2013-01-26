import template

import cherrypy
from cherrypy import plugins
from mako.lookup import TemplateLookup

__all__ = ['MakoPlugin']

class MakoPlugin ( plugins.SimplePlugin ):

    def __init__ ( self, bus, base_dir=None, base_cache_dir=None, collection_size=50, encoding='utf-8' ):
 
        plugins.SimplePlugin.__init__( self, bus )

        self.base_dir = base_dir
        self.base_cache_dir = base_cache_dir or tmpfile.gettempdir ( )
        self.encoding = encoding
        self.collection_size = collection_size
        self.lookup = None

    def start ( self ):
        self.bus.log ( "Setting up Mako" )
        
        self.lookup = TemplateLookup ( directories = self.base_dir,
                                       module_directory = self.base_cache_dir,
                                       input_encoding = self.encoding,
                                       output_encoding = self.encoding,
                                       collection_size = self.collection_size
                                     )

        self.bus.subscribe ( "lookup-template", self.get_template )


    def stop ( self ):
        self.bus.log ( "Freeing Mako Resources" )

        self.bus.unsubscribe ( "lookup-template" )
        self.lookup = None


    def get_template ( self, name ):
        return self.lookup.get_template ( name )
