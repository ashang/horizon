#pragma once
#include "core/clipboard.hpp"
#include "core/core_package.hpp"
#include "core/core_padstack.hpp"
#include "core/core_schematic.hpp"
#include "core/core_symbol.hpp"
#include "core/cores.hpp"
#include "imp_interface.hpp"
#include "keyseq_dialog.hpp"
#include "main_window.hpp"
#include "pool/pool.hpp"
#include "preferences.hpp"
#include "selection_filter_dialog.hpp"
#include "util/window_state_store.hpp"
#include "widgets/spin_button_dim.hpp"
#include "widgets/warnings_box.hpp"
#include "action.hpp"
#include <zmq.hpp>

#ifdef G_OS_WIN32
#undef DELETE
#undef DUPLICATE
#endif

namespace horizon {

class PoolParams {
public:
    PoolParams(const std::string &bp, const std::string &cp = "") : base_path(bp), cache_path(cp)
    {
    }
    std::string base_path;
    std::string cache_path;
};

std::unique_ptr<Pool> make_pool(const PoolParams &params);

class ImpBase : public sigc::trackable {
    friend class ImpInterface;

public:
    ImpBase(const PoolParams &params);
    void run(int argc, char *argv[]);
    virtual void handle_tool_change(ToolID id);
    virtual void construct() = 0;
    void canvas_update_from_pp();
    virtual ~ImpBase()
    {
    }

    std::set<ObjectRef> highlights;
    virtual void update_highlights(){};

protected:
    MainWindow *main_window;
    CanvasGL *canvas;
    class PropertyPanels *panels;
    WarningsBox *warnings_box;
    class ToolPopover *tool_popover;
    Gtk::Menu *context_menu = nullptr;
    SpinButtonDim *grid_spin_button;
    std::unique_ptr<SelectionFilterDialog> selection_filter_dialog;

    std::unique_ptr<Pool> pool;
    Cores core;
    std::unique_ptr<ClipboardManager> clipboard = nullptr;
    std::unique_ptr<KeySequenceDialog> key_sequence_dialog = nullptr;
    std::unique_ptr<ImpInterface> imp_interface = nullptr;
    Glib::RefPtr<Glib::Binding> grid_spacing_binding;

    std::map<std::pair<ActionID, ToolID>, ActionConnection> action_connections;
    ActionConnection &connect_action(ToolID tool_id, std::function<void(const ActionConnection &)> cb);
    ActionConnection &connect_action(ToolID tool_id);
    ActionConnection &connect_action(ActionID action_id, std::function<void(const ActionConnection &)> cb);

    class RulesWindow *rules_window = nullptr;

    zmq::context_t zctx;
    zmq::socket_t sock_broadcast_rx;
    zmq::socket_t sock_project;
    bool sockets_connected = false;
    bool no_update = false;

    virtual void canvas_update() = 0;
    void sc(void);
    bool handle_key_press(GdkEventKey *key_event);
    void handle_cursor_move(const Coordi &pos);
    bool handle_click(GdkEventButton *button_event);
    bool handle_click_release(GdkEventButton *button_event);
    bool handle_context_menu(GdkEventButton *button_event);
    void tool_process(const ToolResponse &resp);
    void tool_begin(ToolID id);
    void add_tool_button(ToolID id, const std::string &label, bool left = true);
    void handle_warning_selected(const Coordi &pos);
    virtual bool handle_broadcast(const json &j);
    bool handle_close(GdkEventAny *ev);
    json send_json(const json &j);

    bool trigger_action(const std::pair<ActionID, ToolID> &action);
    bool trigger_action(ActionID aid);
    bool trigger_action(ToolID tid);

    void add_tool_action(ToolID tid, const std::string &action);
    Glib::RefPtr<Gio::Menu> add_hamburger_menu();

    ImpPreferences preferences;
    class ImpPreferencesWindow *preferences_window = nullptr;

    virtual CanvasPreferences *get_canvas_preferences()
    {
        return &preferences.canvas_non_layer;
    }

    std::unique_ptr<WindowStateStore> state_store = nullptr;

    virtual void handle_maybe_drag();

    virtual ActionCatalogItem::Availability get_editor_type_for_action() const = 0;

    void layer_up_down(bool up);
    void goto_layer(int layer);

    Gtk::Button *create_action_button(std::pair<ActionID, ToolID> action);

    void set_action_sensitive(std::pair<ActionID, ToolID>, bool v);
    bool get_action_sensitive(std::pair<ActionID, ToolID>) const;
    virtual void update_action_sensitivity();

    typedef sigc::signal<void> type_signal_action_sensitive;
    type_signal_action_sensitive signal_action_sensitive()
    {
        return s_signal_action_sensitive;
    }

private:
    void fix_cursor_pos();
    void apply_settings();
    Glib::RefPtr<Gio::FileMonitor> preferences_monitor;
    void show_preferences_window();
    void handle_drag();
    void update_selection_label();


    ActionConnection &connect_action(ActionID action_id, ToolID tool_id,
                                     std::function<void(const ActionConnection &)> cb);
    KeySequence2 keys_current;
    bool handle_action_key(GdkEventKey *ev);
    void handle_tool_action(const ActionConnection &conn);

    class LogWindow *log_window = nullptr;
    std::set<SelectableRef> selection_for_drag_move;
    Coordf cursor_pos_drag_begin;
    Coordi cursor_pos_grid_drag_begin;

    std::map<std::pair<ActionID, ToolID>, bool> action_sensitivity;
    type_signal_action_sensitive s_signal_action_sensitive;
};
} // namespace horizon
