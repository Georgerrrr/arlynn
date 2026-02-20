import arl
import gi 

from gi.repository import Gtk, Gdk, Adw, Graphene

class Scroll:
    def __init__(self):
        self.x = 0  
        self.y = 0

        self.dx = 0 
        self.dy = 0 

        self.scrolling = False

    def screen_to_world(self, x, y):
        return [x + self.x, y + self.y]

    def world_to_screen(self, x, y):
        return [self.x - x, self.y - y]

    def edit_scroll(self, mx, my):
        self.x -= mx - self.dx
        self.y -= my - self.dy

        self.dx = mx
        self.dy = my

class NodeGraph(Gtk.Fixed):
    def __init__(self):
        super().__init__()
        self.set_hexpand(True)
        self.set_vexpand(True)

        self.children = []

        evk = Gtk.GestureClick.new()
        evk.connect("pressed",  self.on_click)
        evk.connect("released", self.on_release)
        self.add_controller(evk)

        evk = Gtk.EventControllerMotion.new()
        evk.connect("motion", self.on_motion)
        self.add_controller(evk)

        self.scroll = Scroll()
    
    def on_click(self, gesture, data, x, y):
        self.scroll.scrolling = True
        self.scroll.dx = x
        self.scroll.dy = y

    def on_release(self, gesture, data, x, y):
        self.scroll.scrolling = False

    def on_motion(self, motion, x, y):
        if self.scroll.scrolling:
            self.scroll.edit_scroll(x, y)

            for child in self.children:
                pos = self.scroll.world_to_screen(child.x, child.y)
                self.move(child, pos[0], pos[1])

            self.queue_draw()

    def do_add(self, widget):
        widget.coreID = arl.AddNode(widget.type)

        self.children.append(widget)
        pos = self.scroll.world_to_screen(widget.x, widget.y)
        self.put(widget, pos[0], pos[1])

    def do_remove(self, widget):
        self.children.remove(widget)
        self.remove(widget)

    def do_snapshot(self, s):
        colour = Gdk.RGBA()
        colour.parse("#ffffff")

        rect = Graphene.Rect().init(0, 0, self.get_width(), self.get_height())
        s.append_color(colour, rect)

        for child in self.children:
            self.snapshot_child(child, s)
