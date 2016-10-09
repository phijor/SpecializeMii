#ifndef CFLDB_H
#define CFLDB_H

#include "mii.h"

#include <3ds/result.h>
#include <3ds/types.h>
#include <3ds/services/fs.h>

#define CFLDB_ERR_NULLPTR                                                      \
    MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_POINTER)

#define CFLDB_ERR_NO_MEM                                                       \
    MAKERESULT(RL_PERMANENT, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY)

#define CFLDB_ERR_WRONG_SIZE                                                   \
    MAKERESULT(RL_PERMANENT, RS_INVALIDSTATE, RM_APPLICATION, RD_INVALID_SIZE)

#define CFLDB_DATA_AT(db, offset) ((void *) ((u8 *) ((db).data)) + (offset))

typedef struct CFL_DB_t {
    void *data;
    u64 size;
    FS_Archive archive;
    Handle fhandle;
} CFL_DB;

Result cfldb_open(CFL_DB *const db);
Result cfldb_close(CFL_DB *const db);

Result cfldb_read(CFL_DB *const db);
Result cfldb_write(CFL_DB *const db);

u16 cfldb_get_checksum(CFL_DB const *const db);
void cfldb_set_checksum(CFL_DB *const db, u16 const checksum);

int cfldb_check_integrity(CFL_DB const *const db);
u16 cfldb_calculate_checksum(CFL_DB const *const db);
u16 cfldb_fix_checksum(CFL_DB *const db);

size_t cfldb_get_mii_count(CFL_DB const *const db);
Mii *cfldb_get_mii_array(CFL_DB const *const db);

#endif /* ----- #ifndef CFLDB_H ----- */
