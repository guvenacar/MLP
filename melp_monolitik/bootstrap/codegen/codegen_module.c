// ============================================================================
// MELP Modular Codegen - codegen_module.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// Module tracking (seen, mark)

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

void codegen_mark_module_seen(Codegen* gen, const char* module_name) {
    ModuleTracker* tracker = malloc(sizeof(ModuleTracker));
    tracker->module_name = malloc(strlen(module_name) + 1);
    strcpy(tracker->module_name, module_name);
    tracker->next = gen->modules_seen;
    gen->modules_seen = tracker;
}
