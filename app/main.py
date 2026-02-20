import arl, gi, sys

gi.require_version("Gtk", "4.0")
gi.require_version("Adw", "1")
from gi.repository import Gtk, Adw

from nodegraph import NodeGraph 

import nodes.output 
import nodes.oscillator

class Window(Gtk.ApplicationWindow):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.set_default_size(1280, 720)
        self.set_title("Arlynn")

        arl.Init(-1, 48000, 128)

        self.node_graph = NodeGraph()
        self.set_child(self.node_graph)

        node = nodes.output.OutputNode()
        self.node_graph.do_add(node)

class Arlynn(Adw.Application):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.connect("activate", self.on_activate)
        self.connect("shutdown", self.on_close)

    def on_activate(self, app):
        self.win = Window(application=app)
        self.win.present()

    def on_close(self, app):
        arl.Close()

if __name__ == "__main__":
    app = Arlynn(application_id="com.github.arlynn")
    app.run(sys.argv)
