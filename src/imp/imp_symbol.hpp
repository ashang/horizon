#pragma once
#include "imp.hpp"

namespace horizon {
class ImpSymbol : public ImpBase {
public:
    ImpSymbol(const std::string &symbol_filename, const std::string &pool_path);

protected:
    void construct() override;

    ActionCatalogItem::Availability get_editor_type_for_action() const
    {
        return ActionCatalogItem::AVAILABLE_IN_SYMBOL;
    };

private:
    void canvas_update() override;
    CoreSymbol core_symbol;

    Gtk::Entry *name_entry = nullptr;
    class SymbolPreviewWindow *symbol_preview_window = nullptr;
};
} // namespace horizon
