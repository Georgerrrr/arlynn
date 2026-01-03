#include <cstdio>

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>

#include "configure.h"

#include "core/audioengine.h"

int main(int argc, char** argv) {
  auto app = Gtk::Application::create("uk.co.ridoutweb.arlynn");

  auto builder = Gtk::Builder::create_from_file(RESOURCE_PATH "/ui/arlynn.ui");
  auto main = builder->get_widget<Gtk::Window>("MainWindow");

  auto& audioengine = core::AudioEngine::get();

  app->signal_activate().connect([&] () { 
      audioengine.initialize(48000, 128);

      app->add_window(*main);
      main->set_visible();
      });

  app->signal_shutdown().connect([&] () { audioengine.close(); });

  return app->run(argc, argv);
  /*

  return app->make_window_and_run<MyWindow>(argc, argv);
  core::AudioEngine::get().initialize(48000, 128);
  core::AudioEngine::get().close();

  */
}

