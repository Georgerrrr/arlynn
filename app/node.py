from gi.repository import Gtk, Adw 

NODE_CSS = """
#node {
    background-color: #f8f8f8;
    border: 1px solid black;
    border-radius: 4px;
}

#top-bar {
    background-color: #e8a2a2;
    color: #000000;
    border-bottom: 1px solid black;
    border-radius: 3px 3px 0 0;
}
"""

class Node(Gtk.Grid):
    def __init__(self, name, type):
        super().__init__()

        self.set_name("node")

        self.x = 50
        self.y = 50
        self.width = 100
        self.height = 100 
        self.name = name
        self.coreID = 0 
        self.type = type

        self.set_size_request(self.width, self.height)

        _label = Gtk.Label()
        _label.set_name("top-bar")
        _label.set_text(self.name)
        _label.set_hexpand(True)
        _label.set_halign(Gtk.Align.FILL)
        _label.set_xalign(0.1)

        self.attach(_label, 0, 0, 1, 1)

        self.css_provider = Gtk.CssProvider()
        self.css_provider.load_from_data(NODE_CSS)

        style = self.get_style_context()
        style.add_provider_for_display(self.get_display(), self.css_provider, Gtk.STYLE_PROVIDER_PRIORITY_USER)

