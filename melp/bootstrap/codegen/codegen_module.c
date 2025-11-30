// MELP Modular Codegen - Module Tracking
// Module import/export takibi

#include "codegen.h"

// ============================================================================
// Module Seen Check
// ============================================================================

int codegen_module_seen(Codegen* gen, const char* module_name) {
    ModuleTracker* tracker = gen->modules_seen;
    while (tracker) {
        if (strcmp(tracker->module_name, module_name) == 0) {
            return 1;  // Already seen
        }
        tracker = tracker->next;
    }
    return 0;  // Not seen
}

// ============================================================================
// Mark Module Seen
// ============================================================================

void codegen_mark_module_seen(Codegen* gen, const char* module_name) {
    ModuleTracker* tracker = malloc(sizeof(ModuleTracker));
    tracker->module_name = malloc(strlen(module_name) + 1);
    strcpy(tracker->module_name, module_name);
    tracker->next = gen->modules_seen;
    gen->modules_seen = tracker;
}
