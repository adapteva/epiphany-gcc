/*
    Copyright (c) 2014-2015 Intel Corporation.  All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of Intel Corporation nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "offload_table.h"
#include "offload_common.h"

#if !HOST_LIBRARY
// Predefined offload entries
extern void omp_set_num_threads_lrb(void*);
extern void omp_get_max_threads_lrb(void*);
extern void omp_get_num_procs_lrb(void*);
extern void omp_set_dynamic_lrb(void*);
extern void omp_get_dynamic_lrb(void*);
extern void omp_set_nested_lrb(void*);
extern void omp_get_nested_lrb(void*);
extern void omp_set_schedule_lrb(void*);
extern void omp_get_schedule_lrb(void*);

extern void omp_init_lock_lrb(void*);
extern void omp_destroy_lock_lrb(void*);
extern void omp_set_lock_lrb(void*);
extern void omp_unset_lock_lrb(void*);
extern void omp_test_lock_lrb(void*);

extern void omp_init_nest_lock_lrb(void*);
extern void omp_destroy_nest_lock_lrb(void*);
extern void omp_set_nest_lock_lrb(void*);
extern void omp_unset_nest_lock_lrb(void*);
extern void omp_test_nest_lock_lrb(void*);

// Predefined entries on the target side
static FuncTable::Entry predefined_entries[] = {
    "omp_set_num_threads_target",
    (void*) &omp_set_num_threads_lrb,
    "omp_get_max_threads_target",
    (void*) &omp_get_max_threads_lrb,
    "omp_get_num_procs_target",
    (void*) &omp_get_num_procs_lrb,
    "omp_set_dynamic_target",
    (void*) &omp_set_dynamic_lrb,
    "omp_get_dynamic_target",
    (void*) &omp_get_dynamic_lrb,
    "omp_set_nested_target",
    (void*) &omp_set_nested_lrb,
    "omp_get_nested_target",
    (void*) &omp_get_nested_lrb,
    "omp_set_schedule_target",
    (void*) &omp_set_schedule_lrb,
    "omp_get_schedule_target",
    (void*) &omp_get_schedule_lrb,

    "omp_init_lock_target",
    (void*) &omp_init_lock_lrb,
    "omp_destroy_lock_target",
    (void*) &omp_destroy_lock_lrb,
    "omp_set_lock_target",
    (void*) &omp_set_lock_lrb,
    "omp_unset_lock_target",
    (void*) &omp_unset_lock_lrb,
    "omp_test_lock_target",
    (void*) &omp_test_lock_lrb,

    "omp_init_nest_lock_target",
    (void*) &omp_init_nest_lock_lrb,
    "omp_destroy_nest_lock_target",
    (void*) &omp_destroy_nest_lock_lrb,
    "omp_set_nest_lock_target",
    (void*) &omp_set_nest_lock_lrb,
    "omp_unset_nest_lock_target",
    (void*) &omp_unset_nest_lock_lrb,
    "omp_test_nest_lock_target",
    (void*) &omp_test_nest_lock_lrb,

    (const char*) -1,
    (void*) -1
};

static FuncList::Node predefined_table = {
    { predefined_entries, -1 },
    0, 0
};

// Entry table
FuncList __offload_entries(&predefined_table);
#else
FuncList __offload_entries;
#endif // !HOST_LIBRARY

// Function table. No predefined entries.
FuncList __offload_funcs;

// Var table
VarList  __offload_vars;

// Given the function name returns the associtated function pointer
const void* FuncList::find_addr(const char *name)
{
    const void* func = 0;

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->name != 0 && strcmp(e->name, name) == 0) {
                func = e->func;
                break;
            }
        }
    }

    m_lock.unlock();

    return func;
}

// Given the function pointer returns the associtated function name
const char* FuncList::find_name(const void *func)
{
    const char* name = 0;

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->func == func) {
                name = e->name;
                break;
            }
        }
    }

    m_lock.unlock();

    return name;
}

// Returns max name length from all tables
int64_t FuncList::max_name_length(void)
{
    if (m_max_name_len < 0) {
        m_lock.lock();

        m_max_name_len = 0;
        for (Node *n = m_head; n != 0; n = n->next) {
            if (n->table.max_name_len < 0) {
                n->table.max_name_len = 0;

                // calculate max name length in a single table
                for (const Table::Entry *e = n->table.entries;
                     e->name != (const char*) -1; e++) {
                    if (e->name != 0) {
                        size_t len = strlen(e->name) + 1;
                        if (n->table.max_name_len < len) {
                            n->table.max_name_len = len;
                        }
                    }
                }
            }

            // select max from all tables
            if (m_max_name_len < n->table.max_name_len) {
                m_max_name_len = n->table.max_name_len;
            }
        }

        m_lock.unlock();
    }
    return m_max_name_len;
}

// Debugging dump
void FuncList::dump(void)
{
    OFFLOAD_DEBUG_TRACE(2, "Function table:\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->name != 0) {
                OFFLOAD_DEBUG_TRACE(2, "%p %s\n", e->func, e->name);
            }
        }
    }

    m_lock.unlock();
}

// Debugging dump
void VarList::dump(void)
{
    OFFLOAD_DEBUG_TRACE(2, "Var table:\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->name != 0) {
#if HOST_LIBRARY
                OFFLOAD_DEBUG_TRACE(2, "%s %p %ld\n", e->name, e->addr,
                                    e->size);
#else  // HOST_LIBRARY
                OFFLOAD_DEBUG_TRACE(2, "%s %p\n", e->name, e->addr);
#endif // HOST_LIBRARY
            }
        }
    }

    m_lock.unlock();
}

//
int64_t VarList::table_size(int64_t &nelems)
{
    int64_t length = 0;

    nelems = 0;

    // calculate string table size and number of elements
    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->name != 0) {
                length += strlen(e->name) + 1;
                nelems++;
            }
        }
    }

    return nelems * sizeof(BufEntry) + length;
}

// copy table to the gven buffer
void VarList::table_copy(void *buf, int64_t nelems)
{
    BufEntry* elems = static_cast<BufEntry*>(buf);
    char*     names = reinterpret_cast<char*>(elems + nelems);

    // copy entries to buffer
    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->name != (const char*) -1; e++) {
            if (e->name != 0) {
                // name field contains offset to the name from the beginning
                // of the buffer
                elems->name = names - static_cast<char*>(buf);
                elems->addr = reinterpret_cast<intptr_t>(e->addr);

                // copy name to string table
                const char *name = e->name;
                while ((*names++ = *name++) != '\0');

                elems++;
            }
        }
    }
}

// patch name offsets in a buffer
void VarList::table_patch_names(void *buf, int64_t nelems)
{
    BufEntry* elems = static_cast<BufEntry*>(buf);
    for (int i = 0; i < nelems; i++) {
        elems[i].name += reinterpret_cast<intptr_t>(buf);
    }
}

// Adds given list element to the global lookup table list
extern "C" void __offload_register_tables(
    FuncList::Node *entry_table,
    FuncList::Node *func_table,
    VarList::Node *var_table
)
{
    OFFLOAD_DEBUG_TRACE(2, "Registering offload function entry table %p\n",
                           entry_table);
    __offload_entries.add_table(entry_table);

    OFFLOAD_DEBUG_TRACE(2, "Registering function table %p\n", func_table);
    __offload_funcs.add_table(func_table);

    OFFLOAD_DEBUG_TRACE(2, "Registering var table %p\n", var_table);
    __offload_vars.add_table(var_table);
}

// Removes given list element from the global lookup table list
extern "C" void __offload_unregister_tables(
    FuncList::Node *entry_table,
    FuncList::Node *func_table,
    VarList::Node *var_table
)
{
    OFFLOAD_DEBUG_TRACE(2, "Unregistering offload function entry table %p\n",
                           entry_table);
    __offload_entries.remove_table(entry_table);

    OFFLOAD_DEBUG_TRACE(2, "Unregistering function table %p\n", func_table);
    __offload_funcs.remove_table(func_table);

    OFFLOAD_DEBUG_TRACE(2, "Unregistering var table %p\n", var_table);
    __offload_vars.remove_table(var_table);
}

#ifdef MYO_SUPPORT

MYOVarTableList  __offload_myo_var_tables;
MYOVarTableList  __offload_myo_vtable_tables;
MYOFuncTableList __offload_myo_func_tables;
MYOInitTableList __offload_myo_init_tables;

// Debugging dump
void MYOVarTableList::dump(void)
{
    OFFLOAD_DEBUG_TRACE(2, "MYO Var tables:\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        OFFLOAD_DEBUG_TRACE(2, "    MYO Var table:\n");
        for (const Table::Entry *e = n->table.entries;
             e->varName != MYO_TABLE_END_MARKER(); e++) {
#ifdef TARGET_WINNT
            if (e->varName == 0) {
                continue;
            }
#endif // TARGET_WINNT
            OFFLOAD_DEBUG_TRACE(2, "        %s %p\n",
                e->varName, e->sharedAddr);
        }
    }

    m_lock.unlock();
}

// check if any shared variables
bool MYOVarTableList::is_empty()
{
    OFFLOAD_DEBUG_TRACE(3, "Are MYO Var tables empty?\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
             e->varName != MYO_TABLE_END_MARKER(); e++) {
#ifdef TARGET_WINNT
            if (e->varName == 0) {
                continue;
            }
#endif // TARGET_WINNT
            m_lock.unlock();
            OFFLOAD_DEBUG_TRACE(3, "No\n");
            return false;
        }
    }

    m_lock.unlock();
    OFFLOAD_DEBUG_TRACE(3, "Yes\n");
    return true;
}

void MYOFuncTableList::dump(void)
{
    OFFLOAD_DEBUG_TRACE(2, "MYO Func tables:\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        OFFLOAD_DEBUG_TRACE(2, "    MYO Func table:\n");
        for (const Table::Entry *e = n->table.entries;
             e->funcName != MYO_TABLE_END_MARKER(); e++) {
#ifdef TARGET_WINNT
            if (e->funcName == 0) {
                continue;
            }
#endif // TARGET_WINNT
#if HOST_LIBRARY
            OFFLOAD_DEBUG_TRACE(2, "        %s %p %p\n",
                e->funcName, e->funcAddr, e->localThunkAddr);
#else // HOST_LIBRARY
            OFFLOAD_DEBUG_TRACE(2, "        %s %p %p %p\n",
                e->funcName, e->funcAddr, e->wrapFuncAddr, e->localThunkAddr);
#endif // HOST_LIBRARY
        }
    }

    m_lock.unlock();
}

// check if any shared functions
bool MYOFuncTableList::is_empty()
{
    OFFLOAD_DEBUG_TRACE(3, "Are MYO Func tables empty?\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        int count = 0;
        for (const Table::Entry *e = n->table.entries;
             e->funcName != MYO_TABLE_END_MARKER(); e++) {
#ifdef TARGET_WINNT
            if (e->funcName == 0) {
                continue;
            }
#endif // TARGET_WINNT
            count++;
            if (count > 1) {
                m_lock.unlock();
                OFFLOAD_DEBUG_TRACE(3, "No\n");
                return false;
            }
        }
    }

    m_lock.unlock();
    OFFLOAD_DEBUG_TRACE(3, "Yes\n");
    return true;
}

void MYOInitTableList::dump(void)
{
    OFFLOAD_DEBUG_TRACE(2, "MYO Init tables:\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        OFFLOAD_DEBUG_TRACE(2, "    MYO Init table:\n");
        for (const Table::Entry *e = n->table.entries;
#ifdef TARGET_WINNT
             e->funcName != MYO_TABLE_END_MARKER(); e++) {
            if (e->funcName == 0) {
                continue;
            }
            OFFLOAD_DEBUG_TRACE(2, "        %s %p\n", e->funcName, e->func);
#else // TARGET_WINNT
             e->func != 0; e++) {
            OFFLOAD_DEBUG_TRACE(2, "        %p\n", e->func);
#endif // TARGET_WINNT
        }
    }

    m_lock.unlock();
}

// check if any shared functions
bool MYOInitTableList::is_empty()
{
    OFFLOAD_DEBUG_TRACE(3, "Are MYO Init tables empty?\n");

    m_lock.lock();

    for (Node *n = m_head; n != 0; n = n->next) {
        for (const Table::Entry *e = n->table.entries;
#ifdef TARGET_WINNT
             e->funcName != MYO_TABLE_END_MARKER(); e++) {
            if (e->funcName == 0) {
                continue;
            }
            m_lock.unlock();
            OFFLOAD_DEBUG_TRACE(3, "No\n");
            return false;
#else // TARGET_WINNT
             e->func != 0; e++) {
#endif // TARGET_WINNT
        }
    }

    m_lock.unlock();
    OFFLOAD_DEBUG_TRACE(3, "Yes\n");
    return true;
}

extern "C" void __offload_myoRegisterTables1(
    MYOInitTableList::Node *init_table,
    MYOVarTableList::Node  *shared_table,
    MYOVarTableList::Node  *shared_vtable,
    MYOFuncTableList::Node *fptr_table
)
{
    OFFLOAD_DEBUG_TRACE(2, "Registering MYO shared var table %p\n",
                        shared_table);
    __offload_myo_var_tables.add_table(shared_table);

    OFFLOAD_DEBUG_TRACE(2, "Registering MYO shared vtable table %p\n",
                        shared_vtable);
    __offload_myo_vtable_tables.add_table(shared_vtable);

    OFFLOAD_DEBUG_TRACE(2, "Registering MYO function table %p\n", fptr_table);
    __offload_myo_func_tables.add_table(fptr_table);

    OFFLOAD_DEBUG_TRACE(2, "Registering MYO init table %p\n", init_table);
    __offload_myo_init_tables.add_table(init_table);
}

extern "C" void __offload_myoRemoveTables(
    MYOInitTableList::Node *init_table,
    MYOVarTableList::Node  *shared_table,
    MYOVarTableList::Node  *shared_vtable,
    MYOFuncTableList::Node *fptr_table
)
{
    OFFLOAD_DEBUG_TRACE(3, "%s\n", __func__);

    OFFLOAD_DEBUG_TRACE(2, "Removing MYO shared var table %p\n",
                        shared_table);
    __offload_myo_var_tables.remove_table(shared_table);

    OFFLOAD_DEBUG_TRACE(2, "Removing MYO shared vtable table %p\n",
                        shared_vtable);
    __offload_myo_vtable_tables.remove_table(shared_vtable);

    OFFLOAD_DEBUG_TRACE(2, "Removing MYO function table %p\n", fptr_table);
    __offload_myo_func_tables.remove_table(fptr_table);

    OFFLOAD_DEBUG_TRACE(2, "Removing MYO init table %p\n", init_table);
    __offload_myo_init_tables.remove_table(init_table);
}

#endif // MYO_SUPPORT
